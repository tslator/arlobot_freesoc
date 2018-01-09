
import json
import sys
import matplotlib.pyplot as plt
import numpy as np

def remove_duplicates(a_list):
    # Remove duplicate entries
    new_list = []
    last_item = None
    for item in a_list:
        if item == last_item:
            continue
            
        new_list.append(item)
        last_item = item

    a_list = new_list

def text_to_json_list(name, filename):
    json_list = []
    with open(filename, 'r') as fh:
        json_list = fh.readlines()
    
    json_list = [ d for d in json_list if name in json.loads(d).keys()]
    #print(json_list)
    #json_list = remove_duplicates(json_list)
    #print(json_list)
    return json_list

class PidResults:
    def __init__(self, name, filename):
        self._name = name + ' pid'
        
        self._list = text_to_json_list(self._name, filename)
        print(len(self._list), type(self._list))
        print(self._list[0])
        first_elem = json.loads(self._list[0])
        last_elem = json.loads(self._list[-1])
        
        self._set_point = first_elem[self._name]['set_point']
        self._ten_percent = self._set_point * 0.1
        self._ninty_percent = self._set_point * 0.9
        self._two_percent = self._set_point * 0.02
        self._set_point_below = self._set_point - self._two_percent
        self._set_point_above = self._set_point + self._two_percent
        self._final_value = last_elem[self._name]['input']
        
        self._extract_input()
        self._extract_setpoint()
        self._extract_error()
        
    def _extract_input(self):
        self._inputs = [ json.loads(d)[self._name]['input'] for d in self._list ]
        
    def _extract_setpoint(self):
        self._set_points = [ json.loads(d)[self._name]['set_point'] for d in self._list ]
        
    def _extract_error(self):
        self._errors = [ json.loads(d)[self._name]['error'] for d in self._list ]
        
    def plot_data(self, plt):
        inp = np.array(self._inputs)
        sp = np.array(self._set_points)
        err = np.array(self._errors)
        
        x = np.arange(0, len(inp) * 0.02, 0.02)
        plt.xticks(np.arange(0, len(x), 0.5), rotation=45)
        plt.plot(x, inp)
        plt.plot(x, sp)
        plt.plot(x, err)
        
        #return inp, sp, err
    

'''
jlist = []

with open('pidresults.txt', 'r') as fh:
    jlist = fh.readlines()

# Remove duplicate entries
new_jlist = []
last_item = None
for item in jlist:
    if item == last_item:
        continue
        
    new_jlist.append(item)
    last_item = item

jlist = new_jlist
    
    
key = list(json.loads(jlist[0]).keys())[0]

set_point = json.loads(jlist[0])[key]['set_point']
ten_percent = set_point * 0.1
ninty_percent = set_point * 0.9
two_percent = set_point * 0.02
set_point_below = set_point - two_percent
set_point_above = set_point + two_percent
final_value = json.loads(jlist[-1])[key]['input']

def extract_input():
    return [ json.loads(d)[key]['input'] for d in jlist ]
    
def extract_setpoint():
    return [ json.loads(d)[key]['set_point'] for d in jlist ]
    
def extract_error():
    return [ json.loads(d)[key]['error'] for d in jlist ]    
    
def find_peak(data):
    max_value = max(data)
    return data.index(max_value), max_value
    
def find_valley(data):
    min_value = min(data)
    return data.index(min_value), min_value
    
def find_value(data, value):
    lower = 0,0
    upper = 0,0
    for ii, d in enumerate(data):
        if d <= value:
            lower = (ii, d)
        else:
            upper = (ii, d)
            break
            
    x2 = ((upper[1] - value)/(upper[1] - lower[1]))*0.05
    x1 = 0.05 - x2
    lower_offset = lower[0] * 0.05
    x = lower_offset + x1
    return x, value

def find_settling_pt(data):
    value = None
    for ii, d in enumerate(data):
        if set_point_below <= d <= set_point_above:
            if value is None:
                value = (ii, d)
        else:
            value = 0,0
        
    return value
    
def calc_stats(data):
    peak = find_peak(data)
    settling_min = find_valley(data[peak[0]:])
    percent_10 = find_value(data, ten_percent)
    percent_90 = find_value(data, ninty_percent)
    settling_pt = find_settling_pt(data)
    settling_pt = 0,0 if settling_pt is None else settling_pt
    
    return peak[1], peak[0] * 0.05, settling_min[1], (percent_90[0] - percent_10[0]) * 0.05, settling_pt[0] * 0.05, ((peak[1] - final_value)/final_value) * 100


inp = extract_input()
sp = extract_setpoint()
err = extract_error()

peak = find_peak(inp)
settling_min = find_valley(inp[peak[0]:])
percent_10 = find_value(inp, ten_percent)
percent_90 = find_value(inp, ninty_percent)
settling_pt = find_settling_pt(inp)

peak_time = peak[0] * 0.05
rise_time = percent_90[0] - percent_10[0]
settling_time = settling_pt[0] * 0.05 if settling_pt is not None else 0.0
overshoot = ((peak[1] - final_value)/final_value) * 100

print(peak, peak_time, settling_min, percent_10, percent_90, rise_time, settling_time, overshoot)

inp = np.array(inp)
sp = np.array(sp)
err = np.array(err)
x = np.arange(0, len(inp) * 0.05, 0.05)

plt.xticks(np.arange(0, len(x), 0.5), rotation=45)
plt.plot(x, inp)
plt.plot(x, sp)
plt.plot(x, err)
x_10 = [percent_10[0], percent_10[0]]
y_10 = [0.0, ninty_percent]
plt.plot(x_10, y_10)
x_90 = [percent_90[0], percent_90[0]]
y_90 = [0.0, ninty_percent]
plt.plot(x_90, y_90)
x_h = [0.0, percent_90[0]]
y_h = [ninty_percent, ninty_percent]
plt.plot(x_h, y_h)

plt.axhline(y=set_point+two_percent)
plt.axhline(y=set_point-two_percent)

plt.xlabel('Seconds')
plt.ylabel('Counts/Second')
plt.legend(["input", "set point", "error"], loc='best', title='Legend')
stats = "Peak: {:.3f} cps\nPeak Time: {:.3f} s\nSettling (min): {:.3f} cps\nSettling (max): {:.3f} cps\nRise Time: {:.3f} s\nSettling Time: {:.3f} s\nOvershoot: {:.3f}%".format(
    peak[1], peak_time, settling_min[1], peak[1], rise_time, settling_time, overshoot)
plt.text(2.0, 600, stats)

plt.show()
'''
pid_overlay = True
if (pid_overlay):

    left_pid = PidResults("left", "pidresults.txt")
    right_pid = PidResults("right", "pidresults.txt")

    left_pid.plot_data(plt)
    
    right_pid.plot_data(plt)

    plt.xlabel('Seconds')
    plt.ylabel('Counts/Second')
    plt.legend(["input", "set point", "error"], loc='best', title='Legend')
    
    plt.show()
