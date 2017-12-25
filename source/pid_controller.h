//*********************************************************************************
// Arduino PID Library Version 1.0.1 Modified Version for C -
// Platform Independent
// 
// Revision: 1.1
// 
// Description: The PID Controller module originally meant for Arduino made
// platform independent. Some small bugs present in the original Arduino source
// have been rectified as well.
// 
// For a detailed explanation of the theory behind this library, go to:
// http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/
// 
// Revisions can be found here:
// https://github.com/tcleg
// 
// Modified by: Trent Cleghorn , <trentoncleghorn@gmail.com>
// 
// Copyright (C) Brett Beauregard , <br3ttb@gmail.com>
// 
//                                 GPLv3 License
// 
// This program is free software: you can redistribute it and/or modify it under 
// the terms of the GNU General Public License as published by the Free Software 
// Foundation, either version 3 of the License, or (at your option) any later 
// version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY 
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with 
// this program.  If not, see <http://www.gnu.org/licenses/>.
//*********************************************************************************

// 
// Header Guard
// 
#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

//*********************************************************************************
// Headers
//*********************************************************************************
#include "freesoc.h"

// 
// C Binding for C++ Compilers
// 
#ifdef __cplusplus
extern "C"
{
#endif

//*********************************************************************************
// Macros and Globals
//*********************************************************************************

typedef FLOAT (*PIDCalcError)(FLOAT setpoint, FLOAT input);

typedef enum
{
    MANUAL,
    AUTOMATIC
} PIDMode;

typedef enum
{
    DIRECT,
    REVERSE
} PIDDirection;

typedef struct
{
    // 
    // Input to the PID Controller
    // 
    FLOAT input;
    
    // 
    // Previous input to the PID Controller
    // 
    FLOAT lastInput;
    
    // 
    // Output of the PID Controller
    // 
    FLOAT output;
    
    // 
    // Gain constant values that were passed by the user
    // These are for display purposes
    // 
    FLOAT dispKp;
    FLOAT dispKi;
    FLOAT dispKd;
    FLOAT dispKf;
    
    // 
    // Gain constant values that the controller alters for
    // its own use
    // 
    FLOAT alteredKp;
    FLOAT alteredKi;
    FLOAT alteredKd;
    FLOAT alteredKf;
    
    // 
    // The Integral Term
    // 
    FLOAT iTerm;
    
    // 
    // The interval (in seconds) on which the PID controller
    // will be called
    // 
    FLOAT sampleTime;
    
    // 
    // The values that the output will be constrained to
    // 
    FLOAT outMin;
    FLOAT outMax;
    
    // 
    // The user chosen operating point
    // 
    FLOAT setpoint;
    
    // 
    // The sense of direction of the controller
    // DIRECT:  A positive setpoint gives a positive output
    // REVERSE: A positive setpoint gives a negative output
    // 
    PIDDirection controllerDirection;
    
    // 
    // Tells how the controller should respond if the user has
    // taken over manual control or not
    // MANUAL:    PID controller is off.
    // AUTOMATIC: PID controller is on.
    // 
    PIDMode mode;

    //
    // Function that computes the error
    // Default (when NULL is passed) is an internal function that does a simple subtract
    // Note: Using PID control on angles requires other than a simple subtract
    PIDCalcError calcError;
} PIDControl;

//*********************************************************************************
// Prototypes
//*********************************************************************************

// 
// PID Initialize
// Description:
//      Initializes a PIDControl instantiation. This should be called at least once
//      before any other PID functions are called.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      kp - Positive P gain constant value.
//      ki - Positive I gain constant value.
//      kd - Positive D gain constant value.
//      kf - Positive F gain (feedforward) constant value.
//      sampleTimeSeconds - Interval in seconds on which PIDCompute will be called.
//      minOutput - Constrain PID output to this minimum value.
//      maxOutput - Constrain PID output to this maximum value.
//      mode - Tells how the controller should respond if the user has taken over
//             manual control or not.
//             MANUAL:    PID controller is off. User can manually control the 
//                        output.
//             AUTOMATIC: PID controller is on. PID controller controls the output.
//      controllerDirection - The sense of direction of the controller
//                            DIRECT:  A positive setpoint gives a positive output.
//                            REVERSE: A positive setpoint gives a negative output.
// Returns:
//      Nothing.
// 
void PIDInit(PIDControl* const pid, FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf,
             FLOAT sampleTimeSeconds, FLOAT minOutput, FLOAT maxOutput, 
             PIDMode mode, PIDDirection controllerDirection, PIDCalcError calcError);     	

// 
// PID Compute
// Description:
//      Should be called on a regular interval defined by sampleTimeSeconds.
//      Typically, PIDSetpointSet and PIDInputSet should be called immediately
//      before PIDCompute.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      True if in AUTOMATIC. False if in MANUAL.
//                     
BOOL PIDCompute(PIDControl* const pid); 

// 
// PID Mode Set
// Description:
//      Sets the PID controller to a new mode.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      mode - Tells how the controller should respond if the user has taken over
//             manual control or not.
//             MANUAL:    PID controller is off. User can manually control the 
//                        output.
//             AUTOMATIC: PID controller is on. PID controller controls the output.
// Returns:
//      Nothing.
//              
void PIDModeSet(PIDControl* const pid, PIDMode mode);                                                                                                                                       

// 
// PID Output Limits Set
// Description:
//      Sets the new output limits. The new limits are applied to the PID
//      immediately.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      min - Constrain PID output to this minimum value.
//      max - Constrain PID output to this maximum value.
// Returns:
//      Nothing.
// 
void PIDOutputLimitsSet(PIDControl* const pid, FLOAT min, FLOAT max); 							  							  

// 
// PID Tunings Set
// Description:
//      Sets the new gain constant values.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      kp - Positive P gain constant value.
//      ki - Positive I gain constant value.
//      kd - Positive D gain constant value.
//      kf - Positive F gain (feedforward) constant value.
// Returns:
//      Nothing.
// 
void PIDTuningsSet(PIDControl* const pid, FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf);         	                                         

// 
// PID Tuning Gain Constant P Set
// Description:
//      Sets the proportional gain constant value.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      kp - Positive P gain constant value.
// Returns:
//      Nothing.
// 
void PIDTuningKpSet(PIDControl* const pid, FLOAT kp);

// 
// PID Tuning Gain Constant I Set
// Description:
//      Sets the integral gain constant value.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      ki - Positive I gain constant value.
// Returns:
//      Nothing.
// 
void PIDTuningKiSet(PIDControl* const pid, FLOAT ki);

// 
// PID Tuning Gain Constant D Set
// Description:
//      Sets the derivative gain constant value.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      kd - Positive D gain constant value.
// Returns:
//      Nothing.
// 
void PIDTuningKdSet(PIDControl* const pid, FLOAT kd);

// 
// PID Tuning Gain Constant F Set
// Description:
//      Sets the feedforward gain constant value.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      kf - Positive F gain constant value.
// Returns:
//      Nothing.
// 
void PIDTunningKfSet(PIDControl* const pid, FLOAT kf);

// 
// PID Controller Direction Set
// Description:
//      Sets the new controller direction.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      controllerDirection - The sense of direction of the controller
//                            DIRECT:  A positive setpoint gives a positive output
//                            REVERSE: A positive setpoint gives a negative output
// Returns:
//      Nothing.
// 
void PIDControllerDirectionSet(PIDControl* const pid, 
                               PIDDirection controllerDirection);	  									  									  									  

// 
// PID Sample Time Set
// Description:
//      Sets the new sampling time (in seconds).
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      sampleTimeSeconds - Interval in seconds on which PIDCompute will be called.
// Returns:
//      Nothing.
// 
void PIDSampleTimeSet(PIDControl* const pid, FLOAT sampleTimeSeconds);                                                       									  									  									   

// 
// Basic Set and Get Functions for PID Parameters
// 

// 
// PID Setpoint Set
// Description:
//      Alters the setpoint the PID controller will try to achieve.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      setpoint - The desired setpoint the PID controller will try to obtain.
// Returns:
//      Nothing.
// 
void PIDSetpointSet(PIDControl* const pid, FLOAT setpoint);

// 
// PID Input Set
// Description:
//      Should be called before calling PIDCompute so the PID controller will
//      have an updated input value to work with.
// Parameters:
//      pid - The address of a PIDControl instantiation.
//      input - The value the controller will work with.
// Returns:
//      Nothing.
// 
void PIDInputSet(PIDControl* const pid, FLOAT input);

// 
// PID Output Get
// Description:
//      Typically, this function is called after PIDCompute in order to
//      retrieve the output of the controller.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      The output of the specific PID controller.
// 
FLOAT PIDOutputGet(PIDControl* const pid);

// 
// PID Proportional Gain Constant Get
// Description:
//      Returns the proportional gain constant value the particular
//      controller is set to.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      The proportional gain constant.
// 
FLOAT PIDKpGet(PIDControl* const pid);

// 
// PID Integral Gain Constant Get
// Description:
//      Returns the integral gain constant value the particular
//      controller is set to.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      The integral gain constant.
// 
FLOAT PIDKiGet(PIDControl* const pid);

// 
// PID Derivative Gain Constant Get
// Description:
//      Returns the derivative gain constant value the particular
//      controller is set to.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      The derivative gain constant.
// 
FLOAT PIDKdGet(PIDControl* const pid);

// 
// PID Mode Get
// Description:
//      Returns the mode the particular controller is set to.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      MANUAL or AUTOMATIC depending on what the user set the 
//      controller to.
// 
PIDMode PIDModeGet(PIDControl* const pid);

// 
// PID Direction Get
// Description:
//      Returns the direction the particular controller is set to.
// Parameters:
//      pid - The address of a PIDControl instantiation.
// Returns:
//      DIRECT or REVERSE depending on what the user set the
//      controller to.
// 
PIDDirection PIDDirectionGet(PIDControl* const pid);


// 
// End of C Binding
// 
#ifdef __cplusplus
}
#endif

#endif  // PID_CONTROLLER_H
