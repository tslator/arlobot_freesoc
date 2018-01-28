
#include <stdio.h>
#include <stdarg.h>
#include "unity.h"
#include "parser.h"
#include "mock_disp.h"
#include "mock_serial.h"
#include "mock_conparser.h"
#include "mock_assertion.h"

typedef struct _tag_test_data
{
    CHAR *p_text;
    DocoptArgs args;
    BOOL (*eval)(DocoptArgs);
} TEST_DATA_TYPE;


static COMMAND_TYPE cmd;


/* Test Config commands

    console config debug (enable|disable) ([lmotor|rmotor|lenc|renc|lpid|rpid|odom|all] | --mask=<mask>)
    console config show [motor|pid|bias|debug|status|params] [--plain-text]
    console config clear (motor|pid|bias|debug|all)
    console config help

*/
#ifdef XXX    
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

         0                 1                   2                   3                   4                   5                   6                   7 
         1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1

typedef struct {
    /* commands */
    int all;        // 1
    int angular;    // 2
    int backward;   // 3
    int bias;       // 4
    int cal;        // 5
    int ccw;        // 6
    int circle;     // 7
    int clear;      // 8
    int config;     // 9
    int cw;         // 10
    int debug;      // 11
    int disable;    // 12
    int enable;     // 13
    int forward;    // 14
    int help;       // 15
    int left;       // 16
    int lenc;       // 17
    int linear;     // 18
    int lmotor;     // 19
    int lpid;       // 20
    int motion;     // 21
    int motor;      // 22
    int odom;       // 23
    int out_and_back;//24
    int params;     // 25
    int pid;        // 26
    int renc;       // 27
    int rep;        // 28
    int right;      // 29
    int rmotor;     // 30
    int rpid;       // 31
    int show;       // 32
    int square;     // 33
    int status;     // 34
    int umbmark;    // 35    
    int val;        // 36
    /* options without arguments */
    int impulse;    // 37
    int no_accel;   // 38
    int no_control; // 39
    int no_pid;     // 40
    int plain_text; // 41
    int speed;      // 42
    int with_debug; // 43
    /* options with arguments */
    char *angle;    // 44
    char *angular_speed;// 45
    char *distance; // 46
    char *duration; // 47
    char *first;    // 48
    char *intvl;    // 49
    char *iters;    // 50
    char *left_speed; // 51
    char *linear_speed; // 52
    char *mask;         // 53
    char *max_percent;  // 54
    char *min_percent;  // 55
    char *num_points;   // 56
    char *radius;       // 57
    char *right_speed;  // 58
    char *second;       // 59
    char *side;         // 60
    char *step;         // 61
    /* special */
    const char *usage_pattern; // 62
    const char *help_message;   // 63
    const char *motor_usage_pattern;    // 64
    const char *motor_help_message; // 65
    const char *pid_usage_pattern;  // 66
    const char *pid_help_message;   // 67
    const char *config_usage_pattern;   // 68
    const char *config_help_message;    // 69
    const char *motion_usage_pattern;   // 70
    const char *motion_help_message;    // 71
} DocoptArgs;
#endif

static BOOL assert_config_debug_enable_mask(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.mask == "1";
}

static BOOL assert_config_debug_disable_mask(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.mask == "1";
}

static BOOL assert_config_debug_enable_lmotor(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.lmotor == 1;
}

static BOOL assert_config_debug_disable_lmotor(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.lmotor == 1;
}

static BOOL assert_config_debug_enable_rmotor(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.rmotor == 1;
}

static BOOL assert_config_debug_disable_rmotor(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.rmotor == 1;
}

static BOOL assert_config_debug_enable_lenc(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.lenc == 1;
}

static BOOL assert_config_debug_disable_lenc(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.lenc == 1;
}

static BOOL assert_config_debug_enable_renc(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.renc == 1;
}

static BOOL assert_config_debug_disable_renc(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.renc == 1;
}

static BOOL assert_config_debug_enable_lpid(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.lpid == 1;
}

static BOOL assert_config_debug_disable_lpid(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.lpid == 1;
}

static BOOL assert_config_debug_enable_rpid(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.rpid == 1;
}

static BOOL assert_config_debug_disable_rpid(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.rpid == 1;
}

static BOOL assert_config_debug_enable_odom(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.odom == 1;
}

static BOOL assert_config_debug_disable_odom(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.odom == 1;
}

static BOOL assert_config_debug_enable_all(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.enable == 1 && args.all == 1;
}

static BOOL assert_config_debug_disable_all(DocoptArgs args)
{
    return args.config == 1 && args.debug == 1 && args.disable == 1 && args.all == 1;
}

static BOOL assert_config_show_motor(DocoptArgs args)
{
    return args.config == 1 && args.show == 1 && args.motor == 1;
}

static BOOL assert_config_show_pid(DocoptArgs args)
{
    return args.config == 1 && args.show == 1 && args.pid == 1;
}

static BOOL assert_config_show_debug(DocoptArgs args)
{
    return args.config == 1 && args.show == 1 && args.debug == 1;
}

static BOOL assert_config_show_status(DocoptArgs args)
{
    return args.config == 1 && args.show == 1 && args.status == 1;
}

static BOOL assert_config_show_params(DocoptArgs args)
{
    return args.config == 1 && args.show == 1 && args.params == 1;
}

static BOOL assert_config_clear_motor(DocoptArgs args)
{
    return args.config == 1 && args.clear == 1 && args.motor == 1;
}

static BOOL assert_config_clear_pid(DocoptArgs args)
{
    return args.config == 1 && args.clear == 1 && args.pid == 1;
}
                
static BOOL assert_config_clear_bias(DocoptArgs args)
{
    return args.config == 1 && args.clear == 1 && args.bias == 1;
}
        
static BOOL assert_config_clear_debug(DocoptArgs args)
{
    return args.config == 1 && args.clear == 1 && args.debug == 1;
}
        
static BOOL assert_config_clear_all(DocoptArgs args)
{
    return args.config == 1 && args.clear == 1 && args.all == 1;
}
                
static BOOL assert_config_help(DocoptArgs args)
{
    return args.config == 1 && args.help == 1;
}
        
static TEST_DATA_TYPE config_test_data[] = {
    {
        "config debug enable --mask=0x0001", 
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"1",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_mask
    },
    {
        "config debug disable --mask=0x0001", 
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"1",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_mask
    },
    {
        "config debug enable lmotor",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_lmotor
    },
    {
        "config debug disable lmotor",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_lmotor
    },
    {
        "config debug enable rmotor",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_rmotor
    },
    {
        "config debug disable rmotor",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_rmotor
    },
    {
        "config debug enable lenc",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_lenc
    },
    {
        "config debug disable lenc",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_lenc
    },
    {
        "config debug enable renc",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_renc
    },
    {
        "config debug disable renc",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_renc
    },
    {
        "config debug enable lpid",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_lpid
    },
    {
        "config debug disable lpid",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_lpid
    },
    {
        "config debug enable rpid",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_rpid
    },
    {
        "config debug disable rpid",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_rpid
    },
    {
        "config debug enable odom",
        {0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_odom
    },
    {
        "config debug disable odom",
        {0,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_odom
    },
    {
        "config debug enable all",
        {1,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_enable_all
    },
    {
        "config debug disable all",
        {1,0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_debug_disable_all
    },
    {
        "config show motor",
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_show_motor
    },
    {
        "config show pid",
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_show_pid
    },
    {
        "config show debug",
        {0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_show_debug
    },
    {
        "config show status",
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_show_status
    },
    {
        "config show params",
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_show_params
    },
    {
        "config clear motor",
        {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_clear_motor        
    },
    {
        "config clear pid",
        {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_clear_pid
    },
    {
        "config clear bias",
        {0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_clear_bias
    },
    {
        "config clear debug",
        {0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_clear_debug
    },
    {
        "config clear all",
        {1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_clear_all
    },
    {
        "config help",
        {0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        assert_config_help
    }
};

void setUp(void)
{
    Parser_Init();
    Parser_Start();
    memset(&cmd, 0, sizeof cmd);
}

void tearDown(void)
{
}

void test_WhenValidConfigCommand_ThenCommandArgsAreValid(void)
{
    DocoptArgs docopt_test(int argc, char *argv[], bool help, const char *version, int* success, int cmock_calls)
    {
        *success = TRUE;
        return config_test_data[cmock_calls].args;
    }

    int ii;
    int num_cases = sizeof config_test_data / sizeof config_test_data[0];

    //TEST_IGNORE();

    docopt_StubWithCallback(docopt_test);
    /* This call only occurs for "config help" and we don't care about the output string */
    Ser_PutString_Ignore();

    for (ii = 0; ii < num_cases; ++ii)
    {
        Parser_Parse(config_test_data[ii].p_text, &cmd);
        TEST_ASSERT_EQUAL_UINT(TRUE, config_test_data[ii].eval(config_test_data[ii].args));
        printf("Comand: %s passed\n", config_test_data[ii].p_text);
    }
}


/* Test Motor commands */

void test_WhenValidMotorCommandButActiveCommandNotAssigned_ThenReturnsIsValidFalse(void)
{
    TEST_IGNORE();
    cmd.args.motor = 1;
    cmd.args.show = 1;
    cmd.args.left = 1;
    cmd.args.plain_text = 0;

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

/* Test Motion commands */


void test_WhenValidMotionCommandButActiveCommandNotAssigned_ThenReturnsIsValidFalse(void)
{
    TEST_IGNORE();
    cmd.args.motion = 1;
    cmd.args.cal = 1;
    cmd.args.linear = 1;
    cmd.args.distance = "1.0";

    TEST_ASSERT_EQUAL_INT(FALSE, cmd.is_valid);
}

