/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef CAL_H
#define CAL_H

#include <project.h>
    
/* The calibration module supports the following calibrations:
    
    1. Count/Sec to PWM - creates a mapping between count/sec velocities and motor pwm values.  The resulting four tables,
       left-forward, left-reverse, right-forward, right-reverse are stored in EEPROM and read into memory on startup.
    2. PID Gains - under control of an external application, executes step input motion to determine the PID gains.  The
       resulting two sets of gains, one for left and one for right, are stored in EEPROM and read into memory on startup.
    3. Linear Bias - under control of an external application, moves 1 meter forward, stops and waits to receive linear
       bias offset.
    4. Angular Bias - under control of an external application, rotates 360 degrees, stops and waits to receive angular
       bias offset.
    
    
    I think the cal module should be the central module through which all of the different calibrations are handled.
    The control module should detect that calibration has been invoked (a non-zero value in the calibration control)
    and call Cal_Update.
    
    Cal_Update will parse the calibration control and dispatch the approach calibration
        Ex
            if (cal_control & CAL_COUNT_PER_SEC_TO_PWM_BIT) -> perform count/sec to pwm calibration
            if (cal_control & CAL_PID_BIT)                  -> perform PID gain calibration
            if (cal_control & CAL_LINEAR_BIAS)              -> perform linear bias calibration
            if (cal_control & CAL_ANGULAR_BIAS)             -> perform angular bias calibration
    
    
 */
    
/* Calibration control/status bits */
#define CAL_COUNT_PER_SEC_TO_PWM_BIT    (0x0001)
#define CAL_PID_BIT                     (0x0002)
#define CAL_LINEAR_BIAS_BIT             (0x0004)
#define CAL_ANGULAR_BIAS_BIT            (0x0008)
#define CAL_VERBOSE_BIT                 (0x0080)
    
#define CAL_NUM_SAMPLES (51)
#define CAL_SCALE_FACTOR (100)
    
#define CAL_DATA_SIZE (CAL_NUM_SAMPLES)

typedef struct _CAL_DATA_TYPE
{
    int32  cps_min;
    int32  cps_max;
    int    cps_scale;
    int32  cps_data[CAL_DATA_SIZE];
    uint16 pwm_data[CAL_DATA_SIZE];
    // Added to force row alignment
    uint16 reserved; 
} __attribute__ ((packed)) CAL_DATA_TYPE;

typedef struct _cal_pid_tag
{
    float kp;
    float ki;
    float kd;
} __attribute__ ((packed)) CAL_PID_TYPE;

typedef struct _eeprom_tag
{
    // the following fields are padded to 16 bytes (1 row)
    uint16 status;                  /* bit 0: Count/Sec to PWM
                                       bit 1: PID
                                       bit 2: Linear Bias
                                       bit 3: Angular Bias
                                     */
    uint16 checksum;
    uint8 reserved[12];
    // The following fields take up 32 bytes (2 rows) and aligned with the EEPROM row size
    //  sizeof(PID_TYPE) = 12, sizeof(float) = 4
    //  12 + 12 + 4 + 4 = 32 bytes (2 rows)
    CAL_PID_TYPE left_gains;
    CAL_PID_TYPE right_gains;
    float  linear_bias;
    float  angular_bias;
    CAL_DATA_TYPE left_motor_fwd;
    CAL_DATA_TYPE left_motor_bwd;
    CAL_DATA_TYPE right_motor_fwd;
    CAL_DATA_TYPE right_motor_bwd;
} __attribute__ ((packed)) CAL_EEPROM_TYPE;

volatile CAL_EEPROM_TYPE *p_cal_eeprom;

void Cal_Init();
void Cal_Start();
void Cal_Update(uint16 status);
void Cal_Validate();

void Cal_LeftGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);
void Cal_RightGetMotorCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);
CAL_PID_TYPE* Cal_LeftGetPidGains();
CAL_PID_TYPE* Cal_RightGetPidGains();

/* For now, the linear and angular calibration scalings are constants defined in config.h.
   One question is whether it makes sense to use these scalings or try to make the PID do a better job of following the
   given velocity.  One think I noticed is that when a commanded velocity of 0.2 is given, often, the resulting velocity
   is 0.19x.  Is that the best we can do?  Why isn't the PID trying to close the gap?  Do we need more Igain?

   Assuming the PIDs are as good as they can get and we need to use scaling, it would be nice to integrate linear and
   angular calibration into the Psoc.  Maybe steal the basic procedure from the calibrate_linear/calibrate_angular.py
   files.  If linear and angular calibration is done within the Psoc, then it can be repeated (as needed) and repeated
   in order to average values and the values can be stored into EEPROM.
void Cal_LinearCalibration();
        self.position = self.get_position()
        
        x_start = self.position.x
        y_start = self.position.y
            
        move_cmd = Twist()
            
        while not rospy.is_shutdown():
            # Stop the robot by default
            move_cmd = Twist()
            
            if self.start_test:
                # Get the current position from the tf transform between the odom and base frames
                self.position = self.get_position()
                
                # Compute the Euclidean distance from the target point
                distance = sqrt(pow((self.position.x - x_start), 2) +
                                pow((self.position.y - y_start), 2))
                                
                # Correct the estimated distance by the correction factor
                distance *= self.odom_linear_scale_correction
                
                # How close are we?
                error =  distance - self.test_distance
                
                # Are we close enough?
                if not self.start_test or abs(error) <  self.tolerance:
                    self.start_test = False
                    params = {'start_test': False}
                    rospy.loginfo(params)
                    dyn_client.update_configuration(params)
                else:
                    # If not, move in the appropriate direction
                    move_cmd.linear.x = copysign(self.speed, -1 * error)
            else:
                self.position = self.get_position()
                x_start = self.position.x
                y_start = self.position.y
                
            self.cmd_vel.publish(move_cmd)
            r.sleep()

        # Stop the robot
        self.cmd_vel.publish(Twist())

void Cal_AngularCalibration();
        reverse = 1
        
        while not rospy.is_shutdown():
            # Execute the rotation

            if self.start_test:
                # Get the current rotation angle from tf
                self.odom_angle = self.get_odom_angle()
                
                last_angle = self.odom_angle
                turn_angle = 0
                
                # Alternate directions between tests
                reverse = -reverse
                angular_speed  = reverse * self.speed
                
                while abs(turn_angle) < abs(self.test_angle):
                    if rospy.is_shutdown():
                        return
                    move_cmd = Twist()
                    move_cmd.angular.z = angular_speed
                    self.cmd_vel.publish(move_cmd)
                    r.sleep()
                 
                    # Get the current rotation angle from tf                   
                    self.odom_angle = self.get_odom_angle()
                    
                    # Compute how far we have gone since the last measurement
                    delta_angle = self.odom_angular_scale_correction * normalize_angle(self.odom_angle - last_angle)
                    
                    # Add to our total angle so far
                    turn_angle += delta_angle
                    last_angle = self.odom_angle
                
                # Stop the robot
                self.cmd_vel.publish(Twist())
                
                # Update the status flag
                self.start_test = False
                params = {'start_test': False}
                dyn_client.update_configuration(params)
                
            rospy.sleep(0.5) 
                    
        # Stop the robot
        self.cmd_vel.publish(Twist())
*/   
#endif

/* [] END OF FILE */
