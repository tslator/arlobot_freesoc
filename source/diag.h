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
 * Methods
 *-------------------------------------------------------------------------------------------------*/    
void Diag_Init();
void Diag_Start();
void Diag_Update();

/* [] END OF FILE */
