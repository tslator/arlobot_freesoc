#include "usbif.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;
#define USBFS_DEVICE    (0u)
#define CDC_IS_READY_TIMEOUT (10)

/* The buffer size is equal to the maximum packet size of the IN and OUT bulk endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)

/*---------------------------------------------------------------------------------------------------
 * Variables
 *-------------------------------------------------------------------------------------------------*/
static BOOL is_connected;

/*---------------------------------------------------------------------------------------------------
 * Functions
 *-------------------------------------------------------------------------------------------------*/
static BOOL WaitForCDCIsReady(UINT32 timeout)
{
    /* The purpose of this routine is to check if CDC is ready in a non-blocking manner.
       If a serial port is connected to the USB then CDC is ready should return immediately; however
       if nothing is connected to the USB then obviously we're not interested in the output and
       so we don't want the main loop to be blocked by calls to check the CDC.
    */
    UINT32 tick;
    
    tick = timeout;

    while (tick > 0)
    {    
        if (0u != USBUART_GetConfiguration())
        {
            while (0 == USBUART_CDCIsReady())
            {
            }
            is_connected = TRUE;
            return is_connected;
        }
        tick--;
        CyDelayUs(1);
    }
    
    is_connected = FALSE;
    return is_connected;    
}

static void Initialize(void)
{
    UINT32 timeout = 10;
    
    /* During startup or any time there is a configuration change detected via USBIF_Update,
       wait for a configuration to set and then initialize the CDC and set as connected.
    
       If no configuration is detected then consider the USBUART unconnected.
    
     */
        
    /* Initialize IN endpoints when device is configured. */
    while (0u == USBUART_GetConfiguration() && timeout > 0)
    {
        timeout--;
        CyDelayUs(1);
    }
    
    if (timeout > 0)
    {        
        /* Note: This path occurs when the USB cable is attached (at any time).
           Cable insertion registers as a configuration change.
         */
        (void) USBUART_CDC_Init();
        is_connected = TRUE;
    }
    else
    {
        /* Note: This path occurs when the USB cable is not attached at Psoc startup */
        is_connected = FALSE;
    }
    
}

void USBIF_Init(void)
{
    is_connected = FALSE;
}

void USBIF_Start(void)
{
    /* Start USBFS operation with 5-V operation. */
    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);

    Initialize();
    
}

void USBIF_Update()
{
    /* Only check if there has been a configuration change.
       This occurs when a USB cable is plugged into the device.
       Note: No events are signaled when the plug is removed.
    
       The USB interface must be 'pumped' which is done by the routines that read and write.  
       The purpose of this routine is to dynamically discover changes to the USB interface.
     */
    if (0u != USBUART_IsConfigurationChanged())
    {
        Initialize();
    }    
    
}

void USBIF_PutString(CHAR* const str)
{
    if (WaitForCDCIsReady(CDC_IS_READY_TIMEOUT))
    {
        /* Note: I have discovered that if the USB cable is removed after entering this function
           it can get stuck in an internal while loop and remain there until the cable is plugged
           in again.  This is an annoyance and can be resolved by plugging in the cable.
           Just be aware.
         */
        USBUART_PutString(str);
    }    
    
}

UINT8 USBIF_GetAll(CHAR* const data)
{
    UINT8 count = 0;
    UINT8 buffer[USBUART_BUFFER_SIZE];

    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            count = USBUART_GetAll(buffer);
            if (0u != count)
            {
                memset(data, 0, USBUART_BUFFER_SIZE);
                memcpy(data, buffer, count);
            }
        }
    }

    return count;
}

UINT8 USBIF_GetChar()
{
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration())
    {
        /* Check for input data from host. */
        if (0u != USBUART_DataIsReady())
        {
            /* Read received data and re-enable OUT endpoint. */
            return USBUART_GetChar();
        }
    }

    return 0;
}

void USBIF_PutChar(CHAR value)
{
    if (WaitForCDCIsReady(CDC_IS_READY_TIMEOUT))
    {
        USBUART_PutChar(value);
    }
}

/* [] END OF FILE */
