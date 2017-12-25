#ifndef ASSERTION_H
#define ASSERTION_H
    
#include "types.h"

/* Under unit testing assertions are mocked and checked, but that removes the short circuiting behavior making it 
   impossible to test such things as null pointers.  In order to check the failure due to an assertion, the following 
   macros are defined and inserted following all assertion function calls.  There are two defines: return on failure 
   and return value on failure.  Basically, all of the assertion tests are inverted and or'ed into the test parameter.
   If any one of them fails then a return is executed which similates an assert.
    
   The macros are disabled for all other build types.
 */
#ifdef FREESOC_TEST
#define RETURN_ON_FAILURE(test) do { if (test) return; } while (0);    
#define RETURN_VALUE_ON_FAILURE(test, rvalue) do { if (test) return rvalue; } while (0);
#else    
#define RETURN_ON_FAILURE(test)
#define RETURN_VALUE_ON_FAILURE(test, rvalue)
#endif
    
void assertion(UINT8 test, char* const msg, char* const file, int line);

#define ASSERTION(test, msg) assertion(test, msg, __FILE__, __LINE__)

#endif