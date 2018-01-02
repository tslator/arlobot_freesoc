'''Usage:
    conparser.py motor (--left-speed=<speed> | --right-speed=<speed> | --left-speed=<speed> --right-speed=<speed>) [--duration=<duration>]
    conparser.py motor repeat [left|right] --first-speed=<speed> --second-speed=<speed> --interval=<interval>
    conparser.py motor stop [left|right]
    conparser.py motor show [--plain-text]
    conparser.py motor cal [left|right] [--iters=<iters>] [--with-debug]
    conparser.py motor val [left|right] (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]
    conparser.py pid cal (left|right) ([--impulse] | [--step=<step>]) [--with-debug]
    conparser.py pid val [left|right] (forward|backward) [--min-percent=<percent>] [--max-percent=<percent>] [--num-points=<points>]
    conparser.py pid show [left|right] [--plain-text]
    conparser.py config debug (enable|disable) ([lmotor|rmotor|lenc|renc|lpid|rpid|odom|all] | --mask=<mask>)
    conparser.py config show [motor|encoder|pid|bias|debug] [--plain-text]
    conparser.py config clear (motor|encoder|pid|bias|debug|all) [--plain-text]
    conparser.py motion cal linear [--distance=<distance>]
    conparser.py motion cal angular [--angle=<angle>]
    conparser.py motion val linear [--distance=<distance>]
    conparser.py motion val angular [--angle=<angle>]
    conparser.py motion val square (left|right) [--side=<side>]
    conparser.py motion val circle (cw|ccw) [--radius=<radius>]
    conparser.py motion out-and-back [--distance=<distance>]
    

Options:
    --speed=<speed>          Speed of the motor (meter/second)
    --duration=<duration>    Duration in seconds [default: 5]
    --mask=<mask>            Bitmap of debug flags
    --plain-text             Display output as plain text (default is JSON)
    --with-debug             Enable PID debug output
    --impulse                Enable impulse response
    --distance=<distance>    Amount of travel (meter) [default: 1.0]
    --angle=<angle>          Amount of travel (degree)   [default: 360] 
    --iters=<iters>          Number of iterations per wheel [default: 3]
    --step=<step>            Percentage of maximum speed to use for step response [default: 0.8]
    --radius=<radius>        Radius of the circle [default: 0.0]
    --side=<side>            Side of the square [default: 1.0]
    --min-percent=<percent>  Minimum value for profile range specified in percent of maximum speed [default: 0.2]
    --max-percent=<percent>  Maximum value for profile range specified in percent of maximum speed [default: 0.8]
    --first-speed=<speed>    First speed of the cycle
    --second-speed=<speed>   Second speed of the cycle
    --interval=<interval>    Time between speed change
    --num-points=<points>    Number of velocity values [default: 7]
'''

if __name__ == "__main__":
    import docopt
    
    print(docopt.docopt(__doc__))