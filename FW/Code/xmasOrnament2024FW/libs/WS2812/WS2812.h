/*
 * 	@brief 		Header file of the STM32 Library for the WS2812B LEDs for STM32C011 MCU.
 *  			Library uses Hardware timer and DMA to send data to LEDs.
 *  			It must use O2 Compiler Optimization and Core clock must be >= 24MHz!
 *
 *  @author		Borna Biro (https://github.com/BornaBiro)
 *  @version	1.0.0
 *  @date		30.12.2024.
 *  @license	MIT
 */

// Include main HAL header file.
#include "stm32c0xx_hal.h"

// Include standard C libraries.
#include <stdint.h>
#include <stdbool.h>

bool ws2812_begin(TIM_HandleTypeDef *_htimPtr, DMA_HandleTypeDef *_hdmaPtr, uint32_t _channel, uint16_t _numOfLeds, uint16_t _timPrescaler, uint16_t _timCounter, uint16_t _timLongPulse, uint16_t _timShortPulse);
void ws2812_setPixelColor(uint32_t _led, uint32_t _color);
void ws2812_show();
uint32_t ws2812_color(uint8_t _r, uint8_t _g, uint8_t _b);
uint32_t ws2812_hsv(uint16_t hue, uint8_t sat, uint8_t val);
void ws2812_rgb(uint32_t _rgb, uint16_t *h, uint8_t *s, uint8_t *v);

// Private members.
static void ws2812_convertToPwm(volatile uint32_t _rgb, uint16_t *_pwm);
static uint32_t* ws2812_getCCReg(uint32_t _timerCh);
static uint32_t ws2812_dmaChannel(uint32_t _timerCh);
static void timerDmaTransferComplete();
