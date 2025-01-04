/*
 * 	@brief 		Source file of the STM32 Library for the WS2812B LEDs for STM32C011 MCU.
 *  			Library uses Hardware timer and DMA to send data to LEDs.
 *  			It must use O2 Compiler Optimization and Core clock must be >= 24MHz!
 *
 *  @author		Borna Biro (https://github.com/BornaBiro
 *  @version	1.0.0
 *  @date		30.12.2024.
 *  @license	MIT
 */

// Include main header file of the library.
#include <WS2812.h>

// Include all other standard C libraries.
#include <stdlib.h>
#include <string.h>

// Flag for finished DMA transfer.
static volatile uint8_t dmaTimerTransfetCompleteFlag = 0;

// Temp. array for the DMA timer compare values for PWM generation (for WS LEDs).
static volatile uint16_t _ledPwmValues[28];

// Buffer for the LEDs.
static uint32_t *_ledBuffer = NULL;

// Number of LEDs.
static uint16_t _numberOfLeds = 0;

// Pointers to the typedefs for the timer and DMA.
static TIM_HandleTypeDef *_htim = NULL;
static DMA_HandleTypeDef *_hdma = NULL;

// PWM compare values. They will be calculated in the begin code.
static uint16_t _shortTiming = 0;
static uint16_t _longTiming = 0;
static uint16_t _counterValue = 0;
static uint16_t _prescaler = 0;

// Timer channel.
uint32_t _timerChannel =0 ;

bool ws2812_begin(TIM_HandleTypeDef *_htimPtr, DMA_HandleTypeDef *_hdmaPtr, uint32_t _channel, uint16_t _numOfLeds)
{
	// Check for the arguments.
	if ((_htimPtr == NULL) || (_hdmaPtr == NULL) || (_numOfLeds == 0)) return false;

	// Allocate the memory for the LED buffer. Return if failed.
	_ledBuffer = (uint32_t*)malloc(_numOfLeds * sizeof(uint32_t));
	if (_ledBuffer == NULL) return false;

	// Clean the array.
	memset((uint32_t*)_ledBuffer, 0, sizeof(uint32_t) * _numOfLeds);

	// Clear the PWM timing array.
	memset((uint16_t*)_ledPwmValues, 0, sizeof(_ledPwmValues));

	// Copy all other parameters internally.
	_numberOfLeds = _numOfLeds;
	_htim = _htimPtr;
	_hdma = _hdmaPtr;
	_timerChannel = _channel;

	// Try to calculate the compare values based on core clock.
	// To Do - since these are calculated manually.
	_shortTiming = 3;
	_longTiming = 6;
	_counterValue = 9;
	_prescaler = 2;

	// Load these values into the timer.
	__HAL_TIM_SET_PRESCALER(_htim, _prescaler);
	__HAL_TIM_SET_AUTORELOAD(_htim, _counterValue);

	// Set the timer and DMA.
	HAL_DMA_RegisterCallback(_hdma, HAL_DMA_XFER_CPLT_CB_ID, timerDmaTransferComplete);
	HAL_TIM_PWM_Start(_htim, _timerChannel);
	__HAL_TIM_ENABLE_DMA(_htim, ws2812_dmaChannel(_timerChannel));

	// If everything went ok, return true.
	return true;
}

void ws2812_setPixelColor(uint32_t _led, uint32_t _color)
{
	// Check for the input parameter.
	if (_led >= _numberOfLeds) return;

	// Modify the color of the LEDs.
	_ledBuffer[_led] = _color;
}

void ws2812_show()
{
	for (int i = 0; i < _numberOfLeds; i++)
	{
		ws2812_convertToPwm(_ledBuffer[i], _ledPwmValues);
		__HAL_TIM_SET_COUNTER(_htim, 0);
		__HAL_TIM_ENABLE_DMA(_htim, _timerChannel);
		HAL_DMA_Start_IT(_hdma, (uint32_t)_ledPwmValues, (uint32_t)ws2812_getCCReg(_timerChannel), sizeof(_ledPwmValues) / sizeof(uint16_t));
		__HAL_TIM_DISABLE_DMA(_htim, _timerChannel);
		while (dmaTimerTransfetCompleteFlag == 0);
		dmaTimerTransfetCompleteFlag = 0;
	}
}

uint32_t ws2812_color(uint8_t _r, uint8_t _g, uint8_t _b)
{

}

uint32_t ws2812_hsv()
{

}

// Private members.
static void ws2812_convertToPwm(uint32_t _rgb, uint16_t *_pwm)
{
	for (int i = 0; i < 24; i++)
	{
		_pwm[23 - i + 2] = _rgb & (1ULL << i)?_longTiming:_shortTiming;
	}
}

static uint32_t* ws2812_getCCReg(uint32_t _timerCh)
{
	switch (_timerCh)
	{
		case TIM_CHANNEL_1:
			return &(_htim->Instance->CCR1);
			break;
		case TIM_CHANNEL_2:
			return &(_htim->Instance->CCR2);
			break;
		case TIM_CHANNEL_3:
			return &(_htim->Instance->CCR3);
			break;
		case TIM_CHANNEL_4:
			return &(_htim->Instance->CCR4);
			break;
	}

	// Failsafe.
	return 0;
}

static uint32_t ws2812_dmaChannel(uint32_t _timerCh)
{
	switch (_timerCh)
	{
		case TIM_CHANNEL_1:
			return TIM_DMA_CC1;
			break;
		case TIM_CHANNEL_2:
			return TIM_DMA_CC2;
			break;
		case TIM_CHANNEL_3:
			return TIM_DMA_CC3;
			break;
		case TIM_CHANNEL_4:
			return TIM_DMA_CC4;
			break;
	}

	// Failsafe.
	return 0;
}

static void timerDmaTransferComplete()
{
	dmaTimerTransfetCompleteFlag = 1;
}

