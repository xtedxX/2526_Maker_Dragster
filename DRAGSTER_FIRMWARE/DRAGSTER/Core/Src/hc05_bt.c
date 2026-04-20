/**
 * @file    hc05_bt.c
 * @brief   HC-05 Bluetooth driver — Dragster project
 * @target  STM32G431CBU6 — USART2 (PA2 TX / PA3 RX)
 */

#include "hc05_bt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bldc_esc.h"

/* -------------------------------------------------------------------------
 * Private helpers
 * ------------------------------------------------------------------------- */

static void prv_Send(BT_Handle_t *hbt, const char *str)
{
    HAL_UART_Transmit(hbt->huart,
                      (const uint8_t *)str,
                      (uint16_t)strlen(str),
                      50U);
}

static void prv_Debug(BT_Handle_t *hbt, const char *str)
{
    if (hbt->hdebug == NULL) return;
    HAL_UART_Transmit(hbt->hdebug,
                      (const uint8_t *)str,
                      (uint16_t)strlen(str),
                      50U);
}

static void prv_ArmRx(BT_Handle_t *hbt)
{
    HAL_UART_Receive_IT(hbt->huart, &hbt->rx_byte, 1U);
}

static inline float prv_Absf(float v) { return v < 0.0f ? -v : v; }

/* -------------------------------------------------------------------------
 * Command processor
 * ------------------------------------------------------------------------- */

static void prv_ProcessCommand(BT_Handle_t *hbt, const char *cmd)
{
    prv_Debug(hbt, "[BT CMD] ");
    prv_Debug(hbt, cmd);
    prv_Debug(hbt, "\r\n");

    /* ---- PING ------------------------------------------------------------ */
    if (strcmp(cmd, "PING") == 0)
    {
        prv_Send(hbt, "$HB\r\n");
    }
    /* ---- INFO ------------------------------------------------------------ */
    else if (strcmp(cmd, "INFO") == 0)
    {
        snprintf(hbt->tx_buf, BT_TX_BUF_SIZE,
                 "$INFO,FW=1.0,RANGE=8G,ODR=800Hz,BAUD=%u\r\n",
                 BT_BAUD_RATE);
        prv_Send(hbt, hbt->tx_buf);
    }
    /* ---- STREAM,ON ------------------------------------------------------- */
    else if (strcmp(cmd, "STREAM,ON") == 0)
    {
        hbt->stream_enabled = true;
        prv_Send(hbt, "$ACK,STREAM,ON\r\n");
    }
    /* ---- STREAM,OFF ------------------------------------------------------ */
    else if (strcmp(cmd, "STREAM,OFF") == 0)
    {
        hbt->stream_enabled = false;
        prv_Send(hbt, "$ACK,STREAM,OFF\r\n");
    }
    /* ---- RESET ----------------------------------------------------------- */
    else if (strcmp(cmd, "RESET") == 0)
    {
        hbt->peak_g = 0.0f;
        prv_Send(hbt, "$ACK,RESET\r\n");
        prv_Debug(hbt, "[BT] Peak reset\r\n");
    }
    /* ---- THROTTLE,xx ----------------------------------------------------- */
    else if (strncmp(cmd, "THROTTLE,", 9) == 0)
    {
        extern ESC_Handle_t esc;
        int pct = atoi(cmd + 9);
        if (pct >= 0 && pct <= 100)
        {
            ESC_SetThrottlePct(&esc, (uint8_t)pct);
            snprintf(hbt->tx_buf, BT_TX_BUF_SIZE,
                     "$ACK,THROTTLE,%d\r\n", pct);
            prv_Send(hbt, hbt->tx_buf);
        }
        else
        {
            prv_Send(hbt, "$ERR,THROTTLE_RANGE\r\n");
        }
    }
    /* ---- MOTOR,STOP ------------------------------------------------------ */
    else if (strcmp(cmd, "MOTOR,STOP") == 0)
    {
        extern ESC_Handle_t esc;
        ESC_Stop(&esc);
        prv_Send(hbt, "$ACK,MOTOR,STOP\r\n");
    }
    /* ---- BURST,<pct>,<ms> ------------------------------------------------ */
    else if (strncmp(cmd, "BURST,", 6) == 0)
    {
        extern ESC_Handle_t esc;

        int pct         = 0;
        int duration_ms = 500;  /* default 500ms if not specified */

        char *comma = strchr(cmd + 6, ',');
        if (comma != NULL)
        {
            pct         = atoi(cmd + 6);
            duration_ms = atoi(comma + 1);
        }
        else
        {
            pct = atoi(cmd + 6);
        }

        if (pct >= 0 && pct <= 100 && duration_ms > 0)
        {
            ESC_SetThrottlePct(&esc, (uint8_t)pct);
            snprintf(hbt->tx_buf, BT_TX_BUF_SIZE,
                     "$ACK,BURST,%d,%d\r\n", pct, duration_ms);
            prv_Send(hbt, hbt->tx_buf);

            HAL_Delay((uint32_t)duration_ms);

            /* Back to idle */
            __HAL_TIM_SET_COMPARE(esc.htim, esc.channel, 200U);
            esc.current_pulse = 200U;
            esc.state         = ESC_STATE_READY;
            prv_Send(hbt, "$ACK,BURST,DONE\r\n");
        }
        else
        {
            prv_Send(hbt, "$ERR,BURST_INVALID\r\n");
        }
    }
    /* ---- Unknown --------------------------------------------------------- */
    else
    {
        prv_Send(hbt, "$ERR,UNKNOWN_CMD\r\n");
    }
}

/* =========================================================================
 * Public API
 * ========================================================================= */

BT_Status_t BT_Init(BT_Handle_t        *hbt,
                    UART_HandleTypeDef  *huart,
                    UART_HandleTypeDef  *hdebug,
                    MMA8452Q_Handle_t   *accel)
{
    memset(hbt, 0, sizeof(BT_Handle_t));

    hbt->huart          = huart;
    hbt->hdebug         = hdebug;
    hbt->accel          = accel;
    hbt->stream_enabled = false;
    hbt->peak_g         = 0.0f;
    hbt->last_hb_ms     = HAL_GetTick();

    prv_ArmRx(hbt);

    prv_Debug(hbt, "[BT] HC-05 driver initialised\r\n");
    prv_Send(hbt, "$INFO,FW=1.0,RANGE=8G,ODR=800Hz,BAUD=9600\r\n");

    return BT_OK;
}

/* -------------------------------------------------------------------------- */

void BT_Task(BT_Handle_t *hbt)
{
    /* ---- 1. Process any complete command --------------------------------- */
    if (hbt->cmd_ready)
    {
        prv_ProcessCommand(hbt, hbt->rx_buf);
        memset(hbt->rx_buf, 0, BT_RX_BUF_SIZE);
        hbt->rx_idx    = 0U;
        hbt->cmd_ready = false;
    }

    /* ---- 2. Read accelerometer ------------------------------------------- */
    float accel_g = 0.0f;

    if (MMA8452Q_IsDataReady(hbt->accel))
    {
        if (MMA8452Q_ReadAccelX(hbt->accel, &accel_g) != MMA8452Q_OK)
        {
            prv_Send(hbt, "$ERR,ACCEL_READ\r\n");
            return;
        }
    }
    else
    {
        return;
    }

    /* ---- 3. Update peak g ------------------------------------------------ */
    float abs_g = prv_Absf(accel_g);
    if (abs_g > hbt->peak_g)
    {
        hbt->peak_g = abs_g;
        snprintf(hbt->tx_buf, BT_TX_BUF_SIZE, "$PEAK,%.3f\r\n", hbt->peak_g);
        prv_Send(hbt, hbt->tx_buf);
    }

    /* ---- 4. Live streaming ----------------------------------------------- */
    if (hbt->stream_enabled)
    {
        snprintf(hbt->tx_buf, BT_TX_BUF_SIZE, "$ACC,%.4f\r\n", accel_g);
        prv_Send(hbt, hbt->tx_buf);
    }

    /* ---- 5. Heartbeat ----------------------------------------------------- */
    uint32_t now = HAL_GetTick();
    if ((now - hbt->last_hb_ms) >= BT_HEARTBEAT_PERIOD_MS)
    {
        hbt->last_hb_ms = now;
        prv_Send(hbt, "$HB\r\n");
    }
}

/* -------------------------------------------------------------------------- */

void BT_UART_RxCpltCallback(BT_Handle_t *hbt)
{
    char c = (char)hbt->rx_byte;

    if (c == '\n' || c == '\r')
    {
        if (hbt->rx_idx > 0U)
        {
            hbt->rx_buf[hbt->rx_idx] = '\0';
            hbt->cmd_ready           = true;
        }
    }
    else
    {
        if (hbt->rx_idx < (BT_RX_BUF_SIZE - 1U))
        {
            hbt->rx_buf[hbt->rx_idx++] = c;
        }
    }

    prv_ArmRx(hbt);
}

/* -------------------------------------------------------------------------- */

void BT_SendString(BT_Handle_t *hbt, const char *str)
{
    prv_Send(hbt, str);
}
