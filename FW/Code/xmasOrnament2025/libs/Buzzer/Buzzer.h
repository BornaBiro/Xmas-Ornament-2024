/*
 * 	@brief 		Header file for the buzzer library for the STM32C011 series MCU.
 * 				It will use HW timer and it's PWM pin for the sound generation.
 * 				Can generate sound frequencies from 10Hz up to 20kHz. It automatically
 * 				adjusts timer parameters to the CPU frequency.
 *
 *  @file       Buzzer.h
 *  @author		Borna Biro (https://github.com/BornaBiro)
 *  @version	1.0.0
 *  @date		5.1.2025.
 *  @license	MIT
 */
#ifdef __cplusplus
extern "C" {
#endif
// Include main HAL header file.
#include "stm32c0xx_hal.h"

// Include standard C libraries.
#include <stdint.h>
#include <stdbool.h>

bool buzzer_begin(TIM_HandleTypeDef *_htimPtr);
void buzzer_tone(uint32_t _freq, uint32_t _channel);
void buzzer_toneDuration(uint32_t _freq, uint32_t _durationMs, uint32_t _channel);
void buzzer_noTone(uint32_t _channel);
#ifdef __cplusplus
}
#endif
