/**
 * @file    tc1602a_lcd.h
 * @brief   TC1602A 16x2 LCD driver — 4-bit mode — Dragster speed display
 * @target  STM32G431CBU6
 *
 * Hardware mapping (from DRAGSTER_Cube_MX.ioc):
 *   RS  → PA8   (GPIO_Output, label: RS)
 *   E   → PC6   (GPIO_Output, label: E)
 *   DB4 → PB15  (GPIO_Output, label: DB4)
 *   DB5 → PB14  (GPIO_Output, label: DB5)
 *   DB6 → PB13  (GPIO_Output, label: DB6)
 *   DB7 → PB12  (GPIO_Output, label: DB7)
 *   DB0-DB3 → GND  (not used in 4-bit mode)
 *   RW  → GND       (write-only)
 *   VSS → GND
 *   VDD → 5V
 *   V0  → potentiometer wiper (contrast)
 *   A   → 5V through 100Ω resistor (backlight)
 *   K   → GND
 *
 * Display layout (dragster mode):
 *   Row 0: "Speed: XXX.X km/h"
 *   Row 1: "Peak:  XXX.X km/h"   or run status
 */

#ifndef TC1602A_LCD_H
#define TC1602A_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------
 * Pin definitions — matched to your IOC labels
 * ------------------------------------------------------------------------- */
#define LCD_RS_PORT     GPIOA
#define LCD_RS_PIN      GPIO_PIN_8

#define LCD_E_PORT      GPIOC
#define LCD_E_PIN       GPIO_PIN_6

#define LCD_DB4_PORT    GPIOB
#define LCD_DB4_PIN     GPIO_PIN_15

#define LCD_DB5_PORT    GPIOB
#define LCD_DB5_PIN     GPIO_PIN_14

#define LCD_DB6_PORT    GPIOB
#define LCD_DB6_PIN     GPIO_PIN_13

#define LCD_DB7_PORT    GPIOB
#define LCD_DB7_PIN     GPIO_PIN_12

/* -------------------------------------------------------------------------
 * Display geometry
 * ------------------------------------------------------------------------- */
#define LCD_COLS        16U
#define LCD_ROWS        2U

/* -------------------------------------------------------------------------
 * Custom character indices (stored in CGRAM slots 0–3)
 * ------------------------------------------------------------------------- */
#define LCD_CHAR_KMH    0   /*!< "k" glyph shortcut — slot 0  */
#define LCD_CHAR_ARROW  1   /*!< Right-arrow glyph  — slot 1  */

/* -------------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------------- */

/**
 * @brief  Initialise the TC1602A in 4-bit mode.
 *         Call after MX_GPIO_Init() and a short power-on delay (~50 ms).
 */
void LCD_Init(void);

/**
 * @brief  Clear the display and return cursor home.
 */
void LCD_Clear(void);

/**
 * @brief  Move cursor to a specific position.
 * @param  col  Column index 0–15.
 * @param  row  Row index 0–1.
 */
void LCD_SetCursor(uint8_t col, uint8_t row);

/**
 * @brief  Write a null-terminated string at the current cursor position.
 * @param  str  String to display (truncated to remaining columns).
 */
void LCD_Print(const char *str);

/**
 * @brief  Write a single character at the current cursor position.
 * @param  c  Character to write.
 */
void LCD_WriteChar(char c);

/**
 * @brief  Display current dragster speed on row 0.
 *         Format: "Speed: XXX.X km/h"  (padded/right-aligned)
 * @param  speed_kmh  Speed in km/h (computed from integrated acceleration).
 */
void LCD_ShowSpeed(float speed_kmh);

/**
 * @brief  Display peak speed on row 1.
 *         Format: "Peak:  XXX.X km/h"
 * @param  peak_kmh  Peak speed in km/h.
 */
void LCD_ShowPeak(float peak_kmh);

/**
 * @brief  Display a status message on row 1 (e.g. "Ready", "Run done").
 *         Automatically padded with spaces to clear previous content.
 * @param  msg  Up to 16-character string.
 */
void LCD_ShowStatus(const char *msg);

/**
 * @brief  Display the post-run summary on both rows.
 *         Row 0: "Time: X.XXX s"
 *         Row 1: "Peak: XX.X km/h"
 * @param  elapsed_ms  Run duration in milliseconds.
 * @param  peak_kmh    Peak speed reached.
 */
void LCD_ShowRunResult(uint32_t elapsed_ms, float peak_kmh);

/**
 * @brief  Display a splash screen on power-up.
 *         Row 0: "  DRAGSTER v1.0 "
 *         Row 1: "  Initialising.."
 */
void LCD_Splash(void);

/**
 * @brief  Turn the display on or off (backlight control via E-pin trick
 *         is not available — this uses the display-on/off command).
 * @param  on  true = display on, false = display off.
 */
void LCD_SetDisplay(bool on);

void LCD_IntegrateSpeed(float accel_g, float dt_s);

void LCD_ResetSpeed(void);

#ifdef __cplusplus
}
#endif

#endif /* TC1602A_LCD_H */
