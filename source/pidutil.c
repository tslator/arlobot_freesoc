
#include "pidutil.h"
#include "config.h"
#include "debug.h"
#include "utils.h"

#if defined (LEFT_PID_DUMP_ENABLED) || defined(RIGHT_PID_DUMP_ENABLED) || defined(CTRL_LINEAR_PID_DUMP_ENABLED) || defined(CTRL_ANGULAR_PID_DUMP_ENABLED)
void DumpPid(char *name, PIDControl *pid)
{
    char set_point_str[10];
    char input_str[10];
    char error_str[10];
    char last_input_str[10];
    char iterm_str[10];
    char output_str[10];

    ftoa(pid->setpoint, set_point_str, 3);
    ftoa(pid->input, input_str, 3);
    ftoa(pid->setpoint - pid->input, error_str, 6);
    ftoa(pid->lastInput, last_input_str, 3);
    ftoa(pid->iTerm, iterm_str, 6);
    ftoa(pid->output, output_str, 6);

    if (PID_DEBUG_CONTROL_ENABLED)
    {
        DEBUG_PRINT_ARG("%s pid: %s %s %s %s %s %s\r\n", name, set_point_str, input_str, error_str, last_input_str, iterm_str, output_str);
    }
}
#endif
