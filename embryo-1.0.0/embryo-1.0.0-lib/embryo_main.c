#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <time.h>

#include "Embryo.h"
#include "embryo_private.h"
#include "embrio.h"

#include "chconf.h"

static Embryo_Version _version = { VMAJ, VMIN, VMIC, VREV };
EAPI Embryo_Version *embryo_version = &_version;

static int _embryo_init_count = 0;

// Memory pools (defined extern in embrio.h)
MemoryPool *EP_mp;
Embryo_Program EP_pool[MAX_EMBRIO_VM_NUM];

MemoryPool *EVM_mp;
EmbrioVM EVM_pool[MAX_EMBRIO_VM_NUM];

MemoryPool *Estp_mp;
int Estp_pool[MAX_EMBRIO_VM_NUM];


int currVM = 0;

// Memory heaps
// code
MemoryHeap *code_mh;
Embryo_Cell code_buff[MAX_CODE_SIZE];

// program
MemoryHeap *prog_mh;
Embryo_Cell prog_buff[MAX_CODE_SIZE];

// native calls
MemoryHeap *nc_mh;
Embryo_Native nc_buff[MAX_NATIVE_CALLS];

/*** EXPORTED CALLS ***/

/**
 * @defgroup Embryo_Library_Group Library Maintenance Functions
 *
 * Functions that start up and shutdown the Embryo library.
 */

/**
 * Initialises the Embryo library.
 * @return  The number of times the library has been initialised without being
 *          shut down.
 * @ingroup Embryo_Library_Group
 */
EAPI int embryo_init(void)
{

	if (++_embryo_init_count != 1)
		return _embryo_init_count;

	// srand(time(NULL));

	return _embryo_init_count;
}


/**
 * Shuts down the Embryo library.
 * @return  The number of times the library has been initialised without being
 *          shutdown.
 * @ingroup Embryo_Library_Group
 */
EAPI int embryo_shutdown(void)
{
   if (--_embryo_init_count != 0)
     return _embryo_init_count;

   return _embryo_init_count;
}
