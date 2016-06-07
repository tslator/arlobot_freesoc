/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
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

static CY_ISR(Front_EOC_Interrupt)
/* This interrupt occurs when the ADC completes the conversion
       Clear the interrupt
       Read value from ADC
       Calculate distance
       Store distance
 */
{
    Front_ADC_SAR_IRQ_ClearPending();
    
    int16 counts = Front_ADC_SAR_GetResult16();
    float voltage = Front_ADC_SAR_CountsTo_Volts(counts);    
    voltage = constrain(voltage, ADC_MIN_VALUE, ADC_MAX_VALUE);
    uint8 distance = CALCULATE_DISTANCE(voltage);
    I2c_WriteFrontInfraredDistance(front_adc_last_channel, distance);    
}

static CY_ISR(Rear_EOC_Interrupt)
/* This interrupt occurs when the ADC completes the conversion
       Clear the interrupt
       Read value from ADC
       Calculate distance
       Store distance
 */
{
    Rear_ADC_SAR_IRQ_ClearPending();
    
    int16 counts = Rear_ADC_SAR_GetResult16();
    float voltage = Rear_ADC_SAR_CountsTo_Volts(counts);
    voltage = constrain(voltage, ADC_MIN_VALUE, ADC_MAX_VALUE);
    uint8 distance = CALCULATE_DISTANCE(voltage);
    I2c_WriteRearInfraredDistance(rear_adc_last_channel, distance);    
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

void Infrared_Test()
{
}


/* [] END OF FILE */
