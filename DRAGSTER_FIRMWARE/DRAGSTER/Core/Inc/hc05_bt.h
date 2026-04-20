/**
 * @file    hc05_bt.h
 * @brief   HC-05 Bluetooth driver — Dragster project
 * @target  STM32G431CBU6
 *
 * USART2 TX → PA2  (HC-05 RX)
 * USART2 RX → PA3  (HC-05 TX)
 * USART1 TX → PA9  (ST-Link debug)
 * USART1 RX → PA10 (ST-Link debug)
 *
 * Commands (phone → MCU):
 *   PING              → $HB
 *   INFO              → $INFO,...
 *   STREAM,ON         → $ACK,STREAM,ON   (live $ACC,<g> every sample)
 *   STREAM,OFF        → $ACK,STREAM,OFF
 *   THROTTLE,<0-100>  → $ACK,THROTTLE,<pct>
 *   MOTOR,STOP        → $ACK,MOTOR,STOP
 *   BURST,<pct>,<ms>  → $ACK,BURST,<pct>,<ms> then $ACK,BURST,DONE
 *   RESET             → $ACK,RESET  (resets peak g)
 *
 * Unsolicited frames (MCU → phone):
 *   $HB               heartbeat every 1s
 *   $ACC,<g>          live accelerometer sample (when streaming)
 *   $ERR,<code>       error notification
 */

#ifndef HC05_BT_H
#define HC05_BT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include "mma8452q.h"
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * Configuration
 * ------------------------------------------------------------------------- */
#define BT_BAUD_RATE            9600U
#define BT_RX_BUF_SIZE          64U
#define BT_TX_BUF_SIZE          80U
#define BT_HEARTBEAT_PERIOD_MS  1000U

/* -------------------------------------------------------------------------
 * Driver status
 * ------------------------------------------------------------------------- */
typedef enum {
    BT_OK    = 0,
    BT_ERROR = 1
} BT_Status_t;

/* -------------------------------------------------------------------------
 * Driver handle
 * ------------------------------------------------------------------------- */
typedef struct {
    UART_HandleTypeDef  *huart;             /*!< BT UART (huart2)             */
    UART_HandleTypeDef  *hdebug;            /*!< Debug UART (huart1, or NULL) */
    MMA8452Q_Handle_t   *accel;             /*!< Accelerometer handle         */

    /* Receive state */
    uint8_t  rx_byte;
    char     rx_buf[BT_RX_BUF_SIZE];
    uint8_t  rx_idx;
    bool     cmd_ready;

    /* Transmit scratch buffer */
    char     tx_buf[BT_TX_BUF_SIZE];

    /* Streaming */
    bool     stream_enabled;

    /* Heartbeat */
    uint32_t last_hb_ms;

    /* Peak g tracking */
    float    peak_g;

} BT_Handle_t;

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */
BT_Status_t BT_Init(BT_Handle_t        *hbt,
                    UART_HandleTypeDef  *huart,
                    UART_HandleTypeDef  *hdebug,
                    MMA8452Q_Handle_t   *accel);

void BT_Task(BT_Handle_t *hbt);
void BT_UART_RxCpltCallback(BT_Handle_t *hbt);
void BT_SendString(BT_Handle_t *hbt, const char *str);

#ifdef __cplusplus
}
#endif

#endif /* HC05_BT_H */
