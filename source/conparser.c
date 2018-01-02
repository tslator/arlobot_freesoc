#ifdef __cplusplus
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "conparser.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "conparser.h"
#endif


const char help_message[] =
"Arlobot Console\r\n"
"\r\n"
"Usage:\r\n"
"    motor --left-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor --right-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor (--left-speed=<speed> --right-speed=<speed>) [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep left --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep right --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters> [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor show [--plain-text]\r\n"
"    motor cal [left|right] [--iters=<iters>] [--with-debug]\r\n"
"    motor val [left|right] (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    motor help\r\n"
"    pid cal left ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid cal right ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid val (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val left (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val right (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid show [left|right] [--plain-text]\r\n"
"    pid help\r\n"
"    config debug (enable|disable) ([lmotor|rmotor|lenc|renc|lpid|rpid|odom|all] | --mask=<mask>)\r\n"
"    config show [motor|pid|bias|debug|status|params] [--plain-text]\r\n"
"    config clear (motor|pid|bias|debug|all)\r\n"
"    config help\r\n"
"    motion cal linear [--speed] [--distance=<distance>]\r\n"
"    motion cal angular [--speed] [--angle=<angle>]\r\n"
"    motion cal umbmark\r\n"
"    motion val linear [--linear-speed=<speed>] [--distance=<distance>]\r\n"
"    motion val angular [--angular-speed=<speed>] [--angle=<angle>]\r\n"
"    motion val square (left|right) [--side=<side>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion val circle (cw|ccw) [--radius=<radius>] [--angular-speed=<speed>]\r\n"
"    motion val out-and-back [--distance=<distance>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion help\r\n"
"    help\r\n"
"    \r\n"
"\r\n"
"Options:\r\n"
"    -l --left-speed=<speed>     Speed of the left motor (meter/second)\r\n"
"    -r --right-speed=<speed>    Speed of the right motor (meter/second)\r\n"
"    -d --duration=<duration>    Duration in seconds [default: 5]\r\n"
"    -m --mask=<mask>            Bitmap of debug flags\r\n"
"    -p --plain-text             Display output as plain text (default is JSON)\r\n"
"    -w --with-debug             Enable PID debug output\r\n"
"    -i --impulse                Enable impulse response\r\n"
"    -s --distance=<distance>    Amount of travel (meter) [default: 1.0]\r\n"
"    -g --angle=<angle>          Amount of travel (degree)   [default: 360] \r\n"
"    -t --iters=<iters>          Number of iterations per wheel [default: 3]\r\n"
"    -e --step=<step>            Percentage of maximum speed to use for step response [default: 0.8]\r\n"
"    -a --radius=<radius>        Radius of the circle [default: 0.0]\r\n"
"    -h --side=<side>            Side of the square [default: 1.0]\r\n"
"    -n --min-percent=<percent>  Minimum value for profile range specified in percent of maximum speed [default: 0.2]\r\n"
"    -x --max-percent=<percent>  Maximum value for profile range specified in percent of maximum speed [default: 0.8]\r\n"
"    -f --first=<speed>          First speed of the cycle\r\n"
"    -o --second=<speed>         Second speed of the cycle\r\n"
"    -v --intvl=<interval>       Time between speed change [default: 10]\r\n"
"    -u --num-points=<points>    Number of velocity values [default: 7]\r\n"
"    -q --no-pid                 Disables PID control\r\n"
"    -j --no-accel               Disables acceleration profile\r\n"
"    -z --no-control             Bypasses the Control/Safety module";

const char usage_pattern[] =
"Usage:\r\n"
"    motor --left-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor --right-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor (--left-speed=<speed> --right-speed=<speed>) [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep left --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep right --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters> [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor show [--plain-text]\r\n"
"    motor cal [left|right] [--iters=<iters>] [--with-debug]\r\n"
"    motor val [left|right] (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    motor help\r\n"
"    pid cal left ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid cal right ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid val (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val left (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val right (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid show [left|right] [--plain-text]\r\n"
"    pid help\r\n"
"    config debug (enable|disable) ([lmotor|rmotor|lenc|renc|lpid|rpid|odom|all] | --mask=<mask>)\r\n"
"    config show [motor|pid|bias|debug|status|params] [--plain-text]\r\n"
"    config clear (motor|pid|bias|debug|all)\r\n"
"    config help\r\n"
"    motion cal linear [--speed] [--distance=<distance>]\r\n"
"    motion cal angular [--speed] [--angle=<angle>]\r\n"
"    motion cal umbmark\r\n"
"    motion val linear [--linear-speed=<speed>] [--distance=<distance>]\r\n"
"    motion val angular [--angular-speed=<speed>] [--angle=<angle>]\r\n"
"    motion val square (left|right) [--side=<side>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion val circle (cw|ccw) [--radius=<radius>] [--angular-speed=<speed>]\r\n"
"    motion val out-and-back [--distance=<distance>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion help\r\n"
"    help";

const char motor_help_message[] = 
"Motor Help\r\n"
"Usage:\r\n"
"    motor --left-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor --right-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor (--left-speed=<speed> --right-speed=<speed>) [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep left --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep right --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters> [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor show [--plain-text]\r\n"
"    motor cal [left|right] [--iters=<iters>] [--with-debug]\r\n"
"    motor val [left|right] (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    motor help\r\n"
"\r\n"
"Options:\r\n"
"    -z --no-control             Bypasses the Control/Safety module\r\n"
"    -r --right-speed=<speed>    Speed of the right motor (meter/second)\r\n"
"    -u --num-points=<points>    Number of velocity values [default: 7]\r\n"
"    -t --iters=<iters>          Number of iterations per wheel [default: 3]\r\n"
"    -d --duration=<duration>    Duration in seconds [default: 5]\r\n"
"    -f --first=<speed>          First speed of the cycle\r\n"
"    -n --min-percent=<percent>  Minimum value for profile range specified in percent of maximum speed [default: 0.2]\r\n"
"    -x --max-percent=<percent>  Maximum value for profile range specified in percent of maximum speed [default: 0.8]\r\n"
"    -p --plain-text             Display output as plain text (default is JSON)\r\n"
"    -o --second=<speed>         Second speed of the cycle\r\n"
"    -q --no-pid                 Disables PID control\r\n"
"    -w --with-debug             Enable PID debug output\r\n"
"    -v --intvl=<interval>       Time between speed change [default: 10]\r\n"
"    -j --no-accel               Disables acceleration profile";

const char motor_usage_pattern[] =
"Motor Usage\r\n"
"Usage:\r\n"
"    motor --left-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor --right-speed=<speed> [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor (--left-speed=<speed> --right-speed=<speed>) [--duration=<duration>] [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep left --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters>  [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor rep right --first=<speed> --second=<speed> --intvl=<interval> --iters=<iters> [--no-pid] [--no-accel] [--no-control]\r\n"
"    motor show [--plain-text]\r\n"
"    motor cal [left|right] [--iters=<iters>] [--with-debug]\r\n"
"    motor val [left|right] (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    motor help";

const char pid_help_message[] =
"Pid Help\r\n"
"Usage:\r\n"
"    pid cal left ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid cal right ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid val (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val left (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val right (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid show [left|right] [--plain-text]\r\n"
"    pid help\r\n"
"\r\n"
"Options:\r\n"
"    -p --plain-text             Display output as plain text (default is JSON)\r\n"
"    -u --num-points=<points>    Number of velocity values [default: 7]\r\n"
"    -n --min-percent=<percent>  Minimum value for profile range specified in percent of maximum speed [default: 0.2]\r\n"
"    -x --max-percent=<percent>  Maximum value for profile range specified in percent of maximum speed [default: 0.8]\r\n"
"    -w --with-debug             Enable PID debug output";

const char pid_usage_pattern[] =
"Pid Usage\r\n"
"Usage:\r\n"
"    pid cal left ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid cal right ([--impulse] | [--step=<step>]) [--with-debug]\r\n"
"    pid val (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val left (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid val right (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]\r\n"
"    pid show [left|right] [--plain-text]\r\n"
"    pid help";

const char config_help_message[] =
"Config Help\r\n"
"Usage:\r\n"
"    config debug (enable|disable) ([lmotor|rmotor|lenc|renc|lpid|rpid|odom|all] | --mask=<mask>)\r\n"
"    config show [motor|pid|bias|debug|status|params] [--plain-text]\r\n"
"    config clear (motor|pid|bias|debug|all)\r\n"
"    config help\r\n"
"\r\n"
"Options:\r\n"
"    -p --plain-text             Display output as plain text (default is JSON)\r\n"
"    -m --mask=<mask>            Bitmap of debug flags";

const char config_usage_pattern[] =
"Config Usage\r\n"
"Usage:\r\n"
"    config debug (enable|disable) ([lmotor|rmotor|lenc|renc|lpid|rpid|odom|all] | --mask=<mask>)\r\n"
"    config show [motor|pid|bias|debug|status|params] [--plain-text]\r\n"
"    config clear (motor|pid|bias|debug|all)\r\n"
"    config help";

const char motion_help_message[] =
"Motion Help\r\n"
"Usage:\r\n"
"    motion cal linear [--speed] [--distance=<distance>]\r\n"
"    motion cal angular [--speed] [--angle=<angle>]\r\n"
"    motion cal umbmark\r\n"
"    motion val linear [--linear-speed=<speed>] [--distance=<distance>]\r\n"
"    motion val angular [--angular-speed=<speed>] [--angle=<angle>]\r\n"
"    motion val square (left|right) [--side=<side>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion val circle (cw|ccw) [--radius=<radius>] [--angular-speed=<speed>]\r\n"
"    motion val out-and-back [--distance=<distance>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion help\r\n"
"\r\n"
"Options:\r\n"
"    -g --angle=<angle>          Amount of travel (degree)   [default: 360] \r\n"
"    -a --radius=<radius>        Radius of the circle [default: 0.0]\r\n"
"    -h --side=<side>            Side of the square [default: 1.0]\r\n"
"    -s --distance=<distance>    Amount of travel (meter) [default: 1.0]";

const char motion_usage_pattern[] =
"Motion Usage\r\n"
"Usage:\r\n"
"    motion cal linear [--speed] [--distance=<distance>]\r\n"
"    motion cal angular [--speed] [--angle=<angle>]\r\n"
"    motion cal umbmark\r\n"
"    motion val linear [--linear-speed=<speed>] [--distance=<distance>]\r\n"
"    motion val angular [--angular-speed=<speed>] [--angle=<angle>]\r\n"
"    motion val square (left|right) [--side=<side>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion val circle (cw|ccw) [--radius=<radius>] [--angular-speed=<speed>]\r\n"
"    motion val out-and-back [--distance=<distance>] [--linear-speed=<speed>] [--angular-speed=<speed>]\r\n"
"    motion help";



typedef struct {
    const char *name;
    bool value;
} Command;

typedef struct {
    const char *name;
    char *value;
    char **array;
} Argument;

typedef struct {
    const char *oshort;
    const char *olong;
    bool argcount;
    bool value;
    char *argument;
} Option;

typedef struct {
    int n_commands;
    int n_arguments;
    int n_options;
    Command *commands;
    Argument *arguments;
    Option *options;
} Elements;


/*
 * Tokens object
 */

typedef struct Tokens {
    int argc;
    char **argv;
    int i;
    char *current;
} Tokens;

Tokens tokens_new(int argc, char **argv) {
    Tokens ts = {argc, argv, 0, argv[0]};
    return ts;
}

Tokens* tokens_move(Tokens *ts) {
    if (ts->i < ts->argc) {
        ts->current = ts->argv[++ts->i];
    }
    if (ts->i == ts->argc) {
        ts->current = NULL;
    }
    return ts;
}


/*
 * ARGV parsing functions
 */

int parse_doubledash(Tokens *ts, Elements *elements) {
    //int n_commands = elements->n_commands;
    //int n_arguments = elements->n_arguments;
    //Command *commands = elements->commands;
    //Argument *arguments = elements->arguments;

    // not implemented yet
    // return parsed + [Argument(None, v) for v in tokens]
    return 0;
}

int parse_long(Tokens *ts, Elements *elements) {
    int i;
    int len_prefix;
    int n_options = elements->n_options;
    char *eq = strchr(ts->current, '=');
    Option *option;
    Option *options = elements->options;

    len_prefix = (eq-(ts->current))/sizeof(char);
    for (i=0; i < n_options; i++) {
        option = &options[i];
        if (!strncmp(ts->current, option->olong, len_prefix))
            break;
    }
    if (i == n_options) {
        // TODO '%s is not a unique prefix
        fprintf(stderr, "%s is not recognized\n", ts->current);
        return 1;
    }
    tokens_move(ts);
    if (option->argcount) {
        if (eq == NULL) {
            if (ts->current == NULL) {
                fprintf(stderr, "%s requires argument\n", option->olong);
                return 1;
            }
            option->argument = ts->current;
            tokens_move(ts);
        } else {
            option->argument = eq + 1;
        }
    } else {
        if (eq != NULL) {
            fprintf(stderr, "%s must not have an argument\n", option->olong);
            return 1;
        }
        option->value = true;
    }
    return 0;
}

int parse_shorts(Tokens *ts, Elements *elements) {
    char *raw;
    int i;
    int n_options = elements->n_options;
    Option *option;
    Option *options = elements->options;

    raw = &ts->current[1];
    tokens_move(ts);
    while (raw[0] != '\0') {
        for (i=0; i < n_options; i++) {
            option = &options[i];
            if (option->oshort != NULL && option->oshort[1] == raw[0])
                break;
        }
        if (i == n_options) {
            // TODO -%s is specified ambiguously %d times
            fprintf(stderr, "-%c is not recognized\n", raw[0]);
            return 1;
        }
        raw++;
        if (!option->argcount) {
            option->value = true;
        } else {
            if (raw[0] == '\0') {
                if (ts->current == NULL) {
                    fprintf(stderr, "%s requires argument\n", option->oshort);
                    return 1;
                }
                raw = ts->current;
                tokens_move(ts);
            }
            option->argument = raw;
            break;
        }
    }
    return 0;
}

int parse_argcmd(Tokens *ts, Elements *elements) {
    int i;
    int n_commands = elements->n_commands;
    //int n_arguments = elements->n_arguments;
    Command *command;
    Command *commands = elements->commands;
    //Argument *arguments = elements->arguments;

    for (i=0; i < n_commands; i++) {
        command = &commands[i];
        if (!strcmp(command->name, ts->current)){
            command->value = true;
            tokens_move(ts);
            return 0;
        }
    }
    // not implemented yet, just skip for now
    // parsed.append(Argument(None, tokens.move()))
    /*fprintf(stderr, "! argument '%s' has been ignored\n", ts->current);
    fprintf(stderr, "  '");
    for (i=0; i<ts->argc ; i++)
        fprintf(stderr, "%s ", ts->argv[i]);
    fprintf(stderr, "'\n");*/
    tokens_move(ts);
    // Return 1 if we didn't find a command
    return 1;
}

int parse_args(Tokens *ts, Elements *elements) {
    int ret;

    while (ts->current != NULL) {
        if (strcmp(ts->current, "--") == 0) {
            ret = parse_doubledash(ts, elements);
            if (!ret) break;
        } else if (ts->current[0] == '-' && ts->current[1] == '-') {
            ret = parse_long(ts, elements);
        } else if (ts->current[0] == '-' && ts->current[1] != '\0') {
            ret = parse_shorts(ts, elements);
        } else
            ret = parse_argcmd(ts, elements);
        if (ret) return ret;
    }
    return 0;
}

int elems_to_args(Elements *elements, DocoptArgs *args, bool help,
                  const char *version){
    Command *command;
    Argument *argument;
    Option *option;
    int i;

    // fix gcc-related compiler warnings (unused)
    (void)command;
    (void)argument;

    /* options */
    for (i=0; i < elements->n_options; i++) {
        option = &elements->options[i];
        if (help && option->value && !strcmp(option->olong, "--help")) {
            printf("%s", args->help_message);
            return 1;
        } else if (version && option->value &&
                   !strcmp(option->olong, "--version")) {
            printf("%s\n", version);
            return 1;
        } else if (!strcmp(option->olong, "--impulse")) {
            args->impulse = option->value;
        } else if (!strcmp(option->olong, "--no-accel")) {
            args->no_accel = option->value;
        } else if (!strcmp(option->olong, "--no-control")) {
            args->no_control = option->value;
        } else if (!strcmp(option->olong, "--no-pid")) {
            args->no_pid = option->value;
        } else if (!strcmp(option->olong, "--plain-text")) {
            args->plain_text = option->value;
        } else if (!strcmp(option->olong, "--speed")) {
            args->speed = option->value;
        } else if (!strcmp(option->olong, "--with-debug")) {
            args->with_debug = option->value;
        } else if (!strcmp(option->olong, "--angle")) {
            if (option->argument)
                args->angle = option->argument;
        } else if (!strcmp(option->olong, "--angular-speed")) {
            if (option->argument)
                args->angular_speed = option->argument;
        } else if (!strcmp(option->olong, "--distance")) {
            if (option->argument)
                args->distance = option->argument;
        } else if (!strcmp(option->olong, "--duration")) {
            if (option->argument)
                args->duration = option->argument;
        } else if (!strcmp(option->olong, "--first")) {
            if (option->argument)
                args->first = option->argument;
        } else if (!strcmp(option->olong, "--intvl")) {
            if (option->argument)
                args->intvl = option->argument;
        } else if (!strcmp(option->olong, "--iters")) {
            if (option->argument)
                args->iters = option->argument;
        } else if (!strcmp(option->olong, "--left-speed")) {
            if (option->argument)
                args->left_speed = option->argument;
        } else if (!strcmp(option->olong, "--linear-speed")) {
            if (option->argument)
                args->linear_speed = option->argument;
        } else if (!strcmp(option->olong, "--mask")) {
            if (option->argument)
                args->mask = option->argument;
        } else if (!strcmp(option->olong, "--max-percent")) {
            if (option->argument)
                args->max_percent = option->argument;
        } else if (!strcmp(option->olong, "--min-percent")) {
            if (option->argument)
                args->min_percent = option->argument;
        } else if (!strcmp(option->olong, "--num-points")) {
            if (option->argument)
                args->num_points = option->argument;
        } else if (!strcmp(option->olong, "--radius")) {
            if (option->argument)
                args->radius = option->argument;
        } else if (!strcmp(option->olong, "--right-speed")) {
            if (option->argument)
                args->right_speed = option->argument;
        } else if (!strcmp(option->olong, "--second")) {
            if (option->argument)
                args->second = option->argument;
        } else if (!strcmp(option->olong, "--side")) {
            if (option->argument)
                args->side = option->argument;
        } else if (!strcmp(option->olong, "--step")) {
            if (option->argument)
                args->step = option->argument;
        }
    }
    /* commands */
    for (i=0; i < elements->n_commands; i++) {
        command = &elements->commands[i];
        if (!strcmp(command->name, "all")) {
            args->all = command->value;
        } else if (!strcmp(command->name, "angular")) {
            args->angular = command->value;
        } else if (!strcmp(command->name, "backward")) {
            args->backward = command->value;
        } else if (!strcmp(command->name, "bias")) {
            args->bias = command->value;
        } else if (!strcmp(command->name, "cal")) {
            args->cal = command->value;
        } else if (!strcmp(command->name, "ccw")) {
            args->ccw = command->value;
        } else if (!strcmp(command->name, "circle")) {
            args->circle = command->value;
        } else if (!strcmp(command->name, "clear")) {
            args->clear = command->value;
        } else if (!strcmp(command->name, "config")) {
            args->config = command->value;
        } else if (!strcmp(command->name, "cw")) {
            args->cw = command->value;
        } else if (!strcmp(command->name, "debug")) {
            args->debug = command->value;
        } else if (!strcmp(command->name, "disable")) {
            args->disable = command->value;
        } else if (!strcmp(command->name, "enable")) {
            args->enable = command->value;
        } else if (!strcmp(command->name, "forward")) {
            args->forward = command->value;
        } else if (!strcmp(command->name, "help")) {
            args->help = command->value;
        } else if (!strcmp(command->name, "left")) {
            args->left = command->value;
        } else if (!strcmp(command->name, "lenc")) {
            args->lenc = command->value;
        } else if (!strcmp(command->name, "linear")) {
            args->linear = command->value;
        } else if (!strcmp(command->name, "lmotor")) {
            args->lmotor = command->value;
        } else if (!strcmp(command->name, "lpid")) {
            args->lpid = command->value;
        } else if (!strcmp(command->name, "motion")) {
            args->motion = command->value;
        } else if (!strcmp(command->name, "motor")) {
            args->motor = command->value;
        } else if (!strcmp(command->name, "odom")) {
            args->odom = command->value;
        } else if (!strcmp(command->name, "out-and-back")) {
            args->out_and_back = command->value;
        } else if (!strcmp(command->name, "params")) {
            args->params = command->value;
        } else if (!strcmp(command->name, "pid")) {
            args->pid = command->value;
        } else if (!strcmp(command->name, "renc")) {
            args->renc = command->value;
        } else if (!strcmp(command->name, "rep")) {
            args->rep = command->value;
        } else if (!strcmp(command->name, "right")) {
            args->right = command->value;
        } else if (!strcmp(command->name, "rmotor")) {
            args->rmotor = command->value;
        } else if (!strcmp(command->name, "rpid")) {
            args->rpid = command->value;
        } else if (!strcmp(command->name, "show")) {
            args->show = command->value;
        } else if (!strcmp(command->name, "square")) {
            args->square = command->value;
        } else if (!strcmp(command->name, "status")) {
            args->status = command->value;
        } else if (!strcmp(command->name, "umbmark")) {
            args->umbmark = command->value;
        } else if (!strcmp(command->name, "val")) {
            args->val = command->value;
        }
    }
    /* arguments */
    for (i=0; i < elements->n_arguments; i++) {
        argument = &elements->arguments[i];
    }
    return 0;
}


/*
 * Main docopt function
 */

DocoptArgs docopt(int argc, char *argv[], bool help, const char *version, int* success) {
    DocoptArgs args = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char*) "360",
        NULL, (char*) "1.0", (char*) "5", NULL, (char*) "10", (char*) "3", NULL,
        NULL, NULL, (char*) "0.8", (char*) "0.2", (char*) "7", (char*) "0.0",
        NULL, NULL, (char*) "1.0", (char*) "0.8",
        usage_pattern, help_message, motor_usage_pattern, motor_help_message,
        pid_usage_pattern, pid_help_message, config_usage_pattern, config_help_message,
        motion_usage_pattern, motion_help_message
    };
    Tokens ts;
    Command commands[] = {
        {"all", 0},
        {"angular", 0},
        {"backward", 0},
        {"bias", 0},
        {"cal", 0},
        {"ccw", 0},
        {"circle", 0},
        {"clear", 0},
        {"config", 0},
        {"cw", 0},
        {"debug", 0},
        {"disable", 0},
        {"enable", 0},
        {"forward", 0},
        {"help", 0},
        {"left", 0},
        {"lenc", 0},
        {"linear", 0},
        {"lmotor", 0},
        {"lpid", 0},
        {"motion", 0},
        {"motor", 0},
        {"odom", 0},
        {"out-and-back", 0},
        {"params", 0},
        {"pid", 0},
        {"renc", 0},
        {"rep", 0},
        {"right", 0},
        {"rmotor", 0},
        {"rpid", 0},
        {"show", 0},
        {"square", 0},
        {"status", 0},
        {"umbmark", 0},
        {"val", 0}
    };
    Argument arguments[] = {
    };
    Option options[] = {
        {"-i", "--impulse", 0, 0, NULL},
        {"-j", "--no-accel", 0, 0, NULL},
        {"-z", "--no-control", 0, 0, NULL},
        {"-q", "--no-pid", 0, 0, NULL},
        {"-p", "--plain-text", 0, 0, NULL},
        {NULL, "--speed", 0, 0, NULL},
        {"-w", "--with-debug", 0, 0, NULL},
        {"-g", "--angle", 1, 0, NULL},
        {NULL, "--angular-speed", 1, 0, NULL},
        {"-s", "--distance", 1, 0, NULL},
        {"-d", "--duration", 1, 0, NULL},
        {"-f", "--first", 1, 0, NULL},
        {"-v", "--intvl", 1, 0, NULL},
        {"-t", "--iters", 1, 0, NULL},
        {"-l", "--left-speed", 1, 0, NULL},
        {NULL, "--linear-speed", 1, 0, NULL},
        {"-m", "--mask", 1, 0, NULL},
        {"-x", "--max-percent", 1, 0, NULL},
        {"-n", "--min-percent", 1, 0, NULL},
        {"-u", "--num-points", 1, 0, NULL},
        {"-a", "--radius", 1, 0, NULL},
        {"-r", "--right-speed", 1, 0, NULL},
        {"-o", "--second", 1, 0, NULL},
        {"-h", "--side", 1, 0, NULL},
        {"-e", "--step", 1, 0, NULL}
    };
    Elements elements = {36, 0, 25, commands, arguments, options};

    *success = 1;
    
    ts = tokens_new(argc, argv);
    if (parse_args(&ts, &elements))
        *success = -1;//exit(EXIT_FAILURE);
    if (elems_to_args(&elements, &args, help, version))
        *success = 0;//exit(EXIT_SUCCESS);
    
    return args;
}
