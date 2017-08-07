/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <math.h>
#include <stdio.h>
#include "config.h"
#include "infrared.h"
#include "debug.h"
#include "time.h"
#include "i2c.h"
#include "utils.h"

/* The Sharp IR sensors have a range of 10 to 80 cm.  Working backward, this limits the ADC values as follows:

ADC    Voltage        Pow          cm           mm
327  0.399169922  2.875203044  80.10315682  801.0315682
350  0.427246094  2.659011307  74.08005503  740.8005503
375  0.457763672  2.456192932  68.42953508  684.2953508
...
1950 2.380371094  0.368857512  10.27637027  102.7637027
1995 2.435302734  0.359305694  10.01025665  100.1025665
 */
#define ADC_MIN_VALUE (327)
#define ADC_MAX_VALUE (1995)

/* Interpolation equation and constants taken from here: 
       https://www.tindie.com/products/upgradeindustries/sharp-10-80cm-infrared-distance-sensor-gp2y0a21yk0f/
 */
#define SHARP_IR_SENSOR_INTERPOLATION_CONST_1 (27.86)
#define SHARP_IR_SENSOR_INTERPOLATION_CONST_2 (-1.15)

#define CALCULATE_DISTANCE(voltage) ((uint8) (SHARP_IR_SENSOR_INTERPOLATION_CONST_1 * pow(voltage, SHARP_IR_SENSOR_INTERPOLATION_CONST_2)))

/* Two interrupts are used: EOS (end-of-sampling) and EOC (end-of-conversion)

    - The EOS interrupt occurs before the EOC interrupt
    - The ADC sampling can be changed while the previous conversion is being performed
    - When the EOS interrupt occurs, the mux is switched to the next change for sampling which gives some amount of
      overlap between sampling and conversion.
    - The EOS interrupt advanced the mux channel, so when the EOC interrupt occurs, it is necessary to use the previous
      channel.

*/
static uint8 rear_adc_last_channel;
static uint8 front_adc_last_channel;

typedef struct queueItem_s
{
    uint8 channel;
    float voltage;
} queueItem_t;

typedef void (*WRITE_DISTANCE_TYPE)(uint8 offset, uint8 distance);
typedef void (*LOCK_TYPE)(void);
typedef void (*UNLOCK_TYPE)(void);

typedef struct circularQueue_s
{
    int first;
    int last;
    int validItems;
    int maxItems;
    int *p_items;
    queueItem_t *p_data;
    LOCK_TYPE lock;
    UNLOCK_TYPE unlock;
    WRITE_DISTANCE_TYPE Report;        
} circularQueue_t;

static circularQueue_t front_queue;
static int front_queue_items[8];
static queueItem_t front_queue_data[8];

static circularQueue_t rear_queue;
static int rear_queue_items[8];
static queueItem_t rear_queue_data[8];

void initializeQueue(circularQueue_t *theQueue, int *items_buffer, queueItem_t *data_buffer)
{
    theQueue->validItems = 0;
    theQueue->first      = 0;
    theQueue->last       = 0;
    theQueue->p_items    = items_buffer;
    theQueue->p_data     = data_buffer;
}
 
int isEmpty(circularQueue_t *theQueue)
{
    int result;
    
    theQueue->lock();
    result = theQueue->validItems == 0;
    theQueue->unlock();
    return result;
}
 
int putItem(circularQueue_t *theQueue, int theItemValue)
{
    theQueue->lock();
    if ( theQueue->validItems>=theQueue->maxItems )
    {
        theQueue->unlock();
        return 0;
    }

    theQueue->validItems++;
    theQueue->p_items[theQueue->last] = theItemValue;
    theQueue->last = (theQueue->last+1) % theQueue->maxItems;
    
    theQueue->unlock();
    
    return 1;
}
 
int getItem(circularQueue_t *theQueue, int *theItemValue)
{
    if(isEmpty(theQueue))
    {
        return 0;
    }
    
    theQueue->lock();
    *theItemValue = theQueue->p_items[theQueue->first];
    theQueue->first = (theQueue->first+1)%theQueue->maxItems;
    theQueue->validItems--;
    theQueue->unlock();
    return 1;
}
 
void printQueue(circularQueue_t *theQueue)
{
    int index, num_valid_items;
    theQueue->lock();
    index  = theQueue->first;
    num_valid_items = theQueue->validItems;
    theQueue->unlock();
    while(num_valid_items>0)
    {
        theQueue->lock();
        int entry = theQueue->p_items[index];
        index=(index+1)%theQueue->maxItems;
        theQueue->unlock();
        num_valid_items--;
        DEBUG_PRINT_ARG("Element #%d = %d\n", index, entry);
    }
    return;
}

static CY_ISR(Front_EOC_Interrupt)
/* This interrupt occurs when the ADC completes the conversion
       Clear the interrupt
       Read value from ADC
       Store channel and counts
 */
{
    Front_ADC_SAR_IRQ_ClearPending();
    
    int16 counts = Front_ADC_SAR_GetResult16();
    float voltage = Front_ADC_SAR_CountsTo_Volts(counts);
    front_queue.p_data[front_adc_last_channel].channel = front_adc_last_channel;
    front_queue.p_data[front_adc_last_channel].voltage = voltage;
    putItem(&front_queue, front_adc_last_channel);
}

static CY_ISR(Rear_EOC_Interrupt)
/* This interrupt occurs when the ADC completes the conversion
       Clear the interrupt
       Read value from ADC
       Store channel and counts
 */
{
    Rear_ADC_SAR_IRQ_ClearPending();
    
    int16 counts = Rear_ADC_SAR_GetResult16();
    float voltage = Rear_ADC_SAR_CountsTo_Volts(counts);
    rear_queue.p_data[rear_adc_last_channel].channel = rear_adc_last_channel;
    rear_queue.p_data[rear_adc_last_channel].voltage = voltage;
    putItem(&rear_queue, rear_adc_last_channel);
}

static CY_ISR(Front_EOS_Interrupt)
/* This interrupt occurs when the ADC finishes sampling the input
       Clear the interrupt
       Save the current channel so the EOC interrupt can read the correct channel
       Connect to the next channel
       Start ADC conversion
 */
{
    Front_EOS_Intr_ClearPending();
    front_adc_last_channel = Front_AMuxSeq_GetChannel();
    Front_AMuxSeq_Next();
    Front_ADC_SAR_StartConvert();
}

static CY_ISR(Rear_EOS_Interrupt)
/* This interrupt occurs when the ADC finishes sampling the input
       Clear the interrupt
       Save the current channel so the EOC interrupt can read the correct channel
       Connect to the next channel
       Start ADC conversion
 */
{
    Rear_EOS_Intr_ClearPending();
    rear_adc_last_channel = Rear_AMuxSeq_GetChannel();
    Rear_AMuxSeq_Next();
    Rear_ADC_SAR_StartConvert();
}

void Infrared_Init()
{
    Front_AMuxSeq_DisconnectAll();
    Rear_AMuxSeq_DisconnectAll();

    initializeQueue(&front_queue, &front_queue_items[0], &front_queue_data[0]);
    initializeQueue(&rear_queue, &rear_queue_items[0], &rear_queue_data[0]);
}

void Infrared_Start()
{
    Front_ADC_SAR_IRQ_StartEx(Front_EOC_Interrupt);
    Rear_ADC_SAR_IRQ_StartEx(Rear_EOC_Interrupt);
    
    Front_EOS_Intr_StartEx(Front_EOS_Interrupt);
    Rear_EOS_Intr_StartEx(Rear_EOS_Interrupt);
    
    Front_AMuxSeq_Start();
    Rear_AMuxSeq_Start();
    
    /* Switching/connecting the Mux before starting the ADC gives additional time for settling.  But, it may not make
       any difference either.
     */
    Front_AMuxSeq_Next();
    Rear_AMuxSeq_Next();
    
    Front_ADC_SAR_Start();
    Rear_ADC_SAR_Start();
    
    Front_ADC_SAR_StartConvert();
    Rear_ADC_SAR_StartConvert();
}

void Infrared_Update()
{
    int index;
    while (getItem(&front_queue, &index))
    {
        float voltage = front_queue.p_data[index].voltage;
        uint8 channel = front_queue.p_data[index].channel;
        uint8 distance = CALCULATE_DISTANCE(constrain(voltage, ADC_MIN_VALUE, ADC_MAX_VALUE));
        front_queue.Report(channel, distance);
    }
    
    while (getItem(&rear_queue, &index))
    {
        float voltage = rear_queue_data[index].voltage;
        uint8 channel = rear_queue_data[index].channel;
        uint8 distance = CALCULATE_DISTANCE(constrain(voltage, ADC_MIN_VALUE, ADC_MAX_VALUE));
        rear_queue.Report(channel, distance);
    }
}

void Infrared_Test()
{
}


/* [] END OF FILE */
