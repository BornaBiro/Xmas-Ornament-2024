/*
 * 	@brief 		Source file for the buzzer library for the STM32C011 series MCU.
 * 				It will use HW timer and it's PWM pin for the sound generation.
 * 				Can generate sound frequencies from 10Hz up to 20kHz. It automatically
 * 				adjusts timer parameters to the CPU frequency.
 *
 *  @author		Borna Biro (https://github.com/BornaBiro)
 *  @version	1.0.0
 *  @date		5.1.2025.
 *  @license	MIT
 */

// Include main library header file.
#include "Buzzer.h"

// Local parameters.
static TIM_HandleTypeDef *_htim = NULL;
static bool _isInitialized = false;

bool buzzer_begin(TIM_HandleTypeDef *_htimPtr)
{
	// Check the parameters.
	if ((_htimPtr == NULL)) return false;

	// Copy parameter locally.
	_htim = _htimPtr;

	// Set the flag.
	_isInitialized = true;

	// Return true for success.
	return true;
}

void buzzer_tone(uint32_t _freq, uint32_t _channel)
{
	// Check for initialization.
	if (!_isInitialized) return;

	// Check for the parameters.
	if ((_freq < 10ULL) || (_freq > 20000ULL)) return;

	// Calculate the prescaler.
	uint32_t _prescaler = (HAL_RCC_GetSysClockFreq() / (50 * _freq)) - 1;

	// Check if the prescaler is not overflowed.
	if (_prescaler > 65535) _prescaler = 65535;

	// Now calculate the period.
	uint32_t _period = ((HAL_RCC_GetSysClockFreq() / (_prescaler)) / _freq) - 1;

	// Set duty cycle to 50%.
	uint32_t _compare = (uint32_t)(_period * 0.5);

	// Get the Compare register addreess.
	uint32_t *_ccr = buzzer_getCCReg(_channel);

	// Set the duty to 0%.
	*(_ccr) = 0;

	// Stop the timer.
	HAL_TIM_PWM_Stop(_htim, _channel);

	// Set new prescaler value.
	__HAL_TIM_SET_PRESCALER(_htim, _prescaler);

	// Set the new auto preload value.
	__HAL_TIM_SET_AUTORELOAD(_htim, _period);

	// Set new compare value.
	*(_ccr) = _compare;

	// Start the PWM.
	HAL_TIM_PWM_Start(_htim, _channel);
}

void buzzer_toneDuration(uint32_t _freq, uint32_t _durationMs, uint32_t _channel)
{
	// Check for initialization.
	if (!_isInitialized) return;

	// Check for the parameters.
	if ((_freq < 10ULL) || (_freq > 20000ULL)) return;

	// Generate the tone.
	buzzer_tone(_freq, _channel);

	// Wait defined amount of time.
	HAL_Delay(_durationMs);

	// Stop the PWM.
	HAL_TIM_PWM_Stop(_htim, _channel);
}

void buzzer_noTone(uint32_t _channel)
{
	// Just stop the PWM generation on this specific channel.ž
	HAL_TIMEx_PWMN_Stop(_htim, _channel);
}

static uint32_t* buzzer_getCCReg(uint32_t _timerCh)
{
	switch (_timerCh)
	{
		case TIM_CHANNEL_1:
			return (uint32_t*)(&(_htim->Instance->CCR1));
			break;
		case TIM_CHANNEL_2:
			return (uint32_t*)(&(_htim->Instance->CCR2));
			break;
		case TIM_CHANNEL_3:
			return (uint32_t*)(&(_htim->Instance->CCR3));
			break;
		case TIM_CHANNEL_4:
			return (uint32_t*)(&(_htim->Instance->CCR4));
			break;
	}

	// Failsafe.
	return 0;
}
