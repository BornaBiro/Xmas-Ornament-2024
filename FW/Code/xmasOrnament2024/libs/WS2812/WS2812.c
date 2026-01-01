/*
 * 	@brief 		Source file of the STM32 Library for the WS2812B LEDs for STM32C011 MCU.
 *  			Library uses Hardware timer and DMA to send data to LEDs.
 *  			It must use O2 Compiler Optimization and Core clock must be >= 24MHz!
 *
 *  @file       WS2812.c
 *  @author		Borna Biro (https://github.com/BornaBiro)
 *  @version	1.0.0
 *  @date		30.12.2024.
 *  @license    MIT except the HSV conversion ws2812_hsv() function, this part is from
 *              Adafruit_NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
 */

// Include main header file of the library.
#include <WS2812.h>

// Include all other standard C libraries.
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Declaration of the static functions.
static void ws2812_convertToPwm(volatile uint32_t _rgb, uint16_t *_pwm);
static uint32_t* ws2812_getCCReg(uint32_t _timerCh);
static uint32_t ws2812_dmaChannel(uint32_t _timerCh);
static void timerDmaTransferComplete();

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
static uint32_t _timerChannel = 0;

bool ws2812_begin(TIM_HandleTypeDef *_htimPtr, DMA_HandleTypeDef *_hdmaPtr, uint32_t _channel, uint16_t _numOfLeds, uint16_t _timPrescaler, uint16_t _timCounter, uint16_t _timLongPulse, uint16_t _timShortPulse)
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
	_shortTiming = _timShortPulse;
	_longTiming = _timLongPulse;
	_counterValue = _timCounter;
	_prescaler = _timPrescaler;

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

	// Split to r, g, b.
	uint8_t _r = (_color >> 16) & 0xff;
	uint8_t _g = (_color >> 8) & 0xff;
	uint8_t _b = _color & 0xff;

	// Modify the color of the LEDs and swap them around (GRB).
	_ledBuffer[_led] = (_g << 16) | (_r << 8) | (_b);
}

void ws2812_show()
{
	for (int i = 0; i < _numberOfLeds; i++)
	{
		ws2812_convertToPwm(_ledBuffer[i], (uint16_t*)_ledPwmValues);
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
	uint32_t _color24;
	_color24 = _r;
	_color24 <<= 8;
	_color24 |= _g;
	_color24 <<= 8;
	_color24 |= _b;
	return _color24;
}

uint32_t ws2812_hsv(uint16_t hue, uint8_t sat, uint8_t val)
{

    uint8_t r, g, b;

    // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
    // 0 is not the start of pure red, but the midpoint...a few values above
    // zero and a few below 65536 all yield pure red (similarly, 32768 is the
    // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
    // each for red, green, blue) really only allows for 1530 distinct hues
    // (not 1536, more on that below), but the full unsigned 16-bit type was
    // chosen for hue so that one's code can easily handle a contiguous color
    // wheel by allowing hue to roll over in either direction.
    hue = (hue * 1530L + 32768) / 65536;
    // Because red is centered on the rollover point (the +32768 above,
    // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
    // where 0 and 1530 would yield the same thing. Rather than apply a
    // costly modulo operator, 1530 is handled as a special case below.

    // So you'd think that the color "hexcone" (the thing that ramps from
    // pure red, to pure yellow, to pure green and so forth back to red,
    // yielding six slices), and with each color component having 256
    // possible values (0-255), might have 1536 possible items (6*256),
    // but in reality there's 1530. This is because the last element in
    // each 256-element slice is equal to the first element of the next
    // slice, and keeping those in there this would create small
    // discontinuities in the color wheel. So the last element of each
    // slice is dropped...we regard only elements 0-254, with item 255
    // being picked up as element 0 of the next slice. Like this:
    // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
    // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
    // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
    // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
    // the constants below are not the multiples of 256 you might expect.

    // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
    if (hue < 510)
    { // Red to Green-1
        b = 0;
        if (hue < 255)
        { //   Red to Yellow-1
            r = 255;
            g = hue; //     g = 0 to 254
        }
        else
        {                  //   Yellow to Green-1
            r = 510 - hue; //     r = 255 to 1
            g = 255;
        }
    }
    else if (hue < 1020)
    { // Green to Blue-1
        r = 0;
        if (hue < 765)
        { //   Green to Cyan-1
            g = 255;
            b = hue - 510; //     b = 0 to 254
        }
        else
        {                   //   Cyan to Blue-1
            g = 1020 - hue; //     g = 255 to 1
            b = 255;
        }
    }
    else if (hue < 1530)
    { // Blue to Red-1
        g = 0;
        if (hue < 1275)
        {                   //   Blue to Magenta-1
            r = hue - 1020; //     r = 0 to 254
            b = 255;
        }
        else
        { //   Magenta to Red-1
            r = 255;
            b = 1530 - hue; //     b = 255 to 1
        }
    }
    else
    { // Last 0.5 Red (quicker than % operator)
        r = 255;
        g = b = 0;
    }

    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat;  // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) | (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
           (((((b * s1) >> 8) + s2) * v1) >> 8);
}

void ws2812_rgb(uint32_t _rgb, uint16_t *h, uint8_t *s, uint8_t *v)
{
    // Extract R, G, and B components from the 24-bit RGB value
    uint8_t r = (_rgb >> 16) & 0xFF;  // Extract R component
    uint8_t g = (_rgb >> 8) & 0xFF;   // Extract G component
    uint8_t b = _rgb & 0xFF;          // Extract B component

    double red = r / 255.0;
    double green = g / 255.0;
    double blue = b / 255.0;

    double cmax = fmax(red, fmax(green, blue));
    double cmin = fmin(red, fmin(green, blue));
    double delta = cmax - cmin;

    double hue = 0, saturation = 0, value = 0;

    // Calculate Hue
    if (delta == 0)
        hue = 0;
    else if (cmax == red)
        hue = 60 * fmod(((green - blue) / delta), 6);
    else if (cmax == green)
        hue = 60 * (((blue - red) / delta) + 2);
    else
        hue = 60 * (((red - green) / delta) + 4);

    if (hue < 0) hue += 360;

    // Calculate Saturation
    if (cmax == 0)
        saturation = 0;
    else
        saturation = (delta / cmax) * 255;

    // Calculate Value
    value = cmax * 255;

    *h = (uint16_t)hue;
    *s = (uint8_t)saturation;
    *v = (uint8_t)value;
}

// Private members.
void ws2812_convertToPwm(volatile uint32_t _rgb, uint16_t *_pwm)
{
	for (int i = 0; i < 24; i++)
	{
		_pwm[23 - i + 2] = _rgb & (1ULL << i)?_longTiming:_shortTiming;
	}
}

uint32_t* ws2812_getCCReg(uint32_t _timerCh)
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

uint32_t ws2812_dmaChannel(uint32_t _timerCh)
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

void timerDmaTransferComplete()
{
	dmaTimerTransfetCompleteFlag = 1;
}

