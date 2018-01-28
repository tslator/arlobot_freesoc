#include <assert.h>
#include "contract.h"
#include "debug.h"

/*---------------------------------------------------------------------------------------------------
 * Constants
 *-------------------------------------------------------------------------------------------------*/
DEFINE_THIS_FILE;

#ifndef NASSERT

void on_assert(char const *file, unsigned line)
{
    DEBUG_PRINT_CRIT("ASSERT: %s - %d", file, line);
#ifndef NDEBUG
    assert(FALSE);
#endif    
}

#endif