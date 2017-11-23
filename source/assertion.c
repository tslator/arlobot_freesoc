#include "assertion.h"
#include "serial.h"

static char MESSAGE[] = "ASSERT MSG (%s:%d): %s\r\n";

/* When in debug and release modes we want information about the assertion.
   When in debug we want an actual assertion to occur
   When in release we don't want an assertion to occur
 */

#ifdef DEBUG
#define MSG_ACTION(msg)   Ser_PutStringFormat(MESSAGE, __FILE__, __LINE__, msg)
#define TEST_ACTION(test) assert(test)
#elif defined NDEBUG
#define MSG_ACTION(msg)    Ser_PutStringFormat(MESSAGE, __FILE__, __LINE__, msg)
#define TEST_ACTION(test)
#else
#define MSG_ACTION(msg)
#define TEST_ACTION(test)
#endif

void assertion(UINT8 test, char* msg)
{
    if (test == 0)
    {
        MSG_ACTION(msg);
        TEST_ACTION(test);
    }
}
