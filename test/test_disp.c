#include <stdio.h>
#include <stdarg.h>
#include "unity.h"
#include "disp.h"
#include "concmd.h"
#include "mock_dispconfig.h"
#include "mock_dispmotor.h"
#include "mock_disppid.h"
#include "mock_dispmotion.h"
#include "mock_concmdif.h"


static COMMAND_TYPE cmd;


void setUp(void)
{
    DispConfig_Init_Expect();
    DispMotor_Init_Expect();
    DispPid_Init_Expect();
    DispMotion_Init_Expect();
    DispConfig_Start_Expect();
    DispMotor_Start_Expect();
    DispPid_Start_Expect();
    DispMotion_Start_Expect();

    Disp_Init();
    Disp_Start();

    memset(&cmd, 0, sizeof cmd);
}

void tearDown(void)
{
}

/* Test Valid/Invalid Commands and associated is_valid/not is_valid */

void test_WhenConCmdInterfaceIsSet_ThenCmdIsValid(void)
{
    ConCmdIf_GetIfacePtr_IgnoreAndReturn(TRUE);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenConCmdInterfaceIsNotSet_ThenCmdIsValid(void)
{
    ConCmdIf_GetIfacePtr_IgnoreAndReturn(FALSE);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

void test_EachPathThroughDispatch_WhenIsValidIsTrueAndFalse(void)
{
    TEST_IGNORE_MESSAGE("Consider whether these tests are worthwhile");
}