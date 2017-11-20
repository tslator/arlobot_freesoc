from math import sin, cos

x = 0
y = 0
theta = 0
alttheta = 0
t = 0.05
t2 = t/2.0
R = 1
L = 1
runtime = 0.5

print("{:0.2f} {:0.2f} {:0.2f}".format(x, y, theta))

for i in range(int(runtime//t)):

    # Radian per Second
    vl = 2.0 #left cnts/s * rad/rev * rev/cnts
    vr = 2.1 #right cnts/s * rad/rev * rev/cnts

    # Meter per Second
    Vl = vl * R
    Vr = vr * R

    # Linear/Angular velocity
    v = (Vr + Vl)/2
    w = (Vr - Vl)/L

    dx = v * cos(theta)
    dy = v * sin(theta)
    dth = w

    print("\tdx {:0.2f} dy {:0.2f} dth {:0.2f}".format(dx, dy, dth))    

    k00 = dx
    k01 = dy
    k02 = w

    k10 = v * cos(theta + t2 * k02)
    k11 = v * sin(theta + t2 * k02)
    k12 = w

    k20 = v * cos(theta + t2 * k12)
    k21 = v * sin(theta + t2 * k12)
    k22 = w 

    k30 = v * cos(theta + t * k22)
    k31 = v * sin(theta + t * k22)
    k32 = w

    x = x + t/6 * (k00 + 2*(k10 + k20) + k30)
    y = y + t/6 * (k01 + 2*(k11 + k21) + k31)
    theta = theta + t/6 * (k02 + 2*(k12 + k22) + k32)
    alttheta  = alttheta + t * w

    print("x {:0.2f} y {:0.2f} theta {:0.2f}/{:0.2f}".format(x, y, theta, alttheta))

