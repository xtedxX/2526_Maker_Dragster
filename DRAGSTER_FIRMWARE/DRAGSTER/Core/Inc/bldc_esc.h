/**
 * @file    bldc_esc.h
 * @brief   Brushless motor ESC driver
 * @target  STM32G431CBU6 @ 170 MHz — TIM3 CH1 — PA6
 *
 * CubeMX TIM3: PSC=169, ARR=19999, Pulse=199
 *
 * Verified compare values:
 *   199  = min throttle  (0%  — motor barely spinning)
 *   700  = max throttle  (100% — fastest allowed)
 *   2999 = neutral       (arming sequence only)
 *
 * Arming sequence (verified on hardware):
 *   2999 for 4s → 199 for 2s → 2999 for 2s → run at 199–700
 *
 * Add after MX_TIM3_Init() in main.c:
 *   HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
 *   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 199);
 */

#ifndef BLDC_ESC_H
#define BLDC_ESC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * Compare values — verified on hardware
 * ------------------------------------------------------------------------- */
#define ESC_PULSE_NEUTRAL_US    2999U   /*!< Neutral — arming only    */
#define ESC_PULSE_MIN_US        199U    /*!< 0%   — min throttle      */
#define ESC_PULSE_MAX_US        9999U    /*!< 100% — max throttle      */

/* -------------------------------------------------------------------------
 * Safety: max throttle percentage
 * ------------------------------------------------------------------------- */
#define ESC_MAX_THROTTLE_PCT    100U

/* -------------------------------------------------------------------------
 * ESC state
 * ------------------------------------------------------------------------- */
typedef enum {
    ESC_STATE_UNINIT  = 0,
    ESC_STATE_ARMING  = 1,
    ESC_STATE_READY   = 2,
    ESC_STATE_RUNNING = 3,
    ESC_STATE_FAULT   = 4
} ESC_State_t;

/* -------------------------------------------------------------------------
 * Driver handle
 * ------------------------------------------------------------------------- */
typedef struct {
    TIM_HandleTypeDef  *htim;
    uint32_t            channel;
    ESC_State_t         state;
    uint32_t            current_pulse;
} ESC_Handle_t;

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */
void        ESC_Init(ESC_Handle_t *hesc, TIM_HandleTypeDef *htim, uint32_t channel);
void        ESC_SetThrottlePct(ESC_Handle_t *hesc, uint8_t pct);
void        ESC_SetPulse(ESC_Handle_t *hesc, uint32_t pulse_us);
void        ESC_Stop(ESC_Handle_t *hesc);
void        ESC_RampTo(ESC_Handle_t *hesc, uint8_t target_pct, uint32_t step_ms);
ESC_State_t ESC_GetState(const ESC_Handle_t *hesc);
uint8_t     ESC_GetThrottlePct(const ESC_Handle_t *hesc);

#ifdef __cplusplus
}
#endif

#endif /* BLDC_ESC_H */
