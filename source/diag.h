/* 
MIT License

Copyright (c) 2017 Tim Slator

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*---------------------------------------------------------------------------------------------------
   Description: This module provides macros and functions for diagnostics.
 *-------------------------------------------------------------------------------------------------*/
 
#ifndef DIAG_H
#define DIAG_H

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include <stdio.h>
#include <project.h>

/*---------------------------------------------------------------------------------------------------
 * Macros
 *-------------------------------------------------------------------------------------------------*/

/* The following macros provide the capability to measure the processing time of the main loop, 
   control, encoder, pid, and odometry update functions.
*/
#undef MAIN_LOOP_DIAG_ENABLED
#undef CONTROL_UPDATE_DIAG_ENABLED
#undef ENCODER_UPDATE_DIAG_ENABLED
#undef PID_UPDATE_DIAG_ENABLED
#undef ODOM_UPDATE_DIAG_ENABLED

#ifdef MAIN_LOOP_DIAG_ENABLED
#define MAIN_LOOP_START()    Diag_Pin_Write( 1 )
#define MAIN_LOOP_END()      Diag_Pin_Write( 0 )
#else
#define MAIN_LOOP_START()
#define MAIN_LOOP_END()
#endif    

#ifdef CONTROL_UPDATE_DIAG_ENABLED
#define CONTROL_UPDATE_START()  do { Diag_Pin_Write( 0 ); Diag_Pin_Write( 1 ); } while (0)
#define CONTROL_UPDATE_END()    Diag_Pin_Write( 0 )
#else    
#define CONTROL_UPDATE_START()
#define CONTROL_UPDATE_END()
#endif

#ifdef ENCODER_UPDATE_DIAG_ENABLED
#define ENCODER_UPDATE_START()  do { Diag_Pin_Write( 0 ); Diag_Pin_Write( 1 ); } while (0)
#define ENCODER_UPDATE_END()    Diag_Pin_Write( 0 )
#else
#define ENCODER_UPDATE_START()
#define ENCODER_UPDATE_END()
#endif

#ifdef PID_UPDATE_DIAG_ENABLED
#define PID_UPDATE_START()  do { Diag_Pin_Write( 0 ); Diag_Pin_Write( 1 ); } while (0)
#define PID_UPDATE_END()    Diag_Pin_Write( 0 )
#else
#define PID_UPDATE_START()
#define PID_UPDATE_END()
#endif
    
#ifdef ODOM_UPDATE_DIAG_ENABLED
#define ODOM_UPDATE_START()  do { Diag_Pin_Write( 0 ); Diag_Pin_Write( 1 ); } while (0)
#define ODOM_UPDATE_END()    Diag_Pin_Write( 0 )
#else    
#define ODOM_UPDATE_START()
#define ODOM_UPDATE_END()
#endif

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    
void Diag_Init();
void Diag_Start();
void Diag_Update();

#endif 

/* [] END OF FILE */
