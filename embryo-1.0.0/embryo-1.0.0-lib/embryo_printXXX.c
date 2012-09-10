#include <stdlib.h>

#include "Embryo.h"
#include "embryo_private.h"

#ifdef _CHIBIOS_VM_
	//#include <cross_studio_io.h>
	#include "chconf.h"
	#include "halconf.h"
	#include "ch.h"
	#include "hal.h"
	#include "chprintf.h"
#else
	#include <stdio.h>
	#if ( defined __MINGW32__ ) || ( defined __MINGW64__ )
		#include <windows.h>
		#define sleep(seconds) Sleep((seconds)*1000)
	#else
		#include <unistd.h>
	#endif
#endif


/// FIXME: switch to heap?

#ifdef _CHIBIOS_VM_
#define STRGET(ep, str, par) { \
   Embryo_Cell *___cptr; \
   str = NULL; \
   if ((___cptr = embryo_data_address_get(ep, par))) { \
	int ___l; \
	___l = embryo_data_string_length_get(ep, ___cptr); \
   (str) = (char*)chCoreAlloc(___l + 1); \
	if (str) embryo_data_string_get(ep, ___cptr, str); \
     } }

#else
#define STRGET(ep, str, par) { \
   Embryo_Cell *___cptr; \
   str = NULL; \
   if ((___cptr = embryo_data_address_get(ep, par))) { \
	int ___l; \
	___l = embryo_data_string_length_get(ep, ___cptr); \
	(str) = malloc(___l + 1); \
	if (str) embryo_data_string_get(ep, ___cptr, str); \
     } }

#endif


static Embryo_Program *subscribed_ep;


static Embryo_Cell
_embryo_printXXX_printXXX(Embryo_Program *ep, Embryo_Cell *params)
{
	char *s;

	/* params[1] = str */
	if (params[0] != (1 * sizeof(Embryo_Cell))){
		return -1;
	}

	STRGET(ep, s, params[1]);
#ifdef _CHIBIOS_VM_
	chprintf((BaseChannel*)&SD3, "%s",s);
#else
	printf("Stringa: %s\n", s);
#endif
	return 0;
}

static Embryo_Cell
_embryo_sleepXXX(Embryo_Program *ep, Embryo_Cell *params)
{
#ifdef _CHIBIOS_VM_
   chThdSleepMilliseconds(1000);
#else
   sleep(1);
#endif

   return 0;
}


static Embryo_Cell
_embryo_toggleXXX(Embryo_Program *ep, Embryo_Cell *params)
{
#ifdef _CHIBIOS_VM_
   palTogglePad(GPIOC, YELLOW_LED);
#else
   printf("toggle!\n");
#endif

   return 0;
}


#ifdef _CHIBIOS_VM_TMP
static void _embryo_printXXX_cb(GPTDriver *gptp) {

	(void)gptp;
	palClearPad(IOPORT2, 5);
//	subscribed_ep->max_run_cycles = -1; HOOK
}


static const GPTConfig gpt1cfg = {
	1000,     /* 1KHz timer clock.*/
	_embryo_printXXX_cb    /* Timer callback.*/
};
#endif

/********* embrio01_INC FUNCTIONS *****************/

static _embrio01_toggleLED1(Embryo_Program *ep, Embryo_Cell *params)
{
#ifdef _CHIBIOS_VM_
   palTogglePad(GPIOC, GREEN_LED);
#else
   printf("toggle green!\n");
#endif

   return 0;
}

static _embrio01_toggleLED2(Embryo_Program *ep, Embryo_Cell *params)
{
#ifdef _CHIBIOS_VM_
   palTogglePad(GPIOC, YELLOW_LED);
#else
   printf("toggle yellow!\n");
#endif

   return 0;
}


static _embrio01_readADC1(Embryo_Program *ep, Embryo_Cell *params)
{
#ifdef _CHIBIOS_VM_
   /// todo: implement
#else
   printf("read ADC1!\n");
#endif

   return 0;
}

static _embrio01_readSPI1(Embryo_Program *ep, Embryo_Cell *params)
{
#ifdef _CHIBIOS_VM_
   /// todo: implement
#else
   printf("read SPI1!\n");
#endif

   return 0;
}

/********* Init NATIVE FUNCTIONS *****************/


void _embryo_printXXX_init(Embryo_Program *ep)
{
	subscribed_ep = ep;
	embryo_program_native_call_add(ep, "printXXX", _embryo_printXXX_printXXX);
	embryo_program_native_call_add(ep, "sleepXXX", _embryo_sleepXXX);
	embryo_program_native_call_add(ep, "toggleXXX", _embryo_toggleXXX);
#ifdef _CHIBIOS_VM_TMP
	if(embryo_program_function_find(ep, "@event") != EMBRYO_FUNCTION_NONE) {
		gptStart(&GPTD1, &gpt1cfg);
		subscribed_ep = ep;
	}
#endif
	DEBUG_PRINT(("printXXX registered\n"));
}


void _embryo_embrio01_init(Embryo_Program *ep)
{
	embryo_program_native_call_add(ep, "toggleLED1", _embrio01_toggleLED1);
	embryo_program_native_call_add(ep, "toggleLED2", _embrio01_toggleLED2);
	embryo_program_native_call_add(ep, "readADC1", _embrio01_readADC1);
	embryo_program_native_call_add(ep, "readSPI1", _embrio01_readSPI1);

	DEBUG_PRINT(("embrio01 registered\n"));
}
