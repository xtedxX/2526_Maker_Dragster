/**
 * @file    mma8452q.c
 * @brief   Driver for MMA8452Q 3-axis accelerometer — X-axis only (Dragster project)
 * @target  STM32G431CBU6 — I2C1 (SCL: PA15 / SDA: PB7)
 *
 * Wiring reminder:
 *   VCC  → 3.3 V
 *   GND  → GND
 *   SCL  → PA15  (I2C1_SCL)
 *   SDA  → PB7   (I2C1_SDA)
 *   INT1 → PC13  (GPIO_Input, label INT_1)
 *   INT2 → PB9   (GPIO_Input, label INT_2)
 *   SA0  → GND   → I2C address 0x1C  (change MMA8452Q_I2C_ADDR if tied to VCC)
 */

#include "mma8452q.h"

/* -------------------------------------------------------------------------
 * I2C timeout (ms)
 * ------------------------------------------------------------------------- */
#define I2C_TIMEOUT_MS   10U

/* -------------------------------------------------------------------------
 * STATUS register bit
 * ------------------------------------------------------------------------- */
#define STATUS_XYZDR     0x08   /*!< Any-axis data-ready flag */
#define STATUS_XDR       0x01   /*!< X-axis data-ready flag   */

/* -------------------------------------------------------------------------
 * CTRL_REG2 — software reset bit
 * ------------------------------------------------------------------------- */
#define CTRL_REG2_RST    0x40

/* -------------------------------------------------------------------------
 * Private helpers
 * ------------------------------------------------------------------------- */

/**
 * @brief Write a single byte to a register.
 */
static MMA8452Q_Status_t prv_WriteReg(MMA8452Q_Handle_t *dev,
                                       uint8_t reg,
                                       uint8_t value)
{
    uint8_t buf[2] = { reg, value };
    if (HAL_I2C_Master_Transmit(dev->hi2c,
                                dev->i2c_addr,
                                buf, 2,
                                I2C_TIMEOUT_MS) != HAL_OK)
    {
        return MMA8452Q_ERROR;
    }
    return MMA8452Q_OK;
}

/**
 * @brief Read one or more bytes starting at a register.
 */
static MMA8452Q_Status_t prv_ReadRegs(MMA8452Q_Handle_t *dev,
                                       uint8_t reg,
                                       uint8_t *data,
                                       uint16_t len)
{
    if (HAL_I2C_Mem_Read(dev->hi2c,
                         dev->i2c_addr,
                         reg,
                         I2C_MEMADD_SIZE_8BIT,
                         data, len,
                         10U) != HAL_OK)
    {
        return MMA8452Q_ERROR;
    }
    return MMA8452Q_OK;
}

/**
 * @brief Compute the sensitivity factor (g per LSB) from the chosen range.
 *        The MMA8452Q outputs 12-bit signed values (right-aligned after shift).
 *        Full scale counts = 2^11 = 2048 for one side.
 */
static float prv_ComputeSensitivity(MMA8452Q_Range_t range)
{
    switch (range)
    {
        case MMA8452Q_RANGE_4G: return 4.0f  / 2048.0f;
        case MMA8452Q_RANGE_8G: return 8.0f  / 2048.0f;
        case MMA8452Q_RANGE_2G:
        default:                return 2.0f  / 2048.0f;
    }
}

/* -------------------------------------------------------------------------
 * Public API implementation
 * ------------------------------------------------------------------------- */

MMA8452Q_Status_t MMA8452Q_Init(MMA8452Q_Handle_t *dev)
{
    uint8_t who_am_i = 0;

    /* --- 1. Verify device identity ---------------------------------------- */
    if (prv_ReadRegs(dev, MMA8452Q_REG_WHO_AM_I, &who_am_i, 1) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }
    if (who_am_i != MMA8452Q_WHO_AM_I_VALUE)
    {
        return MMA8452Q_ERROR;   /* Wrong device or wiring issue */
    }

    /* --- 2. Software reset ------------------------------------------------- */
    if (MMA8452Q_Reset(dev) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    /* --- 3. Go to standby before changing configuration ------------------- */
    if (MMA8452Q_Standby(dev) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    /* --- 4. Set full-scale range ------------------------------------------ */
    /*
     * XYZ_DATA_CFG[1:0] → FS[1:0]
     * No high-pass filter on output data (HPF_OUT = 0).
     */
    if (prv_WriteReg(dev, MMA8452Q_REG_XYZ_DATA_CFG,
                     (uint8_t)dev->range) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    /* --- 5. Set output data rate & low-noise mode ------------------------- */
    /*
     * CTRL_REG1:
     *   LNOISE (bit 2) = 1 → low-noise mode (not available with ±8 g range)
     *   DR[2:0] (bits [5:3]) = selected ODR
     *   ACTIVE (bit 0) = 0 → remain in standby for now
     *
     * For a dragster (high-g transient event) 800 Hz or 400 Hz is recommended.
     */
    uint8_t ctrl1 = (uint8_t)dev->odr;
    if (dev->range != MMA8452Q_RANGE_8G)
    {
        ctrl1 |= 0x04;  /* LNOISE bit — disabled automatically at ±8 g */
    }
    if (prv_WriteReg(dev, MMA8452Q_REG_CTRL_REG1, ctrl1) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    /* --- 6. Store sensitivity for g conversion --------------------------- */
    dev->sensitivity = prv_ComputeSensitivity(dev->range);

    /* --- 7. Activate ------------------------------------------------------- */
    if (MMA8452Q_Active(dev) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    return MMA8452Q_OK;
}

/* -------------------------------------------------------------------------- */

MMA8452Q_Status_t MMA8452Q_ReadRawX(MMA8452Q_Handle_t *dev, int16_t *raw_x)
{
    uint8_t buf[2];

    /* Read OUT_X_MSB and OUT_X_LSB in a single 2-byte burst */
    if (prv_ReadRegs(dev, MMA8452Q_REG_OUT_X_MSB, buf, 2) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    /*
     * The MMA8452Q delivers 12-bit data left-justified in the 16-bit word:
     *   MSB[7:0] = bits [11:4] of the measurement
     *   LSB[7:4] = bits [3:0]  of the measurement
     *   LSB[3:0] = 0 (unused)
     *
     * Reconstruct the signed 12-bit value:
     *   1. Combine bytes into a 16-bit value  (MSB << 8 | LSB)
     *   2. Arithmetic right-shift by 4 to get 12-bit signed integer
     */
    int16_t raw = (int16_t)((buf[0] << 8) | buf[1]);
    raw >>= 4;      /* Sign-extending arithmetic shift */

    *raw_x = raw;
    return MMA8452Q_OK;
}

/* -------------------------------------------------------------------------- */

MMA8452Q_Status_t MMA8452Q_ReadAccelX(MMA8452Q_Handle_t *dev, float *accel_g)
{
    int16_t raw = 0;

    if (MMA8452Q_ReadRawX(dev, &raw) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    *accel_g = (float)raw * dev->sensitivity;
    return MMA8452Q_OK;
}

/* -------------------------------------------------------------------------- */

bool MMA8452Q_IsDataReady(MMA8452Q_Handle_t *dev)
{
    uint8_t status = 0;

    if (prv_ReadRegs(dev, MMA8452Q_REG_STATUS, &status, 1) != MMA8452Q_OK)
    {
        return false;
    }

    /* XDR bit (bit 0) — X-axis new data available */
    return (status & STATUS_XDR) != 0;
}

/* -------------------------------------------------------------------------- */

MMA8452Q_Status_t MMA8452Q_Standby(MMA8452Q_Handle_t *dev)
{
    uint8_t ctrl1 = 0;

    if (prv_ReadRegs(dev, MMA8452Q_REG_CTRL_REG1, &ctrl1, 1) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    ctrl1 &= ~0x01U;   /* Clear ACTIVE bit */

    return prv_WriteReg(dev, MMA8452Q_REG_CTRL_REG1, ctrl1);
}

/* -------------------------------------------------------------------------- */

MMA8452Q_Status_t MMA8452Q_Active(MMA8452Q_Handle_t *dev)
{
    uint8_t ctrl1 = 0;

    if (prv_ReadRegs(dev, MMA8452Q_REG_CTRL_REG1, &ctrl1, 1) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    ctrl1 |= 0x01U;    /* Set ACTIVE bit */

    return prv_WriteReg(dev, MMA8452Q_REG_CTRL_REG1, ctrl1);
}

/* -------------------------------------------------------------------------- */

MMA8452Q_Status_t MMA8452Q_Reset(MMA8452Q_Handle_t *dev)
{
    if (prv_WriteReg(dev, MMA8452Q_REG_CTRL_REG2, CTRL_REG2_RST) != MMA8452Q_OK)
    {
        return MMA8452Q_ERROR;
    }

    HAL_Delay(10);   /* was 2 — increase to 10 ms to be safe */

    return MMA8452Q_OK;
}

/* -------------------------------------------------------------------------
 * Usage example (paste into main.c)
 * -------------------------------------------------------------------------
 *
 * #include "mma8452q.h"
 *
 * extern I2C_HandleTypeDef hi2c1;
 *
 * MMA8452Q_Handle_t accel = {
 *     .hi2c     = &hi2c1,
 *     .i2c_addr = MMA8452Q_I2C_ADDR,       // 0x1C<<1 — SA0 tied to GND
 *     .range    = MMA8452Q_RANGE_8G,        // ±8 g suits dragster launches
 *     .odr      = MMA8452Q_ODR_800HZ        // 800 Hz for fast transients
 * };
 *
 * int main(void)
 * {
 *     HAL_Init();
 *     SystemClock_Config();
 *     MX_GPIO_Init();
 *     MX_I2C1_Init();
 *
 *     if (MMA8452Q_Init(&accel) != MMA8452Q_OK)
 *     {
 *         // Handle init failure (check wiring / I2C address)
 *         Error_Handler();
 *     }
 *
 *     float accel_g = 0.0f;
 *
 *     while (1)
 *     {
 *         if (MMA8452Q_IsDataReady(&accel))
 *         {
 *             MMA8452Q_ReadAccelX(&accel, &accel_g);
 *             // accel_g now holds longitudinal acceleration in g
 *             // e.g. 0.0 g = at rest, +3.5 g = hard launch
 *         }
 *     }
 * }
 * ------------------------------------------------------------------------- */
