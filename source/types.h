#ifndef TYPES_H
#define TYPES_H
    
/* Handles platform-dependent typing */
#ifndef FREESOC_TEST    
#ifndef CYTYPES_INCLUDE
#define CYTYPES_INCLUDE
#include <cytypes.h>
#endif
#else
#include <stdint.h>
#endif

#ifndef STDBOOL_INCLUDE
#define STDBOOL_INCLUDE
#include <stdbool.h>
#endif

typedef int8_t INT8;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef int16_t INT16;
typedef int32_t INT32;
typedef float FLOAT;
typedef bool BOOL;


typedef enum {WHEEL_LEFT, WHEEL_RIGHT, WHEEL_BOTH} WHEEL_TYPE;
typedef enum {DIR_FORWARD, DIR_BACKWARD, DIR_CW, DIR_CCW} DIR_TYPE;

#endif
