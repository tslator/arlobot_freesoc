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

#include "config.h"
#include "ultrasonic.h"
#include "i2c.h"
#include "utils.h"
#include "time.h"
#include "debug.h"

#define MIN_PULSE_US (114)
#define MAX_PULSE_US (23200)

#define TRIGGER_PULSE_WIDTH (10)
#define US_PER_CENTIMETER (58)

#define TIMER_CLEAR_BIT (0x02)
#define TRIGGER_BIT     (0x01)

#define MAX_TIMER_COUNT (3000)


static void Front_Trigger_Next()
{
    static uint8 front_trigger_index = 0;
    
    Front_Trigger_Select_Reg_Write(front_trigger_index);
    front_trigger_index = (front_trigger_index + 1) % NUM_FRONT_ULTRASONIC_SENSORS;
    
    Front_Trigger_Reg_Write(0);
    Front_Trigger_Reg_Write(TIMER_CLEAR_BIT | TRIGGER_BIT);
    CyDelay(TRIGGER_PULSE_WIDTH);
    Front_Trigger_Reg_Write(0);
}

static void Rear_Trigger_Next()
{
    static uint8 rear_trigger_index = 0;
    
    Rear_Trigger_Select_Reg_Write(rear_trigger_index);
    rear_trigger_index = (rear_trigger_index + 1) % NUM_REAR_ULTRASONIC_SENSORS;
    
    Rear_Trigger_Reg_Write(0);
    Rear_Trigger_Reg_Write(TIMER_CLEAR_BIT | TRIGGER_BIT);
    CyDelay(TRIGGER_PULSE_WIDTH);
    Rear_Trigger_Reg_Write(0);
}

static CY_ISR( Front_Echo_Interrupt )
{
    Front_Echo_Intr_ClearPending();
    
    uint16 count = MAX_TIMER_COUNT - Front_Echo_Timer_ReadCounter();
    count = constrain(count, MIN_PULSE_US, MAX_PULSE_US);
    uint8 distance = (uint8) (count / US_PER_CENTIMETER);
    I2c_WriteFrontUltrasonicDistance(Front_Trigger_Select_Reg_Read(), distance);
    Front_Trigger_Next();
}

static CY_ISR( Rear_Echo_Interrupt )
{
    Rear_Echo_Intr_ClearPending();
    
    uint16 count = MAX_TIMER_COUNT - Rear_Echo_Timer_ReadCounter();
    count = constrain(count, MIN_PULSE_US, MAX_PULSE_US);
    uint8 distance = (uint8) (count / US_PER_CENTIMETER);
    I2c_WriteRearUltrasonicDistance(Rear_Trigger_Select_Reg_Read(), distance);
    Rear_Trigger_Next();
}

void Ultrasonic_Init()
{
}

void Ultrasonic_Start()
{
    Front_Echo_Intr_StartEx(Front_Echo_Interrupt);
    Rear_Echo_Intr_StartEx(Rear_Echo_Interrupt);
    
    Front_Echo_Timer_Start();
    Rear_Echo_Timer_Start();
    
    Front_Trigger_Next();
    Rear_Trigger_Next();
}

void Ultrasonic_Test()
{
}

/* [] END OF FILE */
