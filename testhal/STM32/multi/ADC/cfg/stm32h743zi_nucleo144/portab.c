/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    portab.c
 * @brief   Application portability module code.
 *
 * @addtogroup application_portability
 * @{
 */

#include "hal.h"

#include "portab.h"

/*===========================================================================*/
/* Module local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported variables.                                                */
/*===========================================================================*/

/*
 * GPT configuration.
 */
const GPTConfig portab_gptcfg1 = {
  .frequency    =  1000000U,
  .callback     =  NULL,
  .cr2          =  TIM_CR2_MMS_1,   /* MMS = 010 = TRGO on Update Event.    */
  .dier         =  0U
};

const ADCConfig portab_adccfg1 = {
  .difsel       = 0U,
  .calibration  = 0U
};

void adccallback(ADCDriver *adcp);

/*
 * ADC errors callback, should never happen.
 */
void adcerrorcallback(ADCDriver *adcp, adcerror_t err);

/*
 * ADC conversion group 1.
 * Mode:        One shot, 2 channels, SW triggered.
 * Channels:    IN10, IN12.
 */
#if STM32_ADC_DUAL_MODE == FALSE
const ADCConversionGroup portab_adcgrpcfg1 = {
  .circular     = false,
  .num_channels = ADC_GRP1_NUM_CHANNELS,
  .end_cb       = NULL,
  .error_cb     = adcerrorcallback,
  .cfgr         = ADC_CFGR_RES_BITDEPTH,
  .cfgr2        = 0U,
  .ccr          = 0U,
  .pcsel        = ADC_SELMASK_IN10 | ADC_SELMASK_IN12,
  .ltr1         = 0x00000000U,
  .htr1         = 0x03FFFFFFU,
  .ltr2         = 0x00000000U,
  .htr2         = 0x03FFFFFFU,
  .ltr3         = 0x00000000U,
  .htr3         = 0x03FFFFFFU,
  .smpr         = {
    0U,
    ADC_SMPR2_SMP_AN10(ADC_SMPR_SMP_384P5) | ADC_SMPR2_SMP_AN12(ADC_SMPR_SMP_384P5)
  },
  .sqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN10) | ADC_SQR1_SQ2_N(ADC_CHANNEL_IN12),
    0U,
    0U,
    0U
  }
};
#else // STM32_ADC_DUAL_MODE == TRUE
const ADCConversionGroup portab_adcgrpcfg1 = {
  .circular     = false,
  .num_channels = ADC_GRP1_NUM_CHANNELS,
  .end_cb       = NULL,
  .error_cb     = adcerrorcallback,
  .cfgr         = ADC_CFGR_RES_BITDEPTH,
  .cfgr2        = 0U,
  .ccr          = 6U, // ADC_CCR_DUAL_SIM
  .pcsel        = ADC_SELMASK_IN10 | ADC_SELMASK_IN12,
  .ltr1         = 0x00000000U,
  .htr1         = 0x03FFFFFFU,
  .ltr2         = 0x00000000U,
  .htr2         = 0x03FFFFFFU,
  .ltr3         = 0x00000000U,
  .htr3         = 0x03FFFFFFU,
  .smpr         = {
    0U,
    ADC_SMPR2_SMP_AN10(ADC_SMPR_SMP_384P5)
  },
  .sqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN10),
    0U,
    0U,
    0U
  },
  .ssmpr         = {
    0U,
    ADC_SMPR2_SMP_AN12(ADC_SMPR_SMP_384P5)
  },
  .ssqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN12),
    0U,
    0U,
    0U
  }
};
#endif // STM32_ADC_DUAL_MODE


/*
 * ADC conversion group 2.
 * Mode:        Continuous, 2 channels, HW triggered by GPT4-TRGO.
 * Channels:    IN10, IN12.
 */
#if STM32_ADC_DUAL_MODE == FALSE
const ADCConversionGroup portab_adcgrpcfg2 = {
  .circular     = true,
  .num_channels = ADC_GRP2_NUM_CHANNELS,
  .end_cb       = adccallback,
  .error_cb     = adcerrorcallback,
  .cfgr         = ADC_CFGR_RES_BITDEPTH |
                  ADC_CFGR_EXTEN_RISING |
                  ADC_CFGR_EXTSEL_SRC(12),  /* TIM4_TRGO */
  .cfgr2        = 0U,
  .ccr          = 0U,
  .pcsel        = ADC_SELMASK_IN10 | ADC_SELMASK_IN12,
  .ltr1         = 0x00000000U,
  .htr1         = 0x03FFFFFFU,
  .ltr2         = 0x00000000U,
  .htr2         = 0x03FFFFFFU,
  .ltr3         = 0x00000000U,
  .htr3         = 0x03FFFFFFU,
  .smpr         = {
    0U,
    ADC_SMPR2_SMP_AN10(ADC_SMPR_SMP_384P5) | ADC_SMPR2_SMP_AN12(ADC_SMPR_SMP_384P5)
  },
  .sqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN10) | ADC_SQR1_SQ2_N(ADC_CHANNEL_IN12),
    0U,
    0U,
    0U
  }
};
#else // STM32_ADC_DUAL_MODE == TRUE
const ADCConversionGroup portab_adcgrpcfg2 = {
  .circular     = true,
  .num_channels = ADC_GRP2_NUM_CHANNELS,
  .end_cb       = adccallback,
  .error_cb     = adcerrorcallback,
  .cfgr         = ADC_CFGR_RES_BITDEPTH |
                  ADC_CFGR_EXTEN_RISING |
                  ADC_CFGR_EXTSEL_SRC(12),  /* TIM4_TRGO */
  .cfgr2        = 0U,
  .ccr          = 6U, // ADC_CCR_DUAL_SIM
  .pcsel        = ADC_SELMASK_IN10 | ADC_SELMASK_IN12,
  .ltr1         = 0x00000000U,
  .htr1         = 0x03FFFFFFU,
  .ltr2         = 0x00000000U,
  .htr2         = 0x03FFFFFFU,
  .ltr3         = 0x00000000U,
  .htr3         = 0x03FFFFFFU,
  .smpr         = {
    0U,
    ADC_SMPR2_SMP_AN10(ADC_SMPR_SMP_384P5)
  },
  .sqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN10),
    0U,
    0U,
    0U
  },
  .ssmpr         = {
    0U,
    ADC_SMPR2_SMP_AN12(ADC_SMPR_SMP_384P5)
  },
  .ssqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN12),
    0U,
    0U,
    0U
  }
};
#endif // STM32_ADC_DUAL_MODE

/*===========================================================================*/
/* Module local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Module local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Module exported functions.                                                */
/*===========================================================================*/

void portab_setup(void) {

  /* ADC inputs.*/
  palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG); // A1 on Arduino header
  palSetPadMode(GPIOC, 2, PAL_MODE_INPUT_ANALOG); // A4 on Arduino header

  /*
   * Activates the serial driver using the driver default configuration.
   */
  sdStart(&PORTAB_SD1, NULL);

}

/** @} */
