/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,2011 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "halconf.h"
#include "test.h"
#include "shell.h"

/*===========================================================================*/
/* Command line related.	                                                 */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

static void cmd_mem(BaseChannel *chp, int argc, char *argv[]) {
  size_t n, size;
  char buf[52];

  (void)argv;
  if (argc > 0) {
    shellPrintLine(chp, "Usage: mem");
    return;
  }
  n = chHeapStatus(NULL, &size);
  siprintf(buf, "core free memory : %u bytes", chCoreStatus());
  shellPrintLine(chp, buf);
  siprintf(buf, "heap fragments   : %u", n);
  shellPrintLine(chp, buf);
  siprintf(buf, "heap free total  : %u bytes", size);
  shellPrintLine(chp, buf);
}

static void cmd_threads(BaseChannel *chp, int argc, char *argv[]) {
  static const char *states[] = {
    "READY",
    "CURRENT",
    "SUSPENDED",
    "WTSEM",
    "WTMTX",
    "WTCOND",
    "SLEEPING",
    "WTEXIT",
    "WTOREVT",
    "WTANDEVT",
    "SNDMSG",
    "WTMSG",
    "WTQUEUE",
    "FINAL"
  };
  Thread *tp;
  char buf[60];

  (void)argv;
  if (argc > 0) {
    shellPrintLine(chp, "Usage: threads");
    return;
  }
  shellPrintLine(chp, "    addr    stack prio refs     state time");
  tp = chRegFirstThread();
  do {
    siprintf(buf, "%8lx %8lx %4u %4i %9s %u",
             (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
             (unsigned int)tp->p_prio, tp->p_refs - 1,
             states[tp->p_state], (unsigned int)tp->p_time);
    shellPrintLine(chp, buf);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

static void cmd_test(BaseChannel *chp, int argc, char *argv[]) {
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    shellPrintLine(chp, "Usage: test");
    return;
  }
  tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
                           TestThread, chp);
  if (tp == NULL) {
    shellPrintLine(chp, "out of memory");
    return;
  }
  chThdWait(tp);
}

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseChannel *)&SD1,
  commands
};


/*===========================================================================*/
/* ADC related.                                                              */
/*===========================================================================*/

static void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);

/* Total number of channels to be sampled by a single ADC operation.*/
#define ADC_GRP1_NUM_CHANNELS   2

/* Depth of the conversion buffer, channels are sampled four times each.*/
#define ADC_GRP1_BUF_DEPTH      4

/*
 * ADC samples buffer.
 */
static adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 4 samples of 2 channels, SW triggered.
 * Channels:    IN10   (41.5 cycles sample time)
 *              Sensor (239.5 cycles sample time)
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  ADC_GRP1_NUM_CHANNELS,
  adccb,
  /* HW dependent part.*/
  0,
  ADC_CR2_TSVREFE,
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_41P5) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_239P5),
  0,
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10) | ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)
};

/*
 * ADC end conversion callback.
 * The PWM channels are reprogrammed using the latest ADC samples.
 * The latest samples are transmitted into a single SPI transaction.
 */
void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

  (void) buffer; (void) n;
  /* Note, only in the ADC_COMPLETE state because the ADC driver fires an
     intermediate callback when the buffer is half full.*/
  if (adcp->ad_state == ADC_COMPLETE) {
    adcsample_t avg_ch1, avg_ch2;

    /* Calculates the average values from the ADC samples.*/
    avg_ch1 = (samples[0] + samples[2] + samples[4] + samples[6]) / 4;
    avg_ch2 = (samples[1] + samples[3] + samples[5] + samples[7]) / 4;

    chSysLockFromIsr();

    /* Changes the channels pulse width, the change will be effective
       starting from the next cycle.*/
    pwmEnableChannelI(&PWMD3, 0, PWM_FRACTION_TO_WIDTH(&PWMD3, 4096, avg_ch1));
    pwmEnableChannelI(&PWMD3, 1, PWM_FRACTION_TO_WIDTH(&PWMD3, 4096, avg_ch2));

    chSysUnlockFromIsr();
  }
}

/*===========================================================================*/
/* PWM related.                                                              */
/*===========================================================================*/

static void pwmpcb(PWMDriver *pwmp);
/*
 * PWM configuration structure.
 * Cyclic callback enabled, channels 3 and 4 enabled without callbacks,
 * the active state is a logic one.
 */
static PWMConfig pwmcfg = {
  pwmpcb,
  {
	{PWM_OUTPUT_ACTIVE_HIGH, NULL},
	{PWM_OUTPUT_ACTIVE_HIGH, NULL},
	{PWM_OUTPUT_DISABLED, NULL},
	{PWM_OUTPUT_DISABLED, NULL}
  },
  /* HW dependent part.*/
  PWM_COMPUTE_PSC(STM32_TIMCLK1, 10000),    /* 10KHz PWM clock frequency.   */
  PWM_COMPUTE_ARR(10000, 10000000),       /* PWM period 0.01S (in nS).       */
  0
};

/*
 * PWM cyclic callback.
 * A new ADC conversion is started.
 */
static void pwmpcb(PWMDriver *pwmp) {

  (void)pwmp;

  /* Starts an asynchronous ADC conversion operation, the conversion
     will be executed in parallel to the current PWM cycle and will
     terminate before the next PWM cycle.*/
  chSysLockFromIsr();
  adcStartConversionI(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
  chSysUnlockFromIsr();
}

/*
 * Red LED blinker thread, times are in milliseconds.
 */
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

  return 0;
}

/*
 * Application entry point.
 */
int main(void) {
  Thread *shelltp = NULL;

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
   * Activates the serial driver 3 using the driver default configuration.
   */
  sdStart(&SD3, NULL);

  /*
   * Initializes the ADC driver 1.
   */
  adcStart(&ADCD1, NULL);

  /*
   * Initializes the PWM driver 1, re-routes the TIM3 outputs, programs the
   * pins as alternate functions.
   * Note, the AFIO access routes the TIM3 output pins on the PC6...PC9
   * where the LEDs are connected.
   */
  pwmStart(&PWMD3, &pwmcfg);
  AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_0 | AFIO_MAPR_TIM3_REMAP_1;
  palSetGroupMode(GPIOC, PAL_PORT_BIT(GPIOC_LED1) | PAL_PORT_BIT(GPIOC_LED2),PAL_MODE_STM32_ALTERNATE_PUSHPULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */

	while (TRUE) {
		palClearPad(IOPORT4, GPIOD_LED3);
		chThdSleepMilliseconds(500);
		palSetPad(IOPORT4, GPIOD_LED3);
		chThdSleepMilliseconds(500);
		if (!shelltp) {
			shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
		} else if (chThdTerminated(shelltp)) {
			chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
			shelltp = NULL;           /* Triggers spawning of a new shell.        */
		}
		chThdSleepMilliseconds(100);
	}
//
//  while (TRUE) {
//    if (!palReadPad(IOPORT5, 2))
//      TestThread(&SD1);
//    chThdSleepMilliseconds(500);
//  }

  return 0;
}



