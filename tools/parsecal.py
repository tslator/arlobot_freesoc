

import sys

which_pid = sys.argv[1]

with open('pidcalout.txt', 'r') as f:
    for line in f:
        if line.startswith('{} pid:'.format(which_pid)):
            print(line.rstrip())