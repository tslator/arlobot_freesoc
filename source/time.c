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
   Description: This module provides macros and functions related to time.
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Includes
 *-------------------------------------------------------------------------------------------------*/    
#include "time.h"
#include "CyLib.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/    
DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/    
static UINT32 ms_counter;

/*---------------------------------------------------------------------------------------------------
 * Prototypes
 *-------------------------------------------------------------------------------------------------*/    
/* Interrupt prototype */
#ifndef FREESOC_TEST    
static CY_ISR_PROTO(SysTickIsrHandler);
#endif
 
/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/    

/*---------------------------------------------------------------------------------------------------
 * Name: SysTickIsrHandler
 * Description: The system tick interrupt handler for tracking milliseconds.
 *  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
#ifndef FREESOC_TEST    
static CY_ISR(SysTickIsrHandler)
{
    ms_counter++;
}
#endif

/*---------------------------------------------------------------------------------------------------
 * Name: Time_Init
 * Description: Initializes the time module
 *  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Time_Init()
{
    ms_counter = 0;
}

/*---------------------------------------------------------------------------------------------------
 * Name: Time_Start
 * Description: Starts the system tick and wires up the ISR.
 *  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
void Time_Start()
{
#ifndef FREESOC_TEST    
    UINT32 ii;
    
    /* Starts SysTick component */
    CySysTickStart();
    
    CySysTickClear();
 
    /* Find unused callback slot. */
    for (ii = 0u; ii < CY_SYS_SYST_NUM_OF_CALLBACKS; ++ii)
    {
        if (CySysTickGetCallback(ii) == NULL)
        {
            /* Set callback */
            CySysTickSetCallback(ii, SysTickIsrHandler);
            break;
        }
    }
#endif    
}

/*---------------------------------------------------------------------------------------------------
 * Name: millis
 * Description: Returns the current millisecond count value.
 *  
 * Parameters: None
 * Return: None
 * 
 *-------------------------------------------------------------------------------------------------*/
UINT32 millis()
{
    return ms_counter;
}

UINT32 systick()
{
    return CySysTickGetValue();
}

/* [] END OF FILE */
