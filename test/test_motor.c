#include <stdio.h>
#include "unity.h"
#include "mock_cyapicallbacks.h"
#include "mock_CyLib.h"
#include "mock_project.h"
#include "mock_Left_HB25_Enable_Pin.h"
#include "mock_Right_HB25_Enable_Pin.h"
#include "mock_Left_HB25_PWM.h"
#include "mock_Right_HB25_PWM.h"
#include "mock_debug.h"
#include "mock_control.h"
#include "motor.h"

void setUp(void)
{
    Debug_GetLevel_IgnoreAndReturn(DBG_INFO);
    Debug_Print_Ignore();
}

void tearDown(void)
{
}

void test_WhenMotorInit_ThenSuccessful(void)
{
    Left_HB25_Enable_Pin_Write_Expect(1);
    Right_HB25_Enable_Pin_Write_Expect(1);
    
    // When
    Motor_Init();
}

void test_WhenMotorStart_ThenSuccessful(void)
{
    Left_HB25_Enable_Pin_Write_Expect(0);
    Left_HB25_PWM_Start_Expect();
    Left_HB25_PWM_WriteCompare_Expect(1500);
    Right_HB25_Enable_Pin_Write_Expect(0);
    Right_HB25_PWM_Start_Expect();
    Right_HB25_PWM_WriteCompare_Expect(1500);
    
    Control_SetDeviceStatusBit_Expect(STATUS_HB25_CNTRL_INIT_BIT);
    
    // When
    Motor_Start();
}

void test_WhenLeftPwmIs1000_ThenExpectedPwmValue(void)
{
    Left_HB25_PWM_WriteCompare_Expect(1000);
    Debug_IsEnabled_ExpectAndReturn(DEBUG_LEFT_MOTOR_ENABLE_BIT, 1);
    Left_HB25_PWM_ReadCompare_ExpectAndReturn(1000);
    
    // When
    Motor_LeftSetPwm(1000);
}

void test_WhenRightPwmIs1000_ThenExpectedPwmValue(void)
{
    Right_HB25_PWM_WriteCompare_Expect(1000);
    Debug_IsEnabled_ExpectAndReturn(DEBUG_RIGHT_MOTOR_ENABLE_BIT, 1);
    Right_HB25_PWM_ReadCompare_ExpectAndReturn(1000);
    
    // When
    Motor_RightSetPwm(1000);
}

void test_WhenLeftRequestPwm_ThenExpectedPwmValue(void)
{
    UINT16 result;
    
    Left_HB25_PWM_ReadCompare_ExpectAndReturn(1500);
    
    // When
    result = Motor_LeftGetPwm();
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(1500, result);
}

void test_WhenRightRequestPwm_ThenExpectedPwmValue(void)
{
    UINT16 result;
    
    Right_HB25_PWM_ReadCompare_ExpectAndReturn(1500);
    
    // When
    result = Motor_RightGetPwm();
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(1500, result);
}

void test_WhenLeftRightRequestPwm_TheExpectedPwmValues(void)
{
    UINT16 left;
    UINT16 right;
    
    Left_HB25_PWM_ReadCompare_ExpectAndReturn(2000);
    Right_HB25_PWM_ReadCompare_ExpectAndReturn(2000);
    
    // When
    Motor_GetPwm(&left, &right);
    
    // Then
    TEST_ASSERT_EQUAL_UINT16(2000, left);
    TEST_ASSERT_EQUAL_UINT16(2000, right);
}

void test_WhenMotorStop_ThenSuccessful(void)
{
    Left_HB25_PWM_WriteCompare_Expect(1500);
    Right_HB25_PWM_WriteCompare_Expect(1500);
    Left_HB25_PWM_Stop_Expect();
    Right_HB25_PWM_Stop_Expect();
    Left_HB25_Enable_Pin_Write_Expect(1);
    Right_HB25_Enable_Pin_Write_Expect(1);
    Control_ClearDeviceStatusBit_Expect(STATUS_HB25_CNTRL_INIT_BIT);
    
    // When
    Motor_Stop();
}