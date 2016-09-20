#include "spi.h"
#include "time.h"
#include "i2c.h"

#define NUM_SPI_DEVICES (4)

static uint32 last_time;

static void SendRequest(uint8 device)
{
    /* Send a request command to the device */
    
    if (SPIM_GetTxBufferSize() == 0)
    {        
        Slave_Select_Write(device);
        SPIM_WriteTxData(CMD_REQUEST);

        /* Wait for the end of the transfer monitoring the SPI_DONE status */
        while (SPIM_GetTxBufferSize() > 0)
        {
        }
        
        /* Clear dummy bytes from RX buffer */
        SPIM_ClearRxBuffer();
    }    
}

static uint8 NullData[MAX_MSG_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void ProcessResponse(uint8 device, uint8* response)
/* Note: Each device will support either IR or US sensors.  Somehow there needs to be a way to map the sensor to its
   position on the robot.  The GT Control of Robots course discussed an abstraction called 'skirt'.  In considering this
   abstraction, there really is no need to know about IR vs US vs any other sensor outside of the Psoc.  Does it make 
   sense to report distances based only on orientation, e.g. 0, <dist>; 45, <dist>, etc.  With this approach the I2C
   interface would need two bytes: 1 byte - orientation (in degees), 1 byte - dist (in cm)

   With this approach, it is possible to get duplication of orientation, e.g., IR and US sensor at the same orientation.

   The question is whether this abstraction should be at the Psoc level or at the Raspberry Pi level?  My preference is
   to think of the Psoc as h/w.  From that perspective, the I2C interface gives register perspective of the low-level
   hardware in the robot (via the Psoc).

   From this perspective, the I2C interface will be an array of IR distances and an array of US distances.  How they are
   mapped and their ultimate meaning occur at the Raspberry Pi level.  So, there should be no front or back in the API, 
   just 0 through N-1.  However, it will be helpful to think about the convention for how the sensors are indexed, e.g.,
   top-to-bottom-left-to-right-front-to-back, bottom-to-top-left-to-right-front-to-back, etc.
 */
{
    uint8 som = response[0];
    uint8 eom = response[11];
    uint8 type = response[1];
    uint8 offset;

    offset = device == 0 || device == 2 ? 0 : 8;
    
    if (som == SOM && eom == EOM)
    {
        switch (type)
        {
            case IR_TYPE:
                /* Deliver IR data */
                I2c_WriteInfraredDistances(offset, response, NUM_SENSORS);                
                break;
        
            case US_TYPE:
                /* Deliver US data */
                I2c_WriteUltrasonicDistances(offset, response, NUM_SENSORS); 
                break;
            
            default:
                /* Something went wrong so do nothing */
                break;
        }
    }
}

static void RecvResponse(uint8 device)
{    
    /* Receive the response from the device */

    uint8 response[MAX_MSG_SIZE];
    uint8 ii;
    
    if (SPIM_GetTxBufferSize() == 0)
    {
        Slave_Select_Write(device);
        
        /* Start transfer */
        SPIM_PutArray(NullData, sizeof(NullData));

        /* Wait for the end of the transfer. Monitor the SPI_DONE status */
        while (SPIM_GetTxBufferSize() > 0)
        {
        }

        /* Clear dummy bytes from TX buffer */
        SPIM_ClearTxBuffer();
        
        for (ii = 0; ii < MAX_MSG_SIZE; ++ii)
        {
            response[ii] = SPIM_ReadRxData();
        }
        
        ProcessResponse(device, response);
    }
}

void Spi_Init()
{
}

void Spi_Start()
{
    SPIM_Start();
    last_time = millis();
}

void Spi_Update()
{
    uint32 delta_time;
    uint8 ii;
    
    delta_time = last_time - millis();
    if (delta_time > 50)
    {
        for (ii = 0; ii < NUM_SPI_DEVICES; ++ii)
        {
            SendRequest(ii);
            RecvResponse(ii);
        }
    }
}
