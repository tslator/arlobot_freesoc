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
    uint16 reserved; // Added to force 16 byte alignment
} __attribute__ ((packed)) CAL_DATA_TYPE;

void Cal_Init();
void Cal_Start();
void Cal_Update();
void Cal_Validate();

void Cal_LeftGetCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);
void Cal_RightGetCalData(CAL_DATA_TYPE **fwd_cal_data, CAL_DATA_TYPE **bwd_cal_data);

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
