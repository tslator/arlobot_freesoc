#include <stdio.h>
#include "unity.h"
#include "mock_serial.h"
#include "mock_utils.h"
#include "mock_nvstore.h"
#include "mock_control.h"
#include "mock_debug.h"
#include "mock_calmotor.h"
#include "mock_calpid.h"
#include "mock_callin.h"
#include "mock_calang.h"
#include "mock_valmotor.h"
#include "mock_valpid.h"
#include "mock_vallin.h"
#include "mock_valang.h"
#include "mock_assertion.h"
#include "mock_usbif.h"
#include "cal.h"


/* Helper Macros */

#define DEFINE_START_TEST(text, enabled) \
void test_##text(void) \
{ \
    if (!enabled) \
    { \
        printf("DISABLED: %s\n", #text);\
        return; \
    } \
    \
    printf("ENTERING %s\n", #text); \

#define DEFINE_END_TEST(text) \
    printf("EXITING (Successful): %s\n", #text); \
}


/* Helper Functions */

void CalMenuOutput(void)
{
    Ser_PutString_Expect("\r\nWelcome to the Arlobot calibration interface.\r\n");
    Ser_PutString_Expect("The following calibration operations are allowed:\r\n");
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutString_Expect("\r\n");
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutString_Expect("\r\n");
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
}

void ValMenuOutput(void)
{
    Ser_PutString_Expect("\r\nWelcome to the Arlobot validation interface.\r\n");
    Ser_PutString_Expect("The following validation operations are allowed:\r\n");
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
}

void SettingsMenuOutput(void)
{
    Ser_PutString_Expect("\r\nWelcome to the Arlobot settings display\r\n");
    Ser_PutString_Expect("The following settings can be displayed\r\n");
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();
}

void CalValMenuExitOutput(void)
{
    Ser_PutString_Expect("\r\nExiting Arlobot calibration/validation interface.");
    Ser_PutStringFormat_Expect("");
    Ser_PutStringFormat_IgnoreArg_fmt();        
    Control_OverrideDebug_Expect(FALSE);
}

UINT8 Failed_Ser_ReadLine(char *line, UINT8 echo, UINT8 max_length, int call_count)
{
    printf("Failing call to Ser_ReadLine\n");
    return 0;
}


UINT8 EnterExitReadLineReturn(char c, char *line, int call_count)
{
    UINT8 return_value;

    switch (call_count)
    {
        case 0:
            line[0] = c;
            line[1] = '\r';
            return_value = 1;
            break;

        case 1:
        default:
            line[0] = 'x';
            line[1] = '\r';
            return_value = 1;
            break;
    }

    return return_value;
}

UINT8 CalibrationEnterExit_Ser_ReadLine(char *line, UINT8 echo, UINT8 max_length, int call_count)
{
    EnterExitReadLineReturn('c', line, call_count);
}

UINT8 ValidationEnterExit_Ser_ReadLine(char *line, UINT8 echo, UINT8 max_length, int call_count)
{
    EnterExitReadLineReturn('v', line, call_count);
}

UINT8 SettingsEnterExit_Ser_ReadLine(char *line, UINT8 echo, UINT8 max_length, int call_count)
{
    EnterExitReadLineReturn('d', line, call_count);
}

void setUp(void)
{
}

void tearDown(void)
{
}


/*-------------------------------------------------------------------------------------------------
    Cal_Update
 */


/*-------------------------------------------------------------------------------------------------
    Top Level Commands
        Failed Serial Read Line
        Calibration
        Validation
        Display
 */

DEFINE_START_TEST(WhenAnyMenuRequestAndZeroSerialReadLineResult_ThenNoMenuDisplayed, TRUE)
{
    Ser_ReadLine_StubWithCallback(Failed_Ser_ReadLine);

    // When
    Cal_Update();

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenAnyMenuRequestAndZeroSerialReadLineResult_ThenNoMenuDisplayed)

DEFINE_START_TEST(WhenRequestCalibrationMenu_ThenCalibrationMenuDisplayed, TRUE)
{
    Ser_ReadLine_StubWithCallback(CalibrationEnterExit_Ser_ReadLine);
    CalMenuOutput();
    CalValMenuExitOutput();

    // When
    Cal_Update();
    Cal_Update();
    Cal_Update();

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestCalibrationMenu_ThenCalibrationMenuDisplayed)

DEFINE_START_TEST(WhenRequestValidationMenu_ThenValidationMenuDisplayed, TRUE)
{
    Ser_ReadLine_StubWithCallback(ValidationEnterExit_Ser_ReadLine);
    ValMenuOutput();
    CalValMenuExitOutput();

    // When
    Cal_Update();    
    Cal_Update();    
    Cal_Update();    

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestValidationMenu_ThenValidationMenuDisplayed)

DEFINE_START_TEST(WhenRequestSettingsMenu_ThenSettingsMenuDisplayed, TRUE)
{
    printf("Start test_WhenRequestSettingsMenu_ThenSettingsMenuDisplayed\n");
    
    Ser_ReadLine_StubWithCallback(SettingsEnterExit_Ser_ReadLine);
    SettingsMenuOutput();
    CalValMenuExitOutput();

    // When
    Cal_Update();    
    Cal_Update();    
    Cal_Update();    

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestSettingsMenu_ThenSettingsMenuDisplayed)

DEFINE_START_TEST(WhenRequestExitMenu_ThenNoMenuDisplayed, TRUE)
{
    UINT8 mock_Ser_ReadLine(CHAR *line, UINT8 echo, UINT8 max_length, int call_count)
    {
        EnterExitReadLineReturn('x', line, call_count);
    }

    Ser_ReadLine_StubWithCallback(mock_Ser_ReadLine);

    // When
    Cal_Update();

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestExitMenu_ThenNoMenuDisplayed)

/*-------------------------------------------------------------------------------------------------
    Calibration Commands


 */

DEFINE_START_TEST(WhenRequestCalibrationAndFailedSerialReadLineOnCalibrationCommandMenu_ThenNoChange, TRUE)
{
    UINT8 EnterFailExitCalibration(CHAR *line, UINT8 echo, UINT8 max_length, int call_count)
    {
        if (call_count == 0)
        {
            line[0] = 'c';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 1)
        {
            return 0;
        }
        else if (call_count == 2)
        {
            line[0] = 'x';
            line[1] = '\r';
            return 1;
        }
    }


    Ser_ReadLine_StubWithCallback(EnterFailExitCalibration);
    CalMenuOutput();
    CalValMenuExitOutput();        

    // When
    Cal_Update();  // Receive 'c' to enter calibration
    Cal_Update();  // Receive null, remain in calibration state
    Cal_Update();  // Receive 'x', transition to exit state
    Cal_Update();  // Process exit state and display menu

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestCalibrationAndFailedSerialReadLineOnCalibrationCommandMenu_ThenNoChange)

DEFINE_START_TEST(WhenRequestOutOfRangeCalibrationCommand_ThenNullCmdReturned, TRUE)
{
    UINT8 EnterFailExitCalibration(CHAR *line, UINT8 echo, UINT8 max_length, int call_count)
    {
        if (call_count == 0)
        {
            line[0] = 'c';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 1)
        {
            line[0] = 'z';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 2)
        {
            line[0] = 'x';
            line[1] = '\r';
            return 1;
        }
    }


    Ser_ReadLine_StubWithCallback(EnterFailExitCalibration);
    CalMenuOutput();
    CalValMenuExitOutput();        
    
    // When
    Cal_Update();  // Receive 'c' to enter calibration
    Cal_Update();  // Receive 'z' (results in null command), remain in calibration state
    Cal_Update();  // Receive 'x', transition to exit state
    Cal_Update();  // Process exit state and display menu

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestOutOfRangeCalibrationCommand_ThenNullCmdReturned)

DEFINE_START_TEST(WhenRequestInRangeCalibrationCommandButFailedReturn_ThenNullCmdReturned, TRUE)
{
    UINT8 EnterFailExitCalibration(CHAR *line, UINT8 echo, UINT8 max_length, int call_count)
    {
        if (call_count == 0)
        {
            line[0] = 'c';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 1)
        {
            line[0] = '3';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 2)
        {
            line[0] = 'x';
            line[1] = '\r';
            return 1;
        }
    }


    Ser_ReadLine_StubWithCallback(EnterFailExitCalibration);
    CalMenuOutput();
    CalPid_Start_ExpectAndReturn(WHEEL_RIGHT, 0);
    CalValMenuExitOutput();        
    
    // When
    Cal_Update();  // Receive 'c' to enter calibration
    Cal_Update();  // Receive '3' (selects pid calibration for right wheel, but fails on return)
    Cal_Update();  // Receive 'x', transition to exit state
    Cal_Update();  // Process exit state and display menu

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestCalibrationCalibrationCommandOutOfRange_ThenNullCmdReturned)

DEFINE_START_TEST(WhenRequestInRangeCalibrationCommand_ThenCalibrationProcessed, TRUE)
{
    UINT8 EnterFailExitCalibration(CHAR *line, UINT8 echo, UINT8 max_length, int call_count)
    {
        if (call_count == 0)
        {
            line[0] = 'c';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 1)
        {
            line[0] = '1';
            line[1] = '\r';
            return 1;
        }
        else if (call_count == 2)
        {
            line[0] = 'x';
            line[1] = '\r';
            return 1;
        }
    }

    UINT8 Init(void)
    {
        return CAL_OK;
    }

    UINT8 Start(void)
    {
        return CAL_OK;
    }

    UINT8 Update(void)
    {
        return CAL_COMPLETE;
    }

    UINT8 Stop(void)
    {
        return CAL_OK;
    }

    UINT8 Results(void)
    {
        return CAL_OK;
    }

    CALVAL_INTERFACE_TYPE interface = { CAL_INIT_STATE, 
                                              CAL_CALIBRATE_STAGE,
                                              NULL,
                                              Init, 
                                              Start, 
                                              Update, 
                                              Stop, 
                                              Results };    


    Ser_ReadLine_StubWithCallback(EnterFailExitCalibration);
    CalMenuOutput();
    CalMotor_Start_ExpectAndReturn(&interface);
    Debug_DisableAll_Expect();
    CalMenuOutput();
    CalValMenuExitOutput();   
    
    // When
    Cal_Update();  // Receive 'c' to enter calibration
    Cal_Update();  // Receive '1' (selects motor calibration)
    Cal_Update();  // Process Init
    Cal_Update();  // Process Start
    Cal_Update();  // Process Update
    Cal_Update();  // Process Stop
    Cal_Update();  // Process Results
    Cal_Update();  // Receive 'x', transition to exit state
    Cal_Update();  // Process exit state and display menu

    // Note: There is no macro assert because there is no return value to check
}
DEFINE_END_TEST(WhenRequestInRangeCalibrationCommand_ThenCalibrationProcessed)

DEFINE_START_TEST(WhenReadingSingleCharResponse_ThenCorrectResponseReturned, TRUE)
{
    UINT8 EnterFailExitCalibration(CHAR *line, UINT8 echo, UINT8 max_length, int call_count)
    {
        line[0] = '1';
        line[1] = '\r';
        return 1;
    }

    FLOAT result;

    Ser_ReadLine_StubWithCallback(EnterFailExitCalibration);
    USBIF_Update_Expect();


    // When
    result = Cal_ReadResponse();

    // Then
    TEST_ASSERT_EQUAL_FLOAT(1, result);
}
DEFINE_END_TEST(WhenReadingSingleCharResponse_ThenCorrectResponseReturned)

/* Tests Remaining

Cal_ReadResponse
    - Test returning 1 digit
    - Test returning 10 digits
    - Test returning 20 digits
    - Test returning length > 0 but null data
    - Test returning length = 0 but non-null data
Cal_CpsToPwm
Cal_CalcForwardOperatingRange
Cal_CalcBackwardOperatingRange
Cal_CalcTriangularProfile

*/