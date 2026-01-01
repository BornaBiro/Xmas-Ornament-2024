/*
 *  @brief      Source file of the STM32 Library for creating simple menu with one button.
 *              The library supports multiple clicks (taps), regular press and long press.
 *              All timings are configurable.
 *
 *              It uses GPIO interrupts and HAL Sys Tick for timing at millisecond base.
 *
 *  @file       OneButton.cpp
 *  @author     Borna Biro (https://github.com/BornaBiro)
 *  @version    1.0.0
 *  @date       28.12.2025.
 *  @license    MIT
 */

// Include library main header file.
#include "oneButton.h"

OneButton::OneButton(uint16_t _multipleClickMinimalTime, uint16_t _multipleClickMaximalTime, uint16_t _multipleClickDecisionTime, uint16_t _longPressTime)
{
    // Save the timings locally.
    this->_longPressDetection = _longPressTime;
    this->_multipleClickMinTime = _multipleClickMinimalTime;
    this->_multipleClickMaxTime = _multipleClickMaximalTime;
    this->_multipleClickPause = _multipleClickDecisionTime;

    // Clear all arrays.
    memset(this->_timestampsHigh, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
    memset(this->_timestampsLow, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
}

OneButton::~OneButton()
{
    // Empty for now...
}

void OneButton::updateState(uint32_t _timestamp, uint8_t _state)
{
    // Get the proper buffer position.
    int _currentBufferPosition = this->getCircularBufferIndex(0, _state);

    // Check if the state is high or low.
    if (_state == 0)
    {
        this->_timestampsLow[_currentBufferPosition] =_timestamp;
        // Increment current buffer position.
        _circleBufferIndexLow = (_circleBufferIndexLow + 1) % ONEBUTTON_BUFFER_DEPTH;
    }
    else
    {
        this->_timestampsHigh[_currentBufferPosition] =_timestamp;
        _circleBufferIndexHigh = (_circleBufferIndexHigh + 1) % ONEBUTTON_BUFFER_DEPTH;
    }
}

void OneButton::loop()
{
    // Variable to count how many taps are detected.
    uint8_t _taps = 0;

    // Get the latest press and release time.
    uint32_t _latestPressTimestamp = this->_timestampsLow[this->getCircularBufferIndex(ONEBUTTON_BUFFER_DEPTH - 1, 0)];
    uint32_t _lastestReleaseTimestep = this->_timestampsHigh[this->getCircularBufferIndex(ONEBUTTON_BUFFER_DEPTH - 1, 1)];

    // If no valid timestamps, return!
    if (_latestPressTimestamp == 0) return;

    // Get the how much time is passed from the last button tap.
    uint32_t _lastButtonTapPassed = (uint32_t)(HAL_GetTick() - _latestPressTimestamp);

    // Get the how much time passed from the last button release.
    uint32_t _lastButtonRelease = (uint32_t)(HAL_GetTick() - _lastestReleaseTimestep);

    // Check for the long press. And this one is kinda weird. Ok, the button needs to be pressed for some long amount of time. But
    // what about the release? The key is that release time must be longer then last pressed time - long press, other wise,
    // the button is released before the long press time.
    if ((_lastButtonTapPassed >= this->_longPressDetection) && (_lastButtonRelease > _lastButtonTapPassed))
    {
        btnLongPressed = true;
        memset(this->_timestampsHigh, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
        memset(this->_timestampsLow, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
        return;
    }

    // Now check regular button press. Must be longer than multiple click but less than long press.
    // This also has a gotcha! Time to detect "normal" button press must be something in between multiple button presses
    // and long press. But here is the kicker! Time must be half of the time for the maximal time to detech multiple press. The reason
    // is on multiple press we are targeting two falling edges, but now we need one falling edge and one rising and that is haft of the time
    // of the multiple presses detection! This took me a while to figure it out!
    if ((_latestPressTimestamp != 0) && (_lastestReleaseTimestep != 0) && ((_lastestReleaseTimestep - _latestPressTimestamp) > (this->_multipleClickMaxTime / 2)) && ((_lastestReleaseTimestep - _latestPressTimestamp) < this->_longPressDetection))
    {
        btnPressed = true;
        memset(this->_timestampsHigh, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
        memset(this->_timestampsLow, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
        return;
    }

    // Check the last button tap time. Only process them if they are valid (not a startup) and if it's passed some time after the last tap.
    if (_lastButtonTapPassed >= this->_multipleClickPause)
    {
        // Check how much taps are there.
        for (int i = ONEBUTTON_BUFFER_DEPTH - 1; i >= 1; i--)
        {
            // Check the timestamps.
            uint32_t _timestampDifference = this->_timestampsLow[this->getCircularBufferIndex(i, 0)] - this->_timestampsLow[this->getCircularBufferIndex(i - 1, 0)];
            if ((_timestampDifference >= this->_multipleClickMinTime) && (_timestampDifference <= this->_multipleClickMaxTime))
            {
                // It's a valid tap -> increase the taps.
                _taps++;
            }
        }
        // Check the taps. If the taps are detected, save them and clear the buffers.
        if (_taps != 0)
        {
            _detectedTaps = _taps;
            memset(this->_timestampsHigh, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
            memset(this->_timestampsLow, 0, ONEBUTTON_BUFFER_DEPTH * sizeof(uint32_t));
            return;
        }
    }
}

bool OneButton::isPressed()
{
    // As soon as the method is called, clear the state of the button.
    // Store the state in the temporary variable.
    bool _temp = btnPressed;

    // Clear the flag.
    btnPressed = false;

    // Return the stored value.
    return _temp;
}

bool OneButton::isMultipleTap()
{
    // Check if there is multiple clicks detected. If so, return true, otherwise return false.
    return _detectedTaps ? true : false;
}

uint8_t OneButton::getNumberOfTaps()
{
    uint8_t _temp = _detectedTaps;
    _detectedTaps = 0;
    return _temp;
}

bool OneButton::isLongPressed()
{
    bool _temp = btnLongPressed;
    btnLongPressed = false;
    return _temp;
}

uint8_t OneButton::getCircularBufferIndex(int8_t _i, bool _state)
{
    // Circular buffer for the timestamps of the pin state change. There are
    // two buffers - one that stores timestamps of the rising edges, other one
    // falling edges.

    // The idea behind this was to be able to set the index (_i variable) to be negative. Kinda worked...
    if (_i < 0) _i--;

    // Return the index of the buffer for the rising edge or falling edge timestamps.
    return _state?(_circleBufferIndexHigh + (uint8_t)(_i)) % ONEBUTTON_BUFFER_DEPTH:(_circleBufferIndexLow + (uint8_t)(_i)) % ONEBUTTON_BUFFER_DEPTH;
}
