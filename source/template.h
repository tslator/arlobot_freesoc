#ifndef $module
#define $module

#include "freesoc.h"

typedef struct {$commands$arguments$flags$options
    /* special */
    const char *usage_pattern;
    const char *help_message;
    const char *motor_usage_pattern;
    const char *motor_help_message;
    const char *pid_usage_pattern;
    const char *pid_help_message;
    const char *config_usage_pattern;
    const char *config_help_message;
    const char *motion_usage_pattern;
    const char *motion_help_message;
} DocoptArgs;

DocoptArgs docopt(int argc, char *argv[], bool help, const char *version, int* success);


#endif