
from math import pi as PI


def uni2diff(v, w, L, R):
    _2_v = 2 * v
    _w_L = w * L
    _2_R = 2 * R
    
    l_v = (_2_v - _w_L) / _2_R
    r_v = (_2_v + _w_L) / _2_R

    return l_v, r_v


def diff2uni(l_v, r_v, L, R):
    v = (l_v + r_v) * (R / 2)
    w = (r_v - l_v) * (R / L)

    return v, w


SEC_PER_MIN = 60.0
RADIAN_PER_REV = 2.0 * PI
 
MAX_WHEEL_RPM = 95.0
WHEEL_RADIUS = 0.0785
WHEEL_DIAMETER = 2.0 * WHEEL_RADIUS
TRACK_WIDTH = 0.392

WHEEL_CIRCUMFERENCE = (PI * WHEEL_DIAMETER)
WHEEL_METER_PER_REV = (WHEEL_CIRCUMFERENCE)
WHEEL_ENCODER_TICK_PER_REV = (500)
WHEEL_COUNT_PER_REV = (WHEEL_ENCODER_TICK_PER_REV * 4)
WHEEL_METER_PER_COUNT = (WHEEL_METER_PER_REV/WHEEL_COUNT_PER_REV)
WHEEL_COUNT_PER_METER = (WHEEL_COUNT_PER_REV/WHEEL_METER_PER_REV)
WHEEL_RADIAN_PER_COUNT = (PI * (WHEEL_DIAMETER/(TRACK_WIDTH*WHEEL_COUNT_PER_REV)))
WHEEL_RADIAN_PER_METER = (PI * (WHEEL_DIAMETER/(TRACK_WIDTH*WHEEL_METER_PER_REV)))

# Wheel maximum angular velocity: 
#    omega = theta / sec = RPM/60 * 2PI
#
MAX_WHEEL_RADIAN_PER_SECOND = ((MAX_WHEEL_RPM / SEC_PER_MIN) * RADIAN_PER_REV)
# Wheel maximum linear velocity:
#    v = omega * radius
#
MAX_WHEEL_METER_PER_SECOND = (MAX_WHEEL_RADIAN_PER_SECOND * WHEEL_RADIUS)

MAX_WHEEL_BACKWARD_LINEAR_VELOCITY = (-MAX_WHEEL_METER_PER_SECOND)
MAX_WHEEL_FORWARD_LINEAR_VELOCITY = (MAX_WHEEL_METER_PER_SECOND)
MAX_WHEEL_CCW_ANGULAR_VELOCITY = (-MAX_WHEEL_RADIAN_PER_SECOND)
MAX_WHEEL_CW_ANGULAR_VELOCITY = (MAX_WHEEL_RADIAN_PER_SECOND)

print("**** WHEEL PARAMETERS ***")
print("Max CW Angular Velocity: ", MAX_WHEEL_CW_ANGULAR_VELOCITY)
print("Max CCW Angular Velocity: ", MAX_WHEEL_CCW_ANGULAR_VELOCITY)
print("Max Forward Linear Velocity: ", MAX_WHEEL_FORWARD_LINEAR_VELOCITY)
print("Max Backward Linear Velocity: ", MAX_WHEEL_BACKWARD_LINEAR_VELOCITY)
print("Max Left/Right Forward Velocity: ", uni2diff(MAX_WHEEL_FORWARD_LINEAR_VELOCITY, 0, TRACK_WIDTH, WHEEL_RADIUS))
print("Max Left/Right Backward Velocity: ", uni2diff(MAX_WHEEL_BACKWARD_LINEAR_VELOCITY, 0, TRACK_WIDTH, WHEEL_RADIUS))

'''
/* Calculate the maximum rotation of the robot */

/* This is the circumference of the circle made when the robot turns in place, i.e., one wheel max forward, one wheel 
   max reverse.  Only in this case is the angular velocity maximum.   
 */

/* Calculate angular velocity of robot rotation from linear velocity of wheel:
    omega = v / r 
    where,
        v is the linear velocity of the wheel
        r is the half the robot track width (distance between wheels)
 */
'''
ROBOT_METER_PER_REV = (PI * TRACK_WIDTH)
MAX_ROBOT_RPM = ((MAX_WHEEL_FORWARD_LINEAR_VELOCITY * 60) / ROBOT_METER_PER_REV)
MAX_ROBOT_RADIAN_PER_SECOND  = ((MAX_ROBOT_RPM / SEC_PER_MIN) * RADIAN_PER_REV)
ROBOT_NUM_WHEEL_ROTATION_PER_ROBOT_REV = (ROBOT_METER_PER_REV / WHEEL_METER_PER_REV) # num of wheel rotations per robot revolution
ROBOT_COUNT_PER_REV =  (ROBOT_NUM_WHEEL_ROTATION_PER_ROBOT_REV * WHEEL_COUNT_PER_REV) # count/robot revolution


print("Max Robot RPM: ", MAX_ROBOT_RPM)
print("Max Robot Angular Velocity: ", MAX_ROBOT_RADIAN_PER_SECOND)
print("Robot Meter per Rev: ", ROBOT_METER_PER_REV)
print("Wheel Rotations Per Rev: ", ROBOT_NUM_WHEEL_ROTATION_PER_ROBOT_REV)
print("Robot Count per Rev: ", ROBOT_COUNT_PER_REV)
 
