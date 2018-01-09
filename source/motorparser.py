import json
import matplotlib.pyplot as plt
import numpy as np


json_list = None

'''
#{ "wheel": "left", 
#"direction": "backward", 
#"min": -4328, 
#"max": 0, 
#"values": [
#    {"cps": -4328, "pwm": 1000},
#    {"cps": -4328, "pwm": 1000},
#    {"cps": -4318, "pwm": 1010},
#    ...
#    {"cps": -50, "pwm": 1470},
#    {"cps": 0, "pwm": 1480},
#    {"cps": 0, "pwm": 1490},
#    {"cps": 0, "pwm": 1500}
#]}
'''

def extract_data_lists():
    results = {}
    for item in json_list:
        d = json.loads(item)
        
        pwm_list = [dd['pwm'] for dd in d['values']]
        cps_list = [dd['cps'] for dd in d['values']]
        
        results[d['wheel']+'-'+d['direction']] = {'pwm':pwm_list, 'cps':cps_list}
        
    return results

def concat_data_lists(d1, d2):
    results = {}
    
    results['pwm'] = d1['pwm'] + d2['pwm']
    results['cps'] = d1['cps'] + d2['cps']
    
    return results

with open('motorresults.txt', 'r') as fh:
    json_list = fh.readlines()
    
data_lists = extract_data_lists()

left_data = concat_data_lists(data_lists['left-backward'], data_lists['left-forward'])
right_data = concat_data_lists(data_lists['right-backward'], data_lists['right-forward'])

left_pwms = np.array(left_data['pwm'])
left_cps = np.array(left_data['cps'])
right_pwms = np.array(left_data['pwm'])
right_cps = np.array(left_data['cps'])

plt.xticks(np.arange(right_pwms[0], right_pwms[-1], 50), rotation=45)
plt.plot(left_pwms, left_cps)
plt.plot(right_pwms, right_cps)

plt.legend(["Left CPS", "Right CPS"], loc='best', title='Legend')

plt.xlabel('PWM')
plt.ylabel('Counts/Second')

plt.show()

