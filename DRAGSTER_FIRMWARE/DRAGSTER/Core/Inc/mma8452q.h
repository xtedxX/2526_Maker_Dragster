/**
 * @file    mma8452q.h
 * @brief   Driver for MMA8452Q 3-axis accelerometer — X-axis only (Dragster project)
 * @target  STM32G431CBU6 — I2C1 (SCL: PA15 / SDA: PB7)
 *
 * Interrupt pins:
 *   INT1 → PC13 (GPIO_Input, label: INT_1)
 *   INT2 → PB9  (GPIO_Input, label: INT_2)
 */

#ifndef MMA8452Q_H
#define MMA8452Q_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * I2C address (SA0 pin → GND = 0x1C, SA0 pin → VCC = 0x1D)
 * Shift left by 1 for HAL 8-bit format
 * ------------------------------------------------------------------------- */
#define MMA8452Q_I2C_ADDR_SA0_LOW   (0x1C << 1)
#define MMA8452Q_I2C_ADDR_SA0_HIGH  (0x1D << 1)
#define MMA8452Q_I2C_ADDR           MMA8452Q_I2C_ADDR_SA0_LOW  /* Default */

/* -------------------------------------------------------------------------
 * Register map (relevant subset)
 * ------------------------------------------------------------------------- */
#define MMA8452Q_REG_STATUS         0x00
#define MMA8452Q_REG_OUT_X_MSB      0x01
#define MMA8452Q_REG_OUT_X_LSB      0x02
#define MMA8452Q_REG_WHO_AM_I       0x0D
#define MMA8452Q_REG_XYZ_DATA_CFG   0x0E
#define MMA8452Q_REG_HP_FILTER_CUTOFF 0x0F
#define MMA8452Q_REG_CTRL_REG1      0x2A
#define MMA8452Q_REG_CTRL_REG2      0x2B
#define MMA8452Q_REG_CTRL_REG3      0x2C
#define MMA8452Q_REG_CTRL_REG4      0x2D
#define MMA8452Q_REG_CTRL_REG5      0x2E

/* -------------------------------------------------------------------------
 * Expected WHO_AM_I value
 * ------------------------------------------------------------------------- */
#define MMA8452Q_WHO_AM_I_VALUE     0x2A

/* -------------------------------------------------------------------------
 * Full-scale range options (XYZ_DATA_CFG register bits [1:0])
 * ------------------------------------------------------------------------- */
typedef enum {
    MMA8452Q_RANGE_2G = 0x00,   /*!< ±2 g — highest resolution */
    MMA8452Q_RANGE_4G = 0x01,   /*!< ±4 g */
    MMA8452Q_RANGE_8G = 0x02    /*!< ±8 g — recommended for dragster */
} MMA8452Q_Range_t;

/* -------------------------------------------------------------------------
 * Output data rate options (CTRL_REG1 bits [5:3])
 * ------------------------------------------------------------------------- */
typedef enum {
    MMA8452Q_ODR_800HZ  = 0x00,
    MMA8452Q_ODR_400HZ  = 0x08,
    MMA8452Q_ODR_200HZ  = 0x10,
    MMA8452Q_ODR_100HZ  = 0x18,
    MMA8452Q_ODR_50HZ   = 0x20,
    MMA8452Q_ODR_12HZ   = 0x28,
    MMA8452Q_ODR_6HZ    = 0x30,
    MMA8452Q_ODR_1HZ    = 0x38
} MMA8452Q_ODR_t;

/* -------------------------------------------------------------------------
 * Driver status codes
 * ------------------------------------------------------------------------- */
typedef enum {
    MMA8452Q_OK    = 0,
    MMA8452Q_ERROR = 1,
    MMA8452Q_BUSY  = 2
} MMA8452Q_Status_t;

/* -------------------------------------------------------------------------
 * Driver handle / configuration structure
 * ------------------------------------------------------------------------- */
typedef struct {
    I2C_HandleTypeDef *hi2c;        /*!< Pointer to HAL I2C handle (hi2c1)   */
    uint8_t            i2c_addr;    /*!< 8-bit I2C address (already shifted)  */
    MMA8452Q_Range_t   range;       /*!< Selected full-scale range            */
    MMA8452Q_ODR_t     odr;         /*!< Selected output data rate            */
    float              sensitivity; /*!< LSB-to-g factor (computed at init)   */
} MMA8452Q_Handle_t;

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */

/**
 * @brief  Initialise the MMA8452Q and put it in active mode.
 * @param  dev   Pointer to a user-allocated MMA8452Q_Handle_t.
 *               Fill hi2c, i2c_addr, range and odr before calling.
 * @retval MMA8452Q_OK on success, MMA8452Q_ERROR otherwise.
 */
MMA8452Q_Status_t MMA8452Q_Init(MMA8452Q_Handle_t *dev);

/**
 * @brief  Read the raw 12-bit X-axis value.
 * @param  dev      Pointer to initialised handle.
 * @param  raw_x    Output: signed 12-bit value (–2048 … +2047).
 * @retval MMA8452Q_OK / MMA8452Q_ERROR.
 */
MMA8452Q_Status_t MMA8452Q_ReadRawX(MMA8452Q_Handle_t *dev, int16_t *raw_x);

/**
 * @brief  Read X-axis acceleration in g (float).
 * @param  dev      Pointer to initialised handle.
 * @param  accel_g  Output: acceleration in g (e.g. 1.0f = 1 g forward).
 * @retval MMA8452Q_OK / MMA8452Q_ERROR.
 */
MMA8452Q_Status_t MMA8452Q_ReadAccelX(MMA8452Q_Handle_t *dev, float *accel_g);

/**
 * @brief  Check whether new X-axis data is ready (polls STATUS register).
 * @param  dev   Pointer to initialised handle.
 * @retval true if data ready, false otherwise.
 */
bool MMA8452Q_IsDataReady(MMA8452Q_Handle_t *dev);

/**
 * @brief  Put the device into standby mode (low power).
 * @param  dev   Pointer to initialised handle.
 * @retval MMA8452Q_OK / MMA8452Q_ERROR.
 */
MMA8452Q_Status_t MMA8452Q_Standby(MMA8452Q_Handle_t *dev);

/**
 * @brief  Wake the device from standby back to active mode.
 * @param  dev   Pointer to initialised handle.
 * @retval MMA8452Q_OK / MMA8452Q_ERROR.
 */
MMA8452Q_Status_t MMA8452Q_Active(MMA8452Q_Handle_t *dev);

/**
 * @brief  Perform a software reset of the device.
 * @param  dev   Pointer to initialised handle.
 * @retval MMA8452Q_OK / MMA8452Q_ERROR.
 */
MMA8452Q_Status_t MMA8452Q_Reset(MMA8452Q_Handle_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MMA8452Q_H */
