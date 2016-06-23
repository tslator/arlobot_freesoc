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

#include "debug.h"


void Debug_Init()
{
#ifdef COMMS_DEBUG_ENABLED    
    memset(formatted_string, 0, sizeof(formatted_string));
#endif    
}

void Debug_Start()
{
}

/* [] END OF FILE */
