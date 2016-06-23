#include "callin.h"

void PerformLinearBiasCalibration(uint8 verbose)
/* 
    Clear Linear Bias calibration bit
    Clear Linear Bias value
    Wait for Linear Bias velocity
    Wait for START command
    START
    Move forward for 1 meter
        Get current position
        Stop motor
        Calculate distance error
        while distance error > theshold
            Get current position
            Calculate distance
            Calculate distance error
            Move motor at linear bias velocity
        Stop motor
    Wait for Linear Bias offset
    Wait for command
        if START the goto START
        if STOP continue
    Update linear bias value in EEPROM
    Set linear bias calibration status bit
    Update linear bias calibration status bit in EEPROM

 */
{
}
