#ifndef FREESOC_H
#define FREESOC_H

#ifndef STDIO_INCLUDE
#define STDIO_INCLUDE
#include <stdio.h>
#endif

#ifndef MATH_INCLUDE
#define MATH_INCLUDE
#include <math.h>
#endif

#ifndef ASSERT_INCLUDE
#define ASSERT_INCLUDE
#include <assert.h>
#endif

#ifndef FREESOC_TEST
#ifndef PROJECT_INCLUDE
#define PROJECT_INCLUDE
#include <project.h>
#endif
#endif

#ifndef TYPES_INCLUDE
#define TYPES_INCLUDE
#include "types.h"
#endif

#ifndef CONFIG_INCLUDE
#define CONFIG_INCLUDE
#include "config.h"
#endif

#ifdef FREESOC_TEST
#include <stdlib.h>
#define ASSERT(test, msg)   do \
                            { \
                                if (test == 0) \
                                { \
                                    printf("ASSERT MSG: %s\r\n", msg); \
                                } \
                            } while (0)

#else    
#define ASSERT(test, msg)   assert(test);
#endif    



#endif