#include "build/temp/_test_motor.c"
#include "motor.h"
#include "mock_control.h"
#include "mock_serial.h"
#include "mock_debug.h"
#include "mock_Right_HB25_PWM.h"
#include "mock_Left_HB25_PWM.h"
#include "mock_Right_HB25_Enable_Pin.h"
#include "mock_Left_HB25_Enable_Pin.h"
#include "mock_project.h"
#include "mock_CyLib.h"
#include "mock_cyapicallbacks.h"
#include "unity.h"


void setUp(void)

{

}



void tearDown(void)

{

}



void test_WhenMotorInit_ThenSuccessful(void)

{

    Left_HB25_Enable_Pin_Write_CMockExpect(25, 1);

    Right_HB25_Enable_Pin_Write_CMockExpect(26, 1);





    Motor_Init();

}



void test_WhenMotorStart_ThenSuccessful(void)

{

    Left_HB25_Enable_Pin_Write_CMockExpect(34, 0);

    Left_HB25_PWM_Start_CMockExpect(35);

    Left_HB25_PWM_WriteCompare_CMockExpect(36, 1500);

    Right_HB25_Enable_Pin_Write_CMockExpect(37, 0);

    Right_HB25_PWM_Start_CMockExpect(38);

    Right_HB25_PWM_WriteCompare_CMockExpect(39, 1500);



    Control_SetDeviceStatusBit_CMockExpect(41, (0x0001));





    Motor_Start();

}



void test_WhenLeftPwmIs1000_ThenExpectedPwmValue(void)

{

    Left_HB25_PWM_WriteCompare_CMockExpect(49, 1000);

    Debug_IsEnabled_CMockExpectAndReturn(50, 0x0030, 1);

    Left_HB25_PWM_ReadCompare_CMockExpectAndReturn(51, 1000);

    Ser_PutString_CMockExpect(52, "{\"left motor\": {\"pwm\":1000}}\r\n");





    Motor_LeftSetPwm(1000);

}



void test_WhenRightPwmIs1000_ThenExpectedPwmValue(void)

{

    Right_HB25_PWM_WriteCompare_CMockExpect(60, 1000);

    Debug_IsEnabled_CMockExpectAndReturn(61, 0x0030, 1);

    Right_HB25_PWM_ReadCompare_CMockExpectAndReturn(62, 1000);

    Ser_PutString_CMockExpect(63, "{\"right motor\": {\"pwm\":1000}}\r\n");





    Motor_RightSetPwm(1000);

}



void test_WhenLeftRequestPwm_ThenExpectedPwmValue(void)

{

    UINT16 result;



    Left_HB25_PWM_ReadCompare_CMockExpectAndReturn(73, 1500);





    result = Motor_LeftGetPwm();





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((1500)), (UNITY_INT)(UNITY_UINT16)((result)), (((void *)0)), (UNITY_UINT)(79), UNITY_DISPLAY_STYLE_UINT16);

}



void test_WhenRightRequestPwm_ThenExpectedPwmValue(void)

{

    UINT16 result;



    Right_HB25_PWM_ReadCompare_CMockExpectAndReturn(86, 1500);





    result = Motor_RightGetPwm();





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((1500)), (UNITY_INT)(UNITY_UINT16)((result)), (((void *)0)), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_UINT16);

}



void test_WhenLeftRightRequestPwm_TheExpectedPwmValues(void)

{

    UINT16 left;

    UINT16 right;



    Left_HB25_PWM_ReadCompare_CMockExpectAndReturn(100, 2000);

    Right_HB25_PWM_ReadCompare_CMockExpectAndReturn(101, 2000);





    Motor_GetPwm(&left, &right);





    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((2000)), (UNITY_INT)(UNITY_UINT16)((left)), (((void *)0)), (UNITY_UINT)(107), UNITY_DISPLAY_STYLE_UINT16);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((2000)), (UNITY_INT)(UNITY_UINT16)((right)), (((void *)0)), (UNITY_UINT)(108), UNITY_DISPLAY_STYLE_UINT16);

}



void test_WhenMotorStop_ThenSuccessful(void)

{

    Left_HB25_PWM_WriteCompare_CMockExpect(113, 1500);

    Right_HB25_PWM_WriteCompare_CMockExpect(114, 1500);

    Left_HB25_PWM_Stop_CMockExpect(115);

    Right_HB25_PWM_Stop_CMockExpect(116);

    Left_HB25_Enable_Pin_Write_CMockExpect(117, 1);

    Right_HB25_Enable_Pin_Write_CMockExpect(118, 1);

    Control_ClearDeviceStatusBit_CMockExpect(119, (0x0001));





    Motor_Stop();

}
