#include <stdio.h>
#include <stdarg.h>
#include "unity.h"
#include "disp.h"
#include "concmd.h"
#include "mock_conconfig.h"
#include "mock_conmotor.h"
#include "mock_conpid.h"
#include "mock_conmotion.h"


static COMMAND_TYPE cmd;
static CONCMD_IF_TYPE *p_concmd;
static CONCMD_IF_TYPE concmd;


void setUp(void)
{
    ConConfig_Init_Expect();
    ConMotor_Init_Expect();
    ConPid_Init_Expect();
    ConMotion_Init_Expect();
    ConConfig_Start_Expect();
    ConMotor_Start_Expect();
    ConPid_Start_Expect();
    ConMotion_Start_Expect();

    Disp_Init();
    Disp_Start();

    memset(&cmd, 0, sizeof cmd);
    p_concmd = 0;
    memset(&concmd, 0, sizeof concmd);
}

void tearDown(void)
{
}

/* Test Valid/Invalid Commands and associated is_valid/not is_valid */

void test_WhenInvalidCommand_ThenActiveCommandIsNotAssignedAndReturnsIsValidFalse(void)
{
    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

/* Test Config commands */

void test_WhenValidConfigCommandButActiveCommandNotAssigned_ThenReturnsIsValidFalse(void)
{
    cmd.args.config = 1;
    cmd.args.debug = 1;
    cmd.args.enable = 1;
    cmd.args.mask = "1";

    ConConfig_InitConfigDebug_ExpectAndReturn(cmd.args.enable, 0x0001, p_concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}


void test_WhenConfigDebugMaskEnable_ThenIsValidTrue(void)
{
    cmd.args.config = 1;
    cmd.args.debug = 1;
    cmd.args.enable = 1;
    cmd.args.mask = "1";

    ConConfig_InitConfigDebug_ExpectAndReturn(cmd.args.enable, 0x0001, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenConfigDebugAllEnable_ThenIsValidTrue(void)
{
    cmd.args.config = 1;
    cmd.args.debug = 1;
    cmd.args.enable = 1;
    cmd.args.all = 1;

    ConConfig_InitConfigDebug_ExpectAndReturn(cmd.args.enable, 0x03FF, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenConfigDebugBitsEnable_ThenIsValidTrue(void)
{
    cmd.args.config = 1;
    cmd.args.debug = 1;
    cmd.args.enable = 1;
    cmd.args.lenc = 1;
    cmd.args.renc = 0;
    cmd.args.lpid = 1;
    cmd.args.rpid = 0;
    cmd.args.lmotor = 1;
    cmd.args.rmotor = 0;
    cmd.args.odom = 1;

    ConConfig_InitConfigDebug_ExpectAndReturn(TRUE, 0x0055, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenConfigShowBits_ThenIsValidTrue(void)
{
    cmd.args.config = 1;
    cmd.args.show = 1;
    cmd.args.motor = 1;
    cmd.args.pid = 0;
    cmd.args.bias = 1;
    cmd.args.debug = 0;
    cmd.args.params = 1;
    cmd.args.plain_text = 1;

    ConConfig_InitConfigShow_ExpectAndReturn(0x0025, cmd.args.plain_text, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenConfigClearAll_ThenIsValidTrue(void)
{
    cmd.args.config = 1;
    cmd.args.clear = 1;
    cmd.args.all = 1;
    cmd.args.plain_text = 0;

    ConConfig_InitConfigClear_ExpectAndReturn(0x001F, cmd.args.plain_text, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenConfigClearBits_ThenIsValidTrue(void)
{
    cmd.args.config = 1;
    cmd.args.clear = 1;
    cmd.args.motor = 1;
    cmd.args.pid = 1;
    cmd.args.bias = 0;
    cmd.args.debug = 1;
    cmd.args.plain_text = 1;

    ConConfig_InitConfigClear_ExpectAndReturn(0x000B, cmd.args.plain_text, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

/* Test Motor commands */

void test_WhenValidMotorCommandButActiveCommandNotAssigned_ThenReturnsIsValidFalse(void)
{
    cmd.args.motor = 1;
    cmd.args.show = 1;
    cmd.args.left = 1;
    cmd.args.plain_text = 0;

    ConMotor_InitMotorShow_ExpectAndReturn(WHEEL_LEFT, cmd.args.plain_text, p_concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

void test_WhenMotorShowLeftNoPlaintext_ThenIsValidTrue(void)
{
    cmd.args.motor = 1;
    cmd.args.show = 1;
    cmd.args.left = 1;
    cmd.args.plain_text = 0;

    ConMotor_InitMotorShow_ExpectAndReturn(WHEEL_LEFT, cmd.args.plain_text, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotorRepRight_ThenIsValidTrue(void)
{
    cmd.args.motor = 1;
    cmd.args.rep = 1;
    cmd.args.right = 1;
    cmd.args.first = "0.2";
    cmd.args.second = "0.6";
    cmd.args.intvl = "5.0";
    cmd.args.iters = "10.0";
    cmd.args.no_pid = 0;
    cmd.args.no_accel = 0;

    ConMotor_InitMotorRepeat_ExpectAndReturn(WHEEL_RIGHT, 0.2, 0.6, 5.0, 10.0, cmd.args.no_pid, cmd.args.no_accel, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotorCalLeftRightIters_ThenIsValidTrue(void)
{
    cmd.args.motor = 1;
    cmd.args.cal = 1;
    cmd.args.left = 1;
    cmd.args.right = 1;
    cmd.args.iters = "5.0";

    ConMotor_InitMotorCal_ExpectAndReturn(WHEEL_BOTH, 5.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotorValLeftBackward_ThenIsValidTrue(void)
{
    cmd.args.motor = 1;
    cmd.args.val = 1;
    cmd.args.left = 1;
    cmd.args.backward = 1;
    cmd.args.min_percent = "0.2";
    cmd.args.max_percent = "0.6";
    cmd.args.num_points = "7";

    ConMotor_InitMotorVal_ExpectAndReturn(WHEEL_LEFT, DIR_BACKWARD, 0.2, 0.6, 7, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotorMoveLeftRight_ThenIsValidTrue(void)
{
    cmd.args.motor = 1;
    cmd.args.left_speed = "0.2";
    cmd.args.right_speed = "-0.2";
    cmd.args.duration = "10.0";
    cmd.args.no_pid = 0;
    cmd.args.no_accel = 0;

    ConMotor_InitMotorMove_ExpectAndReturn(0.2, -0.2, 10.0, 0, 0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

/* Test PID commands */

void test_WhenValidPidCommandButActiveCommandNotAssigned_ThenReturnsIsValidFalse(void)
{
    cmd.args.pid = 1;
    cmd.args.show = 1;
    cmd.args.right = 1;
    cmd.args.plain_text = 1;

    ConPid_InitPidShow_ExpectAndReturn(WHEEL_RIGHT, 1, p_concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

void test_WhenPidShowRightPlaintext_ThenIsValidTrue(void)
{
    cmd.args.pid = 1;
    cmd.args.show = 1;
    cmd.args.right = 1;
    cmd.args.plain_text = 1;

    ConPid_InitPidShow_ExpectAndReturn(WHEEL_RIGHT, 1, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenPidCalLeftImpulse_ThenIsValidTrue(void)
{
    cmd.args.pid = 1;
    cmd.args.cal = 1;
    cmd.args.left = 1;
    cmd.args.impulse = 1;
    cmd.args.with_debug = 0;

    ConPid_InitPidCal_ExpectAndReturn(WHEEL_LEFT, 1, 0, 0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenPidValLeftRightForward_ThenIsValidTrue(void)
{
    cmd.args.pid = 1;
    cmd.args.val = 1;
    cmd.args.left = 1;
    cmd.args.right = 1;
    cmd.args.forward = 1;
    cmd.args.min_percent = "0.2";
    cmd.args.max_percent = "0.8";
    cmd.args.num_points = "11";

    ConPid_InitPidVal_ExpectAndReturn(WHEEL_BOTH, DIR_FORWARD, 0.2, 0.8, 11, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

/* Test Motion commands */


void test_WhenValidMotionCommandButActiveCommandNotAssigned_ThenReturnsIsValidFalse(void)
{
    cmd.args.motion = 1;
    cmd.args.cal = 1;
    cmd.args.linear = 1;
    cmd.args.distance = "1.0";

    ConMotion_InitCalLinear_ExpectAndReturn(1.0, p_concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

void test_WhenMotionCalLinear_ThenIsValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.cal = 1;
    cmd.args.linear = 1;
    cmd.args.distance = "1.0";

    ConMotion_InitCalLinear_ExpectAndReturn(1.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionCalAngular_ThenIsValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.cal = 1;
    cmd.args.angular = 1;
    cmd.args.angle = "360.0";

    ConMotion_InitMotionCalAngular_ExpectAndReturn(360.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionCalUmbmark_ThenIsValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.cal = 1;
    cmd.args.umbmark = 1;

    ConMotion_InitMotionCalUmbmark_ExpectAndReturn(&concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionValLinear_ThenIsValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.val = 1;
    cmd.args.linear = 1;
    cmd.args.distance = "1.0";

    ConMotion_InitMotionValLinear_ExpectAndReturn(1.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionValAngular_ThenIsValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.val = 1;
    cmd.args.angular = 1;
    cmd.args.angle = "360.0";

    ConMotion_InitMotionValAngular_ExpectAndReturn(360.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionValSquare_ThenisValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.val = 1;
    cmd.args.square = 1;
    cmd.args.left = 1;
    cmd.args.side = "1.0";

    ConMotion_InitMotionValSquare_ExpectAndReturn(1, 1.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionValCircle_ThenisValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.val = 1;
    cmd.args.circle = 1;
    cmd.args.cw = 1;
    cmd.args.radius = "0.0";

    ConMotion_InitMotionValCircle_ExpectAndReturn(1, 0.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

void test_WhenMotionValOutAndBack_ThenisValidTrue(void)
{
    cmd.args.motion = 1;
    cmd.args.val = 1;
    cmd.args.out_and_back = 1;
    cmd.args.distance = "1.0";

    ConMotion_InitMotionValOutAndBack_ExpectAndReturn(1.0, &concmd);

    Disp_Dispatch(&cmd);

    TEST_ASSERT_EQUAL_INT(TRUE, cmd.is_valid);
}

