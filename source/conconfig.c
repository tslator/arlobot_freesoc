#include <stdarg.h>
#include "conconfig.h"
#include "consts.h"
#include "debug.h"
#include "cal.h"
#include "utils.h"

typedef enum {CONFIG_FIRST = 0, CONFIG_DEBUG=CONFIG_FIRST, CONFIG_CLEAR, CONFIG_SHOW, CONFIG_LAST} CONFIG_CMD_TYPE;

typedef struct _tag_config_show
{
    UINT16 mask;
    BOOL plain_text;
} CONFIG_SHOW_TYPE;

typedef struct _tag_config_debug
{
    BOOL enable;
    UINT16 mask;
} CONFIG_DEBUG_TYPE;

typedef struct _tag_config_clear
{
    UINT16 mask;
    BOOL plain_text;
} CONFIG_CLEAR_TYPE;


static BOOL is_running;

static CONFIG_DEBUG_TYPE config_debug;
static CONFIG_SHOW_TYPE config_show;
static CONFIG_CLEAR_TYPE config_clear;


static CONCMD_IF_TYPE cmd_if_array[CONFIG_LAST];


/*-------------------------------------------------------------------
    Config Debug
*/
static CONCMD_IF_TYPE * const config_debug_init(BOOL enable, UINT16 mask)
{    
    config_debug.enable = enable;
    config_debug.mask = mask;

    is_running = TRUE;

    return &cmd_if_array[CONFIG_DEBUG];
}

static BOOL config_debug_update(void)
{
    if (config_debug.enable)
    {
        Debug_Enable(config_debug.mask);
    }
    else
    {
        Debug_Disable(config_debug.mask);
    }
    
    is_running = FALSE;
    return is_running;
}

static BOOL config_debug_status(void)
{
    return is_running;
}

static void config_debug_results(void)
{
}

/*-------------------------------------------------------------------
    Config Show
*/

static CONCMD_IF_TYPE * const config_show_init(UINT16 mask, BOOL plain_text)
{
    config_show.mask = mask;
    config_show.plain_text = plain_text;
    is_running = TRUE;

    return &cmd_if_array[CONFIG_SHOW];
}

static BOOL config_show_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL config_show_status(void)
{
    return is_running;
}

static void config_show_results(void)
{
    switch (config_show.mask)
    {
        case 0x0001:
            Cal_PrintAllMotorParams(!config_show.plain_text);
            break;

        case 0x0002:
            Cal_PrintAllPidGains(!config_show.plain_text);
            break;

        case 0x0004:
            Cal_PrintBias(!config_show.plain_text);
            break;

        case 0x0008:
            Cal_PrintStatus(!config_show.plain_text);
            break;
            
        case 0x0010:
        {
            Ser_PutStringFormat("Track Width (meter)              : %.4f\r\n", TRACK_WIDTH);
            Ser_PutStringFormat("Wheel Radius (meter)             : %.4f\r\n", WHEEL_RADIUS);
            Ser_PutStringFormat("Wheel Diameter (meter)           : %.4f\r\n", WHEEL_DIAMETER);
            Ser_PutStringFormat("Wheel Circumference (meter/rev)  : %.4f\r\n", WHEEL_CIRCUMFERENCE);
            
            Ser_PutStringFormat("Wheel Max RPM                    : %2.0f\r\n", MAX_WHEEL_RPM);
            Ser_PutStringFormat("Wheel Encoder (tick/rev)         : %d\r\n", WHEEL_ENCODER_TICK_PER_REV);
            Ser_PutStringFormat("Wheel Encoder (count/Rev)        : %d\r\n", WHEEL_COUNT_PER_REV);
            Ser_PutStringFormat("Wheel (meter/count)              : %.4f\r\n", WHEEL_METER_PER_COUNT);
            Ser_PutStringFormat("Wheel (count/meter)              : %.4f\r\n", WHEEL_COUNT_PER_METER);
            Ser_PutStringFormat("Wheel (count/radian)             : %.4f\r\n", WHEEL_COUNT_PER_RADIAN);
            Ser_PutStringFormat("Wheel (radian/count)             : %.4f\r\n", WHEEL_RADIAN_PER_COUNT);            
            Ser_PutStringFormat("Wheel (radian/second)            : %.4f\r\n", MAX_WHEEL_RADIAN_PER_SECOND);
            Ser_PutStringFormat("Wheel (count/second)             : %.4f\r\n", MAX_WHEEL_COUNT_PER_SECOND);
            Ser_PutStringFormat("Wheel (meter/second)             : %.4f\r\n", MAX_WHEEL_METER_PER_SECOND);
            

            Ser_PutStringFormat("Wheel Forward Max (meter/second) : %.4f\r\n", MAX_WHEEL_FORWARD_LINEAR_VELOCITY);
            Ser_PutStringFormat("Wheel Backward Max (meter/second): %.4f\r\n", MAX_WHEEL_BACKWARD_LINEAR_VELOCITY);
            Ser_PutStringFormat("Wheel Forward Max (count/second) : %.4f\r\n", MAX_WHEEL_FORWARD_COUNT_PER_SEC);
            Ser_PutStringFormat("Wheel Backward Max (count/second): %.4f\r\n", MAX_WHEEL_BACKWARD_COUNT_PER_SEC);

            Ser_PutStringFormat("Wheel CW Max (radian/second)     : %.4f\r\n", MAX_WHEEL_CW_ANGULAR_VELOCITY);
            Ser_PutStringFormat("Wheel CCW Max (radian/second)    : %.4f\r\n", MIN_WHEEL_CCW_ANGULAR_VELOCITY);
            Ser_PutStringFormat("Wheel CW Max (count/second)      : %.4f\r\n", MAX_WHEEL_CW_COUNT_PER_SEC);
            Ser_PutStringFormat("Wheel CCW Max (count/second)     : %.4f\r\n", MAX_WHEEL_CCW_COUNT_PER_SEC);
            
            Ser_PutStringFormat("Robot Max RPM                    : %.4f\r\n", MAX_ROBOT_RPM);
            Ser_PutStringFormat("Robot (meter/rev)                : %.4f\r\n", ROBOT_METER_PER_REV);
            Ser_PutStringFormat("Robot (count/rev)                : %.4f\r\n", ROBOT_COUNT_PER_REV);

            Ser_PutStringFormat("Robot Max Linear (meter/second)  : %.4f\r\n", MAX_WHEEL_METER_PER_SECOND);
            Ser_PutStringFormat("Robot Max Angular (radian/second): %.4f\r\n", CalcMaxAngularVelocity());
            Ser_PutStringFormat("Robot Max Diff (radian/second)   : %.4f\r\n", CalcMaxDiffVelocity());
            Ser_PutStringFormat("Robot CW Max (radian/second)     : %.4f\r\n", MAX_ROBOT_CW_RADIAN_PER_SECOND);
            Ser_PutStringFormat("Robot CCW Max (radian/second)    : %.4f\r\n", MAX_ROBOT_CCW_RADIAN_PER_SECOND);

            break; 
        }
    }
}

/*-------------------------------------------------------------------
    Config Clear

    I'm not sure exactly what config clear should do.  Are we talking about
    clearing out calibration values, e.g. motor, pid, bias, etc.  Leaving
    it unimplmeneted for now
*/

static CONCMD_IF_TYPE * const config_clear_init(UINT16 mask, BOOL plain_text)
{
    config_clear.mask = mask;
    config_clear.plain_text = plain_text;

    is_running = TRUE;
    return &cmd_if_array[CONFIG_SHOW];
}

static BOOL config_clear_update(void)
{
    is_running = FALSE;
    return is_running;
}

static BOOL config_clear_status(void)
{
    return is_running;
}

static void config_clear_results(void)
{    
}

void ConConfig_Init(void)
{    
    cmd_if_array[CONFIG_DEBUG].update = config_debug_update;
    cmd_if_array[CONFIG_DEBUG].status = config_debug_status;
    cmd_if_array[CONFIG_DEBUG].results = config_debug_results;

    cmd_if_array[CONFIG_CLEAR].update = config_clear_update;
    cmd_if_array[CONFIG_CLEAR].status = config_clear_status;
    cmd_if_array[CONFIG_CLEAR].results = config_clear_results;

    cmd_if_array[CONFIG_SHOW].update = config_show_update;
    cmd_if_array[CONFIG_SHOW].status = config_show_status;
    cmd_if_array[CONFIG_SHOW].results = config_show_results;

    memset(&config_debug, 0, sizeof config_debug);
    memset(&config_show, 0, sizeof config_show);
    memset(&config_clear, 0, sizeof config_clear);

    is_running = FALSE;
}

void ConConfig_Start(void)
{
}

CONCMD_IF_TYPE * const ConConfig_InitConfigDebug(BOOL enable, UINT16 mask)
{
    return config_debug_init(enable, mask);
}

CONCMD_IF_TYPE * const ConConfig_InitConfigShow(UINT16 mask, BOOL plain_text)
{
    return config_show_init(mask, plain_text);
}

CONCMD_IF_TYPE * const ConConfig_InitConfigClear(UINT16 mask, BOOL plain_text)
{
    return config_clear_init(mask, plain_text);
}

/* [] END OF FILE */