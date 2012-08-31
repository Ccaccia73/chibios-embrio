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
	int i;

	if (++_embryo_init_count != 1)
		return _embryo_init_count;

	// srand(time(NULL));

	// initialize and preload the blocks

	chPoolInit( EP_mp, MAX_EMBRIO_VM_NUM * sizeof(Embryo_Program), NULL);
	chPoolInit(EVM_mp, MAX_EMBRIO_VM_NUM * sizeof(EmbrioVM), NULL);

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(EP_mp, (void*)&EP_pool[i]);
	}

	for (i = 0; i < MAX_EMBRIO_VM_NUM; ++i) {
		chPoolFree(EVM_mp, (void*)&EVM_pool[i]);
	}

	// initialize a memory heap to load the code
	chHeapInit(code_mh, (void*)code_buff, MAX_CODE_SIZE * sizeof(Embryo_Cell) );

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
