/**
 * @file    bldc_esc.c
 * @brief   Brushless motor ESC driver
 * @target  STM32G431CBU6 @ 170 MHz — TIM3 CH1 — PA6
 */

#include "bldc_esc.h"

/* -------------------------------------------------------------------------
 * Private: clamp and write compare value
 * ------------------------------------------------------------------------- */
static void prv_SetCompare(ESC_Handle_t *hesc, uint32_t pulse)
{
    if (pulse < ESC_PULSE_MIN_US) pulse = ESC_PULSE_MIN_US;
    if (pulse > ESC_PULSE_MAX_US) pulse = ESC_PULSE_MAX_US;
    __HAL_TIM_SET_COMPARE(hesc->htim, hesc->channel, pulse);
    hesc->current_pulse = pulse;
}

/* -------------------------------------------------------------------------
 * Private: percentage → pulse
 *   0%   = 199  (min throttle)
 *   100% = 700  (max throttle)
 * ------------------------------------------------------------------------- */
static uint32_t prv_PctToPulse(uint8_t pct)
{
    if (pct > ESC_MAX_THROTTLE_PCT) pct = ESC_MAX_THROTTLE_PCT;
    uint32_t range = ESC_PULSE_MAX_US - ESC_PULSE_MIN_US;
    return ESC_PULSE_MIN_US + ((uint32_t)pct * range / 100U);
}

/* -------------------------------------------------------------------------
 * Private: pulse → percentage
 * ------------------------------------------------------------------------- */
static uint8_t prv_PulseToPct(uint32_t pulse)
{
    if (pulse <= ESC_PULSE_MIN_US) return 0U;
    if (pulse >= ESC_PULSE_MAX_US) return 100U;
    uint32_t range = ESC_PULSE_MAX_US - ESC_PULSE_MIN_US;
    return (uint8_t)((pulse - ESC_PULSE_MIN_US) * 100U / range);
}

/* =========================================================================
 * Public API
 * ========================================================================= */

void ESC_Init(ESC_Handle_t *hesc, TIM_HandleTypeDef *htim, uint32_t channel)
{
    hesc->htim          = htim;
    hesc->channel       = channel;
    hesc->state         = ESC_STATE_ARMING;
    hesc->current_pulse = ESC_PULSE_MIN_US;

    /*
     * Verified arming sequence:
     *   2999 for 4s → ESC beeps, sees neutral
     *   199  for 2s → ESC arms
     *   2999 for 2s → back to neutral
     * Then motor runs between 199 (min) and 700 (max)
     */
    __HAL_TIM_SET_COMPARE(hesc->htim, hesc->channel, 2999);
    HAL_Delay(4000);

    __HAL_TIM_SET_COMPARE(hesc->htim, hesc->channel, 199);
    HAL_Delay(2000);

    __HAL_TIM_SET_COMPARE(hesc->htim, hesc->channel, 2999);
    HAL_Delay(2000);

    /* Set to min throttle — ready to receive commands */
    __HAL_TIM_SET_COMPARE(hesc->htim, hesc->channel, ESC_PULSE_MIN_US);
    hesc->current_pulse = ESC_PULSE_MIN_US;
    hesc->state         = ESC_STATE_READY;
}

void ESC_SetThrottlePct(ESC_Handle_t *hesc, uint8_t pct)
{
    if (hesc->state < ESC_STATE_READY) return;
    prv_SetCompare(hesc, prv_PctToPulse(pct));
    hesc->state = (pct == 0U) ? ESC_STATE_READY : ESC_STATE_RUNNING;
}

void ESC_SetPulse(ESC_Handle_t *hesc, uint32_t pulse_us)
{
    if (hesc->state < ESC_STATE_READY) return;
    prv_SetCompare(hesc, pulse_us);
    hesc->state = (pulse_us <= ESC_PULSE_MIN_US) ? ESC_STATE_READY : ESC_STATE_RUNNING;
}

void ESC_Stop(ESC_Handle_t *hesc)
{
    __HAL_TIM_SET_COMPARE(hesc->htim, hesc->channel, ESC_PULSE_MIN_US);
    hesc->current_pulse = ESC_PULSE_MIN_US;
    hesc->state         = ESC_STATE_READY;
}

void ESC_RampTo(ESC_Handle_t *hesc, uint8_t target_pct, uint32_t step_ms)
{
    if (hesc->state < ESC_STATE_READY) return;
    if (target_pct > ESC_MAX_THROTTLE_PCT) target_pct = ESC_MAX_THROTTLE_PCT;

    uint8_t current_pct = ESC_GetThrottlePct(hesc);

    if (current_pct < target_pct)
    {
        while (current_pct < target_pct)
        {
            ESC_SetThrottlePct(hesc, ++current_pct);
            HAL_Delay(step_ms);
        }
    }
    else if (current_pct > target_pct)
    {
        while (current_pct > target_pct)
        {
            ESC_SetThrottlePct(hesc, --current_pct);
            HAL_Delay(step_ms);
        }
    }
}

ESC_State_t ESC_GetState(const ESC_Handle_t *hesc)
{
    return hesc->state;
}

uint8_t ESC_GetThrottlePct(const ESC_Handle_t *hesc)
{
    return prv_PulseToPct(hesc->current_pulse);
}
