
#include <math.h>
#include "pidutil.h"
#include "config.h"
#include "debug.h"
#include "utils.h"

#define IS_NAN_DEFAULT(x,d)   isnan(x) ? d : x

#if defined (LEFT_PID_DUMP_ENABLED) || defined(RIGHT_PID_DUMP_ENABLED) || defined(CTRL_LINEAR_PID_DUMP_ENABLED) || defined(CTRL_ANGULAR_PID_DUMP_ENABLED)
void DumpPid(char *name, PIDControl *pid, uint16 pwm)
{
    char set_point_str[10];
    char input_str[10];
    char error_str[10];
    char last_input_str[10];
    char iterm_str[10];
    char output_str[10];
    
    float set_point = IS_NAN_DEFAULT(pid->setpoint, 0);
    float input = IS_NAN_DEFAULT(pid->input, 0);
    float error = IS_NAN_DEFAULT(pid->setpoint - pid->input, 0);
    float last_input = IS_NAN_DEFAULT(pid->lastInput, 0);
    float iterm = IS_NAN_DEFAULT(pid->iTerm, 0);
    float output = IS_NAN_DEFAULT(pid->output, 0);

    ftoa(set_point, set_point_str, 3);
    ftoa(input, input_str, 3);
    ftoa(error, error_str, 3);
    ftoa(last_input, last_input_str, 3);
    ftoa(iterm, iterm_str, 3);
    ftoa(output, output_str, 3);

    if (PID_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("%s pid: %s %s %s %s %s %s %d\r\n", name, set_point_str, input_str, error_str, last_input_str, iterm_str, output_str, pwm);
    }
}
#endif
