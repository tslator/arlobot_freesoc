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
* Left/Right Enable - output driving relay to power the HB-25 motor drivers
* Left/Right PWM - bi-directional to sense HB-25 initialization and drive PWM

### HB25
The [HB25](https://www.parallax.com/sites/default/files/downloads/29144-HB-25-Motor-Controller-V1.2.pdf) is a motor control made by 
Parallax.  It was part of the first Arlobot but it has been replaced with a different controller in the current Arlobot (see link above).

#### Initialization
The HB25 has a specific startup sequence.  Those who have read through the forums on the Parallax site will know about the power on 
sequence between the Propeller board and the HB25 motor drivers.  Using the Psoc and some digital logic, I was able to implement the 
startup sequence in hardware and eliminate the need for any manual sequencing (see figure 1).

#### PWM
The HB25 presents a servo control interface.  It accepts signal and Vcc inputs.  The Psoc5 provides drag-and-drop PWM component implemented in hardware via UDBs.  Each PWM is 16-bit and driven at 1MHz providing microsecond resolution.

![alt text](https://github.com/tslator/arlobot_freesoc/raw/master/images/hb25.png "Figure 1")
 
### Encoders
The encoders provided with Arlobot have 36 teeth or 144 CPR when using quadrature encoding.  I encountered problems with the encoder
boards provided by Parallax -- several of them failed.  I decided to invest in better encoders ultimately choosing the 500-series  encoder from [US Digital](http://cdn.usdigital.com/assets/datasheets/EM1_datasheet.pdf?k=636093962901874636).  I had to make some modifications in order to mount the encoder (see pictures).  The Psoc provides a drag-and-drop 32-bit quadrature encoder component accepting A/B inputs from the encoder and generating a 32-bit signed count.  With quadrature decoding, I'm now getting 2000 CPR.

![alt text](https://github.com/tslator/arlobot_freesoc/raw/master/images/encoder.png "Figure 2")

## Communication
There are several communication paths with the Psoc including I2C for sending motor commands and receiving odometry and RS-232 for debugging and calibration.

### I2C
The Psoc I2C implementation provides a simple register-based interface.  The Psoc is implemented as an I2C master and provides a set of 
read/write and read-only addresses.  The layout is easily defined using a C structure and pragma for packing.

#### Read/Write
The read/write section defines control bits (device and debug) and left/right velocity (as floating point values).

#### Read-Only
The read-only section defines status (device and calibration), odometry (left/right speed, left/right distance, and heading), front and read sensor readings (ultrasonic and infrared).

### RS-232
The Freesoc has two USB ports: one attached to the programmer and one attached to the Psoc5LP.  Both can be used, but presently, only the
5LP USB port is being used.  The USB port serves dual purpose for debugging messages and also as a calibration terminal interface.  The
terminal interface is available when calibration mode is entered.  A menu system is used to select and perform various calibration
operations.  The results of calibration are stored in NVRAM.

![alt text](https://github.com/tslator/arlobot_freesoc/raw/master/images/comms.png "Figure 3")
