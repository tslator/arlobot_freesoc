#ifndef CONPARSER_H
#define CONPARSER_H

#include "freesoc.h"

typedef struct {
    /* commands */
    int all;
    int angular;
    int backward;
    int bias;
    int cal;
    int ccw;
    int circle;
    int clear;
    int config;
    int cw;
    int debug;
    int disable;
    int enable;
    int forward;
    int help;
    int left;
    int lenc;
    int linear;
    int lmotor;
    int lpid;
    int motion;
    int motor;
    int odom;
    int out_and_back;
    int params;
    int pid;
    int renc;
    int rep;
    int right;
    int rmotor;
    int rpid;
    int set;
    int show;
    int square;
    int status;
    int umbmark;
    int val;
    /* options without arguments */
    int impulse;
    int interactive;
    int load_gains;
    int no_accel;
    int no_control;
    int no_debug;
    int no_pid;
    int plain_text;
    /* options with arguments */
    char *angle;
    char *angular_speed;
    char *distance;
    char *duration;
    char *first;
    char *interval;
    char *iters;
    char *left_speed;
    char *linear_speed;
    char *mask;
    char *max_percent;
    char *min_percent;
    char *num_points;
    char *radius;
    char *right_speed;
    char *second;
    char *side;
    char *step;
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
