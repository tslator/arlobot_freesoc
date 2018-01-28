test_cases = {
    'motor' : [
        "motor --left-speed=0.5",
        "motor --right-speed=0.5",
        "motor --left-speed=0.5 --right-speed=0.5",
        "motor --left-speed=0.5 --right-speed=0.5 --duration=60",
        "motor --left-speed=0.5 --right-speed=0.5 --duration=60 --no-pid",
        "motor --left-speed=0.5 --right-speed=0.5 --duration=60 --no-pid --no-accel",
        "motor --left-speed=0.5 --right-speed=0.5 --duration=60 --no-pid --no-accel --no-control",
        "motor rep --first=0.5 --second=0.5 --interval=10 --iters=100",
        "motor rep left --first=0.5 --second=0.5 --interval=10 --iters=100",
        "motor rep right --first=0.5 --second=0.5 --interval=10 --iters=100",
        "motor rep left right --first=0.5 --second=0.5 --interval=10 --iters=100",
        "motor rep left right --first=0.5 --second=0.5 --interval=10 --iters=100",
        "motor rep left right --first=0.5 --second=0.5 --interval=10 --iters=100 --no-pid",
        "motor rep left right --first=0.5 --second=0.5 --interval=10 --iters=100 --no-pid --no-accel",
        "motor rep left right --first=0.5 --second=0.5 --interval=10 --iters=100 --no-pid --no-accel --no-control",
        "motor show",
        "motor show left",
        "motor show right",
        "motor show left right",
        "motor show left right --plain-text",
        "motor cal",
        "motor cal left",
        "motor cal right",
        "motor cal left right",
        "motor cal left right --iters=10",
        "motor cal left right --iters=20 --no-debug",
        "motor val forward",
        "motor val backward",
        "motor val left forward",
        "motor val left backward",
        "motor val right forward",
        "motor val right backward",
        "motor val left right forward",
        "motor val left right backward",
        "motor val left right backward --min-percent=0.5",
        "motor val left right backward --min-percent=0.5 --max-percent=0.5",
        "motor val left right backward --min-percent=0.5 --max-percent=0.5 --num-points=50",
        "motor help"
    ],
    'config' : [
        "config debug enable --mask=0x0001", 
        "config debug disable --mask=0x0001", 
        "config debug enable lmotor",
        "config debug disable lmotor",
        "config debug enable rmotor",
        "config debug disable rmotor",
        "config debug enable lenc",
        "config debug disable lenc",
        "config debug enable renc",
        "config debug disable renc",
        "config debug enable lpid",
        "config debug disable lpid",
        "config debug enable rpid",
        "config debug disable rpid",
        "config debug enable odom",
        "config debug disable odom",
        "config debug enable all",
        "config debug disable all",
        "config show motor",
        "config show pid",
        "config show debug",
        "config show status",
        "config show params",
        "config clear motor",
        "config clear pid",
        "config clear bias",
        "config clear debug",
        "config clear all",
        "config help"
    ],

}





options = ''
with open('conparser.docopt', 'r') as fh:
    options = fh.read()

if __name__ == "__main__":
    import docopt
    
    for item in test_cases['motor']:
        print(item)
        args = docopt.docopt(options, item)
        for key in args.keys():
            if type(args[key]) is bool:
                if args[key]:
                    print("\t", key, args[key])
            elif type(args[key]) is str:
                if args[key] != '':
                    print("\t", key, args[key])