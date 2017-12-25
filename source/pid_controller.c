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

//*********************************************************************************
// Headers
//*********************************************************************************
#include "pid_controller.h"

//*********************************************************************************
// Macros and Globals
//*********************************************************************************
#define CONSTRAIN(x,lower,upper)    ((x)<(lower)?(lower):((x)>(upper)?(upper):(x)))

//*********************************************************************************
// Functions
//*********************************************************************************
static FLOAT CalcError(FLOAT setpoint, FLOAT input)
{
    return setpoint - input;
}

void PIDInit(PIDControl* const pid, FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf,
             FLOAT sampleTimeSeconds, FLOAT minOutput, FLOAT maxOutput, 
             PIDMode mode, PIDDirection controllerDirection, PIDCalcError calcError)     	
{
    pid->controllerDirection = controllerDirection;
    pid->mode = mode;
    pid->iTerm = 0.0f;
    pid->input = 0.0f;
    pid->lastInput = 0.0f;
    pid->output = 0.0f;
    pid->setpoint = 0.0f;
    pid->calcError = (calcError == (void *)0) ? CalcError : calcError;
    
    if(sampleTimeSeconds > 0.0f)
    {
        pid->sampleTime = sampleTimeSeconds;
    }
    else
    {
        // If the passed parameter was incorrect, set to 1 second
        pid->sampleTime = 1.0f;
    }
    
    PIDOutputLimitsSet(pid, minOutput, maxOutput);
    PIDTuningsSet(pid, kp, ki, kd, kf);
}

BOOL PIDCompute(PIDControl* const pid) 
{
    FLOAT error, dInput;

    if(pid->mode == MANUAL)
    {
        return false;
    }
    
    // The classic PID error term
    //error = (pid->setpoint) - (pid->input);
    error = pid->calcError(pid->setpoint, pid->input);
    
    // Compute the integral term separately ahead of time
    pid->iTerm += (pid->alteredKi) * error;
    
    // Constrain the integrator to make sure it does not exceed output bounds
    pid->iTerm = CONSTRAIN( (pid->iTerm), (pid->outMin), (pid->outMax) );
    
    // Take the "derivative on measurement" instead of "derivative on error"
    dInput = (pid->input) - (pid->lastInput);
    
    // Run all the terms together to get the overall output
    pid->output = pid->alteredKf * pid->setpoint + (pid->alteredKp) * error + (pid->iTerm) - (pid->alteredKd) * dInput;
    
    // Bound the output
    pid->output = CONSTRAIN( (pid->output), (pid->outMin), (pid->outMax) );
    
    // Make the current input the former input
    pid->lastInput = pid->input;
    
    return true;
}
     
void PIDModeSet(PIDControl* const pid, PIDMode mode)                                                                                                                                       
{
    // If the mode changed from MANUAL to AUTOMATIC
    if(pid->mode != mode && mode == AUTOMATIC)
    {
        // Initialize a few PID parameters to new values
        pid->iTerm = pid->output;
        pid->lastInput = pid->input;
        
        // Constrain the integrator to make sure it does not exceed output bounds
        pid->iTerm = CONSTRAIN( (pid->iTerm), (pid->outMin), (pid->outMax) );
    }
    
    pid->mode = mode;
}

void PIDOutputLimitsSet(PIDControl* const pid, FLOAT min, FLOAT max) 							  							  
{
    // Check if the parameters are valid
    if(min >= max)
    {
        return;
    }
    
    // Save the parameters
    pid->outMin = min;
    pid->outMax = max;
    
    // If in automatic, apply the new constraints
    if(pid->mode == AUTOMATIC)
    {
        pid->output = CONSTRAIN(pid->output, min, max);
        pid->iTerm  = CONSTRAIN(pid->iTerm,  min, max);
    }
}

void PIDTuningsSet(PIDControl *pid, FLOAT kp, FLOAT ki, FLOAT kd, FLOAT kf)         	                                         
{
    // Check if the parameters are valid
    if(kp < 0.0f || ki < 0.0f || kd < 0.0f || kf < 0.0f)
    {
        return;
    }
    
    // Save the parameters for displaying purposes
    pid->dispKp = kp;
    pid->dispKi = ki;
    pid->dispKd = kd;
    pid->dispKf = kf;
    
    // Alter the parameters for PID
    pid->alteredKp = kp;
    pid->alteredKi = ki * pid->sampleTime;
    pid->alteredKd = kd / pid->sampleTime;
    pid->alteredKf = kf;
    
    // Apply reverse direction to the altered values if necessary
    if(pid->controllerDirection == REVERSE)
    {
        pid->alteredKp = -(pid->alteredKp);
        pid->alteredKi = -(pid->alteredKi);
        pid->alteredKd = -(pid->alteredKd);
        pid->alteredKf = -(pid->alteredKf);
    }
}

void PIDTuningKpSet(PIDControl* const pid, FLOAT kp)
{
    PIDTuningsSet(pid, kp, pid->dispKi, pid->dispKd, pid->dispKf);
}

void PIDTuningKiSet(PIDControl* const pid, FLOAT ki)
{
    PIDTuningsSet(pid, pid->dispKp, ki, pid->dispKd, pid->dispKf);
}

void PIDTuningKdSet(PIDControl* const pid, FLOAT kd)
{
    PIDTuningsSet(pid, pid->dispKp, pid->dispKi, kd, pid->dispKf);
}

void PIDTunningKfSet(PIDControl* const pid, FLOAT kf)
{
    PIDTuningsSet(pid, pid->dispKp, pid->dispKi, pid->dispKd, kf);
}

void PIDControllerDirectionSet(PIDControl* const pid, PIDDirection controllerDirection)	  									  									  									  
{
    // If in automatic mode and the controller's sense of direction is reversed
    if(pid->mode == AUTOMATIC && controllerDirection == REVERSE)
    {
        // Reverse sense of direction of PID gain constants
        pid->alteredKp = -(pid->alteredKp);
        pid->alteredKi = -(pid->alteredKi);
        pid->alteredKd = -(pid->alteredKd);
        pid->alteredKf = -(pid->alteredKf);
    }
    
    pid->controllerDirection = controllerDirection;
}

void PIDSampleTimeSet(PIDControl* const pid, FLOAT sampleTimeSeconds)                                                       									  									  									   
{
    FLOAT ratio;

    if(sampleTimeSeconds > 0.0f)
    {
        // Find the ratio of change and apply to the altered values
        ratio = sampleTimeSeconds / pid->sampleTime;
        pid->alteredKi *= ratio;
        pid->alteredKd /= ratio;
        
        // Save the new sampling time
        pid->sampleTime = sampleTimeSeconds;
    }
}

void PIDSetpointSet(PIDControl* const pid, FLOAT setpoint) { pid->setpoint = setpoint; }

void PIDInputSet(PIDControl* const pid, FLOAT input) { pid->input = input; }

FLOAT PIDOutputGet(PIDControl* const pid) { return pid->output; }

FLOAT PIDKpGet(PIDControl* const pid) { return pid->dispKp; }						  

FLOAT PIDKiGet(PIDControl* const pid) { return pid->dispKi; }						  

FLOAT PIDKdGet(PIDControl* const pid) { return pid->dispKd; }	

FLOAT PIDKfGet(PIDControl* const pid) { return pid->dispKf; }

PIDMode PIDModeGet(PIDControl* const pid) { return pid->mode; }						  

PIDDirection PIDDirectionGet(PIDControl* const pid) { return pid->controllerDirection; }		



