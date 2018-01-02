#include <stdio.h>
#include <string.h>
#include "conparser.h"

#define ENABLE_MOTOR_TESTS
//#define ENABLE_PID_TESTS

#define MAX_NUM_ELMS (10)
#define MAX_STR_LEN (63)
#ifdef ENABLE_MOTOR_TESTS
#define NUM_MOTOR_TESTS (48)
#else
#define NUM_MOTOR_TESTS (0)
#endif
#ifdef ENABLE_PID_TESTS
#define NUM_PID_TESTS (1)
#else
#define NUM_PID_TESTS (0)
#endif
#define NUM_DEBUG_TESTS (13)
#define NUM_CAL_TESTS (38)
#define NUM_MOTION_TESTS (10)
#define NUM_VAL_TESTS (32)

#define NUM_TESTS (NUM_MOTOR_TESTS+NUM_PID_TESTS)

static char args_strs[MAX_NUM_ELMS][MAX_STR_LEN];
static char args_ptrs[sizeof(char *) * MAX_NUM_ELMS] = {0};

static char test_strs[NUM_TESTS][80] = {
    /* Motor Options */
#ifdef ENABLE_MOTOR_TESTS
    "motor --left-speed=0.2",
    "motor --right-speed=0.2",
    "motor --left-speed=0.2 --duration=1.0",
    "motor --right-speed=-0.2 --duration=1.0",
    "motor --left-speed=0.2 --right-speed=-0.2",
    "motor --left-speed=0.2 --right-speed=-0.2 --duration=1.0",

    "motor repeat left --first-speed=0.2 --second-speed=0.8 --interval=5.0",
    "motor repeat right --first-speed=0.2 --second-speed=0.8 --interval=5.0",
    "motor repeat --first-speed=0.2 --second-speed=0.8 --interval=5.0",
    "motor repeat",

    "motor stop left",
    "motor stop right",
    "motor stop",
    "motor show",
    "motor show --plain-text",
    "motor cal left",
    "motor cal right",
    "motor cal",
    "motor cal left --iters=1",
    "motor cal right --iters=1",
    "motor cal --iters=1",
    "motor cal left --iters=2 --with-debug",
    "motor cal right --iters=2 --with-debug",
    "motor cal --iters=2 --with-debug",
    "motor val left forward",
    "motor val left backward",
    "motor val right forward",
    "motor val right backward",
    "motor val forward",
    "motor val backward",
    "motor val left forward --min-percent=0.2",
    "motor val left backward --min-percent=0.2",
    "motor val right forward --min-percent=0.2",
    "motor val right backward --min-percent=0.2",
    "motor val forward --min-percent=0.2",
    "motor val backward --min-percent=0.2",
    "motor val left forward --min-percent=0.2 --max-percent=0.8",
    "motor val left backward --min-percent=0.2 --max-percent=0.8",
    "motor val right forward --min-percent=0.2 --max-percent=0.8",
    "motor val right backward --min-percent=0.2 --max-percent=0.8",
    "motor val forward --min-percent=0.2 --max-percent=0.8",
    "motor val backward --min-percent=0.2 --max-percent=0.8",
    "motor val left forward --min-percent=0.2 --max-percent=0.8 --num-points=9",
    "motor val left backward --min-percent=0.2 --max-percent=0.8 --num-points=9",
    "motor val right forward --min-percent=0.2 --max-percent=0.8 --num-points=9",
    "motor val right backward --min-percent=0.2 --max-percent=0.8 --num-points=9",
    "motor val forward --min-percent=0.2 --max-percent=0.8 --num-points=9",
    "motor val backward --min-percent=0.2 --max-percent=0.8 --num-points=9",
#endif
    /* PID Options */
#ifdef ENABLE_PID_TESTS
    "pid show",
#endif
#ifdef XXX
    "debug enable --mask=0xFFFF",
    "debug disable --mask=0xFFFF",
    "debug enable --mask=0xAAAA",
    "debug disable --mask=0xCCCC",
    "debug enable lmotor",
    "debug enable rmotor",
    "debug enable lenc",
    "debug enable renc",
    "debug enable lpid",
    "debug enable rpid",
    "debug enable odom",
    "debug enable all",    
    "debug show",
    /* Calibration Options */
    "cal motor left",
    "cal motor left --with-debug",
    "cal motor left --iters=4",
    "cal motor left --iters=8 --with-debug",
    "cal motor right",
    "cal motor right --with-debug",
    "cal motor right --iters=10",
    "cal motor right --iters=2 --with-debug",
    "cal motor both",
    "cal motor both --with-debug",
    "cal motor both --iters=7",
    "cal motor both --iters=5 --with-debug",
    "cal pid left --impulse",
    "cal pid left --step=0.5",
    "cal pid left --impulse --with-debug",
    "cal pid left --step=0.7 --with-debug",
    "cal pid right --impulse",
    "cal pid right --step=0.5",
    "cal pid right --impulse --with-debug",
    "cal pid right --step=0.7 --with-debug",
    "cal linear",
    "cal linear --distance=2.0",
    "cal angular",
    "cal angular --angle=180",
    "cal show lmotor",
    "cal show lmotor --plain-text",
    "cal show rmotor",
    "cal show rmotor --plain-text",
    "cal show lpid",
    "cal show lpid --plain-text",
    "cal show rpid",
    "cal show rpid --plain-text",
    "cal show lbias",
    "cal show lbias --plain-text",
    "cal show abias",
    "cal show abias --plain-text",
    "cal show all",
    "cal show all --plain-text",    
    /* Motion Options */
    "motion square left",
    "motion square right",
    "motion square left --side=0.5",
    "motion square right --side=3.0",
    "motion circle cw",
    "motion circle ccw",
    "motion circle cw --radius=3.0",
    "motion circle ccw --radius=2.0",
    "motion out-and-back",
    "motion out-and-back --distance=2",
    /* Validation Options */
    "val motor left forward",
    "val motor left forward --min-percent=0.3",
    "val motor left forward --min-percent=0.2 --max-percent=0.8",
    "val motor left forward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val motor left backward",
    "val motor left backward --min-percent=0.3",
    "val motor left backward --min-percent=0.2 --max-percent=0.8",
    "val motor left backward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val motor right forward",
    "val motor right forward --min-percent=0.3",
    "val motor right forward --min-percent=0.2 --max-percent=0.8",
    "val motor right forward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val motor right backward",
    "val motor right backward --min-percent=0.3",
    "val motor right backward --min-percent=0.2 --max-percent=0.8",
    "val motor right backward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val pid left forward",
    "val pid left forward --min-percent=0.3",
    "val pid left forward --min-percent=0.2 --max-percent=0.8",
    "val pid left forward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val pid left backward",
    "val pid left backward --min-percent=0.3",
    "val pid left backward --min-percent=0.2 --max-percent=0.8",
    "val pid left backward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val pid right forward",
    "val pid right forward --min-percent=0.3",
    "val pid right forward --min-percent=0.2 --max-percent=0.8",
    "val pid right forward --min-percent=0.2 --max-percent=0.8 --num-points=11",
    "val pid right backward",
    "val pid right backward --min-percent=0.3",
    "val pid right backward --min-percent=0.2 --max-percent=0.8",
    "val pid right backward --min-percent=0.2 --max-percent=0.8 --num-points=11"
#endif
};


static void init_args_storage(char **ppargs)
{
    int ii;
    *ppargs = args_ptrs;
    
    for (ii = 0; ii < MAX_NUM_ELMS; ++ii)
    {
        ppargs[ii] = args_strs[ii];
    }
}

static int args_str_array(char *str, char **args)
{
   const char s[2] = " ";
   char *token;
   int index = 0;
   int ii;
   
   /* get the first token */
   token = strtok(str, s);
   
   /* walk through other tokens */
   while( token != NULL ) {
      strcpy(args[index], token);
      index++;
      token = strtok(NULL, s);
   }
   
   return index;
}

static void evaluate_test_string(char *test_str, char **ppargs)
{
    int num_args;
    DocoptArgs parsed;
    int success;

    num_args = args_str_array(test_str, ppargs);    
    parsed = docopt(num_args, ppargs, /* help */ 1, /* version */ "2.0rc2", &success);
    printf("Success: %d\n", success);

    printf("Commands\n");
    printf("    motor == %s\n", parsed.motor ? "true" : "false");
    printf("    repeat == %s\n", parsed.stop ? "true" : "false");
    printf("    stop == %s\n", parsed.stop ? "true" : "false");
    printf("    show == %s\n", parsed.show ? "true" : "false");
    printf("    left == %s\n", parsed.left ? "true" : "false");
    printf("    right == %s\n", parsed.right ? "true" : "false");
    printf("    debug == %s\n", parsed.debug ? "true" : "false");
    printf("    enable == %s\n", parsed.enable ? "true" : "false");
    printf("    disable == %s\n", parsed.disable ? "true" : "false");
    printf("    lmotor == %s\n", parsed.lmotor ? "true" : "false");
    printf("    rmotor == %s\n", parsed.rmotor ? "true" : "false");
    printf("    lenc == %s\n", parsed.lenc ? "true" : "false");
    printf("    renc == %s\n", parsed.renc ? "true" : "false");
    printf("    lpid == %s\n", parsed.lpid ? "true" : "false");
    printf("    rpid == %s\n", parsed.rpid ? "true" : "false");
    printf("    odom == %s\n", parsed.odom ? "true" : "false");
    printf("    all == %s\n", parsed.all ? "true" : "false");
    printf("    bias == %s\n", parsed.bias ? "true" : "false");
    printf("    square == %s\n", parsed.square ? "true" : "false");
    printf("    circle == %s\n", parsed.circle ? "true" : "false");
    printf("    cw == %s\n", parsed.cw ? "true" : "false");
    printf("    ccw == %s\n", parsed.ccw ? "true" : "false");
    printf("    out-and-back == %s\n", parsed.out_and_back ? "true" : "false");
    printf("    forward == %s\n", parsed.forward ? "true" : "false");
    printf("    backward == %s\n", parsed.backward ? "true" : "false");
    printf("    cal == %s\n", parsed.cal ? "true" : "false");
    printf("    val == %s\n", parsed.val ? "true" : "false");
    printf("Flags\n");
    printf("    --plain-text == %s\n", parsed.plain_text ? "true" : "false");
    printf("    --impulse == %s\n", parsed.impulse ? "true" : "false");
    printf("    --with-debug == %s\n", parsed.with_debug ? "true" : "false");
    printf("Options\n");
    printf("    --left-speed == %s\n", parsed.left_speed);
    printf("    --right-speed == %s\n", parsed.right_speed);
    printf("    --duration == %s\n", parsed.duration);
    printf("    --mask == %s\n", parsed.mask);
    printf("    --iters == %s\n", parsed.iters);
    printf("    --step == %s\n", parsed.step);
    printf("    --angle == %s\n", parsed.angle);
    printf("    --distance == %s\n", parsed.distance);
    printf("    --side == %s\n", parsed.side);
    printf("    --radius == %s\n", parsed.radius);
    printf("    --min-percent == %s\n", parsed.min_percent);
    printf("    --max-percent == %s\n", parsed.max_percent);
    printf("    --num-points == %s\n", parsed.num_points);

    //return parsed;
}

int main(int argc, char *argv[])
{
    int num_args;
    char **ppargs;
    int ii;

    DocoptArgs parsed;

    init_args_storage(ppargs);

    for (ii = 0; ii < NUM_TESTS; ++ii)
    {
        printf("---------------------------------------------------------\n");
        printf("Parsing Test %d : %s\n", ii+1, test_strs[ii]);
        printf("---------------------------------------------------------\n");
        evaluate_test_string(test_strs[ii], ppargs);
    }

    return 0;
}
