/*
 *  @brief      Header file of the STM32 Library for creating simple menu with one button.
 *              The library supports multiple clicks (taps), regular press and long press.
 *              All timings are configurable.
 *
 *              It uses GPIO interrupts and HAL Sys Tick for timing at millisecond base.
 *
 *  @file       OneButton.h
 *  @author     Borna Biro (https://github.com/BornaBiro)
 *  @version    1.0.0
 *  @date       28.12.2025.
 *  @license    MIT
 */

// Add a header guard.
#ifndef __ONEBUTTON_H__
#define __ONEBUTTON_H__

// Add main STM32 HAL Library.
#include "stm32c0xx_hal.h"

// Include standard C libraries.
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Define the button presses buffer depth.
#define ONEBUTTON_BUFFER_DEPTH  10

// Library class.
class OneButton
{
  public:
    // Class constructor and destructor.
    OneButton(uint16_t _multipleClickMinimalTime = 5U, uint16_t _multipleClickMaximalTime = 350U, uint16_t _multipleClickDecisionTime = 350U, uint16_t _longPressTime = 1000U);
    ~OneButton();

    // Main method for button state update.
    void updateState(uint32_t _timestamp, uint8_t _state);

    // Main loop for processing the button presses.
    void loop();

    // Getters for checking the button.
    bool isPressed();
    bool isMultipleTap();
    uint8_t getNumberOfTaps();
    bool isLongPressed();

  private:
    // Method for easily getting the correct index for the circular buffer (it also counts roll-over).
    uint8_t getCircularBufferIndex(int8_t _i, bool _state);

    // Timestamps on pin/button change. These timings are provided by the OneButton::updateState() method.
    uint32_t _timestampsHigh[ONEBUTTON_BUFFER_DEPTH] = {0};
    uint32_t _timestampsLow[ONEBUTTON_BUFFER_DEPTH] = {0};

    // Counter for the circular buffer.
    uint8_t _circleBufferIndexHigh = 0;
    uint8_t _circleBufferIndexLow = 0;

    // Variable holds how many multiple clicks are detected.
    uint8_t _detectedTaps = 0;

    // Variables hold duration values for multiple clicks, long press etc.
    // Values are default ones passed inside the constructor, can be overwritten.
    // All durations are in milliseconds.
    /*
     * |
     * |
     * |-----+          +-------
     * |     |          |
     * |     +----------+
     * +------------------------
     *       <---------->
     *    Long Press Duration
     *
     * |
     * |
     * |-----+  +--+  +--+  +--------
     * |     |  |  |  |  |  |
     * |     +--+  +--+  +--+
     * +------------------------------
     *       <-->            <---->
     *     Multiple           Click
     *      Click             pause
     *       time             time
     *
     */
    uint16_t _longPressDetection;
    uint16_t _multipleClickMinTime;
    uint16_t _multipleClickMaxTime;
    // This duration how much time must pass until the last one button press so we start to process the multiple clicks.
    // The best is to use the same value for the max time for multiple clicks. Do not use less than that.
    uint16_t _multipleClickPause;

    // Local variables for button states.
    bool btnPressed = false;
    bool btnLongPressed = false;
};

// Close the header guard.
#endif
