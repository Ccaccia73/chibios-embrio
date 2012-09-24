#include <stdlib.h>

#include "Embryo.h"
#include "embryo_private.h"
#include "embrio.h"

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
	#ifdef _HY_
		chprintf((BaseChannel*)&SD1, "%s",s);
	#else
		chprintf((BaseChannel*)&SD3, "%s",s);
	#endif
#else
	printf("Stringa: %s\n", s);
#endif
	return 0;
}

static Embryo_Cell
_embryo_sleepXXX(Embryo_Program *ep, Embryo_Cell *params)
{
	(void)ep;
	(void)params;
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
	(void)ep;
	(void)params;
#ifdef _CHIBIOS_VM_
	#ifdef _HY_
		palTogglePad(GPIOC, GPIOC_LED1);
	#else
		palTogglePad(GPIOC, YELLOW_LED);
	#endif
#else
   printf("toggle!\n");
#endif

   return 0;
}


/********* embrio01_INC FUNCTIONS *****************/

static Embryo_Cell
_embrio01_toggleLED1(Embryo_Program *ep, Embryo_Cell *params)
{
	(void)ep;
	(void)params;
#ifdef _CHIBIOS_VM_
	#ifdef _HY_
		palTogglePad(GPIOC, GPIOC_LED1);
	#else
		palTogglePad(GPIOC, GREEN_LED);
	#endif
#else
   printf("toggle green!\n");
#endif

   return 0;
}

static Embryo_Cell
_embrio01_toggleLED2(Embryo_Program *ep, Embryo_Cell *params)
{
	(void)ep;
	(void)params;
#ifdef _CHIBIOS_VM_
	#ifdef _HY_
		palTogglePad(GPIOC, GPIOC_LED2);
	#else
		palTogglePad(GPIOC, YELLOW_LED);
	#endif
#else
   printf("toggle yellow!\n");
#endif

   return 0;
}


static Embryo_Cell
_embrio01_readADC1(Embryo_Program *ep, Embryo_Cell *params)
{
	(void)ep;
	(void)params;
	adcsample_t avg_ch1, avg_ch2, avg_temp, avg_v;
#ifdef _CHIBIOS_VM_

	#ifdef _HY_
		msg_t ret;
		ret = adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);

		if(ret == RDY_OK){
			int i;

			avg_ch1 = 0;
			for (i = 0; i < ADC_GRP1_BUF_DEPTH*ADC_GRP1_NUM_CHANNELS; i+=ADC_GRP1_NUM_CHANNELS) {
				avg_ch1+=samples[i];
			}
			avg_ch1 >>= 2;

			avg_ch2 = 0;
			for (i = 1; i < ADC_GRP1_BUF_DEPTH*ADC_GRP1_NUM_CHANNELS; i+=ADC_GRP1_NUM_CHANNELS) {
				avg_ch2+=samples[i];
			}
			avg_ch2 >>= 2;

			avg_v = 0;
			for (i = 2; i < ADC_GRP1_BUF_DEPTH*ADC_GRP1_NUM_CHANNELS; i+=ADC_GRP1_NUM_CHANNELS) {
				avg_v+=samples[i];
			}
			avg_v >>= 2;

			avg_temp = 0;
			for (i = 3; i < ADC_GRP1_BUF_DEPTH*ADC_GRP1_NUM_CHANNELS; i+=ADC_GRP1_NUM_CHANNELS) {
				avg_temp+=samples[i];
			}
			avg_temp >>= 2;

			// adcStopConversion(&ADCD1);
			chprintf((BaseChannel*)&SD1,"%d %d %d %d\r\n",avg_ch1,avg_ch2,avg_v,avg_temp);

		}else if(ret == RDY_RESET){
			chprintf((BaseChannel*)&SD1,"conv stopped\r\n");
		}else{
			chprintf((BaseChannel*)&SD1,"timeout\r\n");
		}
	#else
		/// todo : implement
		chprintf((BaseChannel*)&SD3,"adc\r\n");
	#endif
#else
   printf("read ADC1!\n");
#endif

   return 0;
}

static Embryo_Cell
_embrio01_readSPI1(Embryo_Program *ep, Embryo_Cell *params)
{
	(void)ep;
	(void)params;
#ifdef _CHIBIOS_VM_
#ifdef _HY_


	chprintf((BaseChannel*)&SD1,"spi\r\n");
#else
	/// todo: implement
	chprintf((BaseChannel*)&SD3,"spi\r\n");
#endif
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
