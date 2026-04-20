/**
 * @file    tc1602a_lcd.c
 * @brief   TC1602A 16x2 LCD driver — 4-bit mode — HAL_Delay timing
 * @target  STM32G431CBU6 @ 85 MHz
 */

#include "tc1602a_lcd.h"
#include <string.h>
#include <stdio.h>

#define LCD_CMD_CLEAR        0x01
#define LCD_CMD_HOME         0x02
#define LCD_CMD_ENTRY_MODE   0x06
#define LCD_CMD_DISPLAY_ON   0x0C
#define LCD_CMD_DISPLAY_OFF  0x08
#define LCD_CMD_FUNC_4BIT    0x28
#define LCD_CMD_CGRAM_ADDR   0x40
#define LCD_CMD_DDRAM_ROW0   0x80
#define LCD_CMD_DDRAM_ROW1   0xC0

static inline void prv_Pin(GPIO_TypeDef *port, uint16_t pin, uint8_t state)
{
    HAL_GPIO_WritePin(port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void prv_Pulse(void)
{
    prv_Pin(LCD_E_PORT, LCD_E_PIN, 1);
    HAL_Delay(2);
    prv_Pin(LCD_E_PORT, LCD_E_PIN, 0);
    HAL_Delay(2);
}

static void prv_WriteNibble(uint8_t nibble)
{
    prv_Pin(LCD_DB4_PORT, LCD_DB4_PIN, (nibble >> 0) & 1U);
    prv_Pin(LCD_DB5_PORT, LCD_DB5_PIN, (nibble >> 1) & 1U);
    prv_Pin(LCD_DB6_PORT, LCD_DB6_PIN, (nibble >> 2) & 1U);
    prv_Pin(LCD_DB7_PORT, LCD_DB7_PIN, (nibble >> 3) & 1U);
    prv_Pulse();
}

static void prv_WriteByte(uint8_t byte, uint8_t rs)
{
    prv_Pin(LCD_RS_PORT, LCD_RS_PIN, rs);
    prv_WriteNibble((byte >> 4) & 0x0F);
    prv_WriteNibble( byte       & 0x0F);
    HAL_Delay(2);
}

static void prv_Cmd(uint8_t cmd)   { prv_WriteByte(cmd, 0); }
static void prv_Data(uint8_t data) { prv_WriteByte(data, 1); }

static void prv_LoadCustomChars(void)
{
    static const uint8_t char_kmh[8] = {
        0b10001, 0b10010, 0b11100, 0b10010,
        0b10001, 0b00000, 0b00000, 0b00000
    };
    static const uint8_t char_arrow[8] = {
        0b00000, 0b00100, 0b00110, 0b11111,
        0b00110, 0b00100, 0b00000, 0b00000
    };
    prv_Cmd(LCD_CMD_CGRAM_ADDR | (LCD_CHAR_KMH << 3));
    for (uint8_t i = 0; i < 8; i++) { prv_Data(char_kmh[i]); }
    prv_Cmd(LCD_CMD_CGRAM_ADDR | (LCD_CHAR_ARROW << 3));
    for (uint8_t i = 0; i < 8; i++) { prv_Data(char_arrow[i]); }
    prv_Cmd(LCD_CMD_DDRAM_ROW0);
}

void LCD_Init(void)
{
    HAL_Delay(100);
    prv_Pin(LCD_E_PORT,  LCD_E_PIN,  0);
    prv_Pin(LCD_RS_PORT, LCD_RS_PIN, 0);

    /* HD44780 init: 3x 0x3 nibble */
    prv_Pin(LCD_DB4_PORT, LCD_DB4_PIN, 1);
    prv_Pin(LCD_DB5_PORT, LCD_DB5_PIN, 1);
    prv_Pin(LCD_DB6_PORT, LCD_DB6_PIN, 0);
    prv_Pin(LCD_DB7_PORT, LCD_DB7_PIN, 0);
    prv_Pulse(); HAL_Delay(10);
    prv_Pulse(); HAL_Delay(5);
    prv_Pulse(); HAL_Delay(2);

    /* Switch to 4-bit */
    prv_Pin(LCD_DB4_PORT, LCD_DB4_PIN, 0);
    prv_Pin(LCD_DB5_PORT, LCD_DB5_PIN, 1);
    prv_Pin(LCD_DB6_PORT, LCD_DB6_PIN, 0);
    prv_Pin(LCD_DB7_PORT, LCD_DB7_PIN, 0);
    prv_Pulse(); HAL_Delay(2);

    prv_Cmd(LCD_CMD_FUNC_4BIT);
    prv_Cmd(LCD_CMD_DISPLAY_OFF);
    prv_Cmd(LCD_CMD_CLEAR);    HAL_Delay(5);
    prv_Cmd(LCD_CMD_ENTRY_MODE);
    prv_Cmd(LCD_CMD_DISPLAY_ON);
    prv_LoadCustomChars();
}

void LCD_Clear(void)
{
    prv_Cmd(LCD_CMD_CLEAR);
    HAL_Delay(5);
}

void LCD_SetCursor(uint8_t col, uint8_t row)
{
    uint8_t addr = (row == 0) ? LCD_CMD_DDRAM_ROW0 : LCD_CMD_DDRAM_ROW1;
    addr += (col & 0x0FU);
    prv_Cmd(addr);
}

void LCD_WriteChar(char c)  { prv_Data((uint8_t)c); }

void LCD_Print(const char *str)
{
    while (*str) { prv_Data((uint8_t)*str++); }
}

void LCD_SetDisplay(bool on)
{
    prv_Cmd(on ? LCD_CMD_DISPLAY_ON : LCD_CMD_DISPLAY_OFF);
}

void LCD_ShowSpeed(float speed_kmh)
{
    char buf[18];
    if (speed_kmh < 0.0f) speed_kmh = 0.0f;
    snprintf(buf, sizeof(buf), "Speed:%6.1f km/h", speed_kmh);
    LCD_SetCursor(0, 0);
    LCD_Print(buf);
}

void LCD_ShowPeak(float peak_kmh)
{
    char buf[18];
    if (peak_kmh < 0.0f) peak_kmh = 0.0f;
    snprintf(buf, sizeof(buf), "Peak: %6.1f km/h", peak_kmh);
    LCD_SetCursor(0, 1);
    LCD_Print(buf);
}

void LCD_ShowStatus(const char *msg)
{
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", msg);
    LCD_SetCursor(0, 1);
    LCD_Print(buf);
}

void LCD_ShowRunResult(uint32_t elapsed_ms, float peak_kmh)
{
    char buf[19];
    float t_s = (float)elapsed_ms / 1000.0f;
    snprintf(buf, sizeof(buf), "Time:%8.3f s   ", t_s);
    LCD_SetCursor(0, 0);
    LCD_Print(buf);
    snprintf(buf, sizeof(buf), "Peak: %6.1f km/h", peak_kmh < 0.0f ? 0.0f : peak_kmh);
    LCD_SetCursor(0, 1);
    LCD_Print(buf);
}

void LCD_Splash(void)
{
    LCD_Init();
    LCD_SetCursor(0, 0);
    LCD_Print("  DRAGSTER v1.0 ");
    LCD_SetCursor(0, 1);
    LCD_Print("  Initialising..");
}

static float s_speed_ms   = 0.0f;
static float s_peak_kmh   = 0.0f;
static float s_last_accel = 0.0f;

void LCD_IntegrateSpeed(float accel_g, float dt_s)
{
    float a_ms2 = ((s_last_accel + accel_g) * 0.5f) * 9.81f;
    s_last_accel = accel_g;
    s_speed_ms  += a_ms2 * dt_s;
    if (s_speed_ms < 0.0f) s_speed_ms = 0.0f;
    float speed_kmh = s_speed_ms * 3.6f;
    if (speed_kmh > s_peak_kmh) s_peak_kmh = speed_kmh;
    LCD_ShowSpeed(speed_kmh);
    LCD_ShowPeak(s_peak_kmh);
}

void LCD_ResetSpeed(void)
{
    s_speed_ms   = 0.0f;
    s_peak_kmh   = 0.0f;
    s_last_accel = 0.0f;
    LCD_ShowSpeed(0.0f);
    LCD_ShowPeak(0.0f);
}
