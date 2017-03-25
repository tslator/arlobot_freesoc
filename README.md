# Arlobot (freesoc)

The repo contains the implementation of motor control for the Arlobot as realized using the Sparkfun Freesoc2.  Here are some linkes for 
information about Arlobot:
* [Parallax's Arlobot](https://www.parallax.com/product/arlo-robotic-platform-system)
* [Chrisl8's Arlobot (repo is forked here)](https://github.com/chrisl8/ArloBot)
* [Arlobot RPi](https://github.com/tslator/arlobot_rpi)
* [Arlobot TX2 (Comming soon!)](https://github.com/tslator/arlobot_tx2)

## Motor Control
Motor control consists of two PWM components with associated digital logic for initialization and output enable and two HB25 motor 
controllers.  The Psoc exposes two sets of pins:
* Left PWM output
* Left Enable
* Right PWM output
* Right Enable

### HB25
The [HB25](https://www.parallax.com/sites/default/files/downloads/29144-HB-25-Motor-Controller-V1.2.pdf) is a motor control made by 
Parallax.  It was part of the first Arlobot but it has been replaced with a different controller in the current Arlobot (see link above).

#### Initialization
The HB25 has a specific startup sequence.  Those who have read through the forums on the Parallax site will know about the power on 
sequence between the Propeller board and the HB25 motor drivers.  Using the Psoc and some digital logic, I was able to implement the 
startup sequence in hardware and eliminate the need for any manual sequencing [Figure 1](https://github.com/tslator/arlobot_freesoc/images/hb25.png).

#### PWM
The HB25 presents a servo control interface.  It accepts signal and Vcc inputs.  The Psoc5 provides drag-and-drop PWM component implemented
in hardware via UDBs.  Each PWM is 16-bit and driven at 1MHz providing microsecond resolution.

### Encoders

## Communication

### I2C

### RS-232

Freesoc2 Implementation for the Arlobot Robot
