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
    memset(formatted_string, 0, sizeof(formatted_string));
}

void Debug_Start()
{
#ifdef COMMS_DEBUG_ENABLED
    //UART_Debug_Start();
#endif
}

/* [] END OF FILE */
