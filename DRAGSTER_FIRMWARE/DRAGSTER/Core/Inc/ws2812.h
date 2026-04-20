/*
 * ws2812.h
 *
 *  Created on: Dec 8, 2025
 *      Author: jaja
 */

#ifndef INC_WS2812_H_
#define INC_WS2812_H_

#include "main.h"

// Nombre de LEDs sur votre ruban
#define WS2812_NUM_LEDS    1

void WS2812_Init(void);
void WS2812_SetColor(uint8_t r, uint8_t g, uint8_t b);
void WS2812_Refresh(void);

#endif /* INC_WS2812_H_ */
