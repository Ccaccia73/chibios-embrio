/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/

#include "ch.h"
#include "hal.h"
#include "board.h"
#include "test.h"
#include "chprintf.h"
#include "embrio.h"
#include "Embryo.h"



/* test embrio program
extern unsigned char _binary_test01_eaf_start;
extern unsigned char _binary_test01_eaf_end;
extern unsigned char _binary_test01_eaf_size;
*/

/*
extern unsigned char _binary_hello1_eaf_start;
extern unsigned char _binary_hello1_eaf_end;
extern unsigned char _binary_hello1_eaf_size;

extern unsigned char _binary_hello2_eaf_start;
extern unsigned char _binary_hello2_eaf_end;
extern unsigned char _binary_hello2_eaf_size;
*/

// VM0 code
extern unsigned char _binary_vm0_eaf_start;
extern unsigned char _binary_vm0_eaf_end;
extern unsigned char _binary_vm0_eaf_size;

// VM1 code
extern unsigned char _binary_vm1_eaf_start;
extern unsigned char _binary_vm1_eaf_end;
extern unsigned char _binary_vm1_eaf_size;


/*
// TEST
extern unsigned char _binary_blob_bin_start;
extern unsigned char _binary_blob_bin_end;
extern unsigned char _binary_blob_bin_size;
*/

/*
 * Green LED blinker thread, times are in milliseconds.
 */
/*
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palClearPad(GPIOC, GPIOC_LED_STATUS1);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOC, GPIOC_LED_STATUS1);
    chThdSleepMilliseconds(500);
  }
  return (msg_t)1;
}
*/

/*===========================================================================*/
/* GPT                                                                       */
/*===========================================================================*/

/*
 * GPT1
 */

static void gpt1cb(GPTDriver *gptp) {
	(void)gptp;

	EmbrioVM *vm_tmp;
	Embryo_Function ef;

	vm_tmp = vm_man->vm_first;

	while(vm_tmp != NULL){

		ef = embryo_program_function_find(vm_tmp->ep, "@eventGPT1");

		if(ef != EMBRYO_FUNCTION_NONE){
			// chprintf((BaseChannel*)&SD3,"%d.",i);
			chMsgSend(vm_tmp->tp,(msg_t)ef);
		}else{
			// chprintf((BaseChannel*)&SD3,"%dk",i);
		}
		vm_tmp = vm_tmp->vm_next;
		// i++;
	}
}

static const GPTConfig gpt1cfg = {
  1000,     /* 1kHz timer clock.*/
  gpt1cb    /* Timer callback.*/
};

/*
 * GPT2
 */

static void gpt2cb(GPTDriver *gptp) {
	(void)gptp;

	EmbrioVM *vm_tmp;
	Embryo_Function ef;

	vm_tmp = vm_man->vm_first;

	while(vm_tmp != NULL){


		ef = embryo_program_function_find(vm_tmp->ep, "@eventGPT2");

		if(ef != EMBRYO_FUNCTION_NONE){
			chMsgSend(vm_tmp->tp,(msg_t)ef);
			// chprintf((BaseChannel*)&SD3,":");
		}else{
			// chprintf((BaseChannel*)&SD3,"y");
		}

		vm_tmp = vm_tmp->vm_next;
	}
}

static const GPTConfig gpt2cfg = {
  1000,     /* 1kHz timer clock.*/
  gpt2cb    /* Timer callback.*/
};

/*
 * GPT3
 */

static void gpt3cb(GPTDriver *gptp) {
	(void)gptp;

	EmbrioVM *vm_tmp;
	Embryo_Function ef;

	vm_tmp = vm_man->vm_first;

	while(vm_tmp != NULL){


		ef = embryo_program_function_find(vm_tmp->ep, "@eventGPT3");

		if(ef != EMBRYO_FUNCTION_NONE){
			chMsgSend(vm_tmp->tp,(msg_t)ef);
			// chprintf((BaseChannel*)&SD3,";");
		}else{
			// chprintf((BaseChannel*)&SD3,"z");
		}

		vm_tmp = vm_tmp->vm_next;
	}
}

static const GPTConfig gpt3cfg = {
  1000,     /* 1kHz timer clock.*/
  gpt3cb    /* Timer callback.*/
};


/*===========================================================================*/
/* EXTI                                                                      */
/*===========================================================================*/

static void exti0cb(EXTDriver *extp, expchannel_t channel) {

	(void)extp;
	(void)channel;
}

static void exti1cb(EXTDriver  *extp, expchannel_t channel) {

	(void)extp;
	(void)channel;
}

static const EXTConfig exticfg = {
  .channels = {
    {EXT_CH_MODE_RISING_EDGE, exti0cb}, /* EXTI line 0 */
    {EXT_CH_MODE_RISING_EDGE, exti1cb},	/* EXTI line 1 */
  }
};



/*
 * Application entry point.
 */
int main(void) {

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	/*
	 * Creates the blinker thread.
	 */
	// chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);


	embryo_init();
	embrioInit();

	/*
	 * Activates the serial driver 3 using the driver default configuration.
	 */
	sdStart(&SD3, NULL);


	// switch off the leds
	palClearPad(GPIOC, GREEN_LED);
	palClearPad(GPIOC, YELLOW_LED);

	if(vm_man == NULL){
		chprintf((BaseChannel*)&SD3,"\n\n\rVMM NO\r\n");
	}else{
		vm_man->vm_count = 0;
		vm_man->vm_first = NULL;
		vm_man->state = EMBRIOVMM_STOP;
		// chprintf((BaseChannel*)&SD3,"\n\n\rVMM OK\r\n");
	}

	vm[0] = (EmbrioVM*)chPoolAlloc(&EVM_mp);

	if(vm[0] == NULL){
		chprintf((BaseChannel*)&SD3,"VM 0 NO\r\n");
	}else{
		// insert VM in linked list of VMs
		embrioVMMinsert(vm_man, vm[0]);
		// chprintf((BaseChannel*)&SD3,"VM 0 OK\r\n");
		vm[0]->ep = embryo_program_load_local(&_binary_vm0_eaf_start, &_binary_vm0_eaf_end, &_binary_vm0_eaf_size, (BaseChannel*)&SD3, TRUE);

		if(vm[0]->ep == NULL){
			chprintf((BaseChannel*)&SD3,"VM 0 ep NO\r\n");
			vm[0]->state = EMBRIOVM_FAIL;
		}else{
			// chprintf((BaseChannel*)&SD3,"VM 0 ep OK\r\n");
			vm[0]->hook = NULL;
			vm[0]->tp = vmStart(vm[0], NORMALPRIO, "VM0");
		}
	}

	vm[1] = (EmbrioVM*)chPoolAlloc(&EVM_mp);

	if(vm[1] == NULL){
		chprintf((BaseChannel*)&SD3,"VM 1 NO\r\n");
	}else{
		// insert VM in linked list of VMs
		embrioVMMinsert(vm_man, vm[1]);
		// chprintf((BaseChannel*)&SD3,"VM 1 OK\r\n");
		vm[1]->ep = embryo_program_load_local(&_binary_vm1_eaf_start, &_binary_vm1_eaf_end, &_binary_vm1_eaf_size, (BaseChannel*)&SD3, TRUE);

		if(vm[1]->ep == NULL){
			chprintf((BaseChannel*)&SD3,"VM 1 ep NO\r\n");
			vm[1]->state = EMBRIOVM_FAIL;
		}else{
			// chprintf((BaseChannel*)&SD3,"VM 1 ep OK\r\n");
			vm[1]->hook = NULL;
			vm[1]->tp = vmStart(vm[1], NORMALPRIO, "VM1");
		}
	}

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */

	// sdWrite(&SD3,"pippo\n",strlen("pippo\n"));

	chRegSetThreadName("VM Manager");

	gptStart(&GPTD1, &gpt1cfg);
	gptStart(&GPTD2, &gpt2cfg);
	gptStart(&GPTD3, &gpt3cfg);

	gptStartContinuous(&GPTD1, 20001);
	gptStartContinuous(&GPTD2, 12001);
	gptStartContinuous(&GPTD3, 23001);

	while (TRUE) {
		if (palReadPad(GPIOC, GPIOC_SWITCH_TAMPER) == 0){
			// TestThread(&SD3);
			palTogglePad(GPIOC, GREEN_LED);
		}
		chThdSleepMilliseconds(500);
	}
}
