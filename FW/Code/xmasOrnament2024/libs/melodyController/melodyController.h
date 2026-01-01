/*
 *  @brief      Header file of the melory controller aka melody player.
 *              It controls and uses extracted melodies.
 *              Uses my custom made Buzzer library for tone generation.
 *
 *  @file       melodyController.h
 *  @author     Borna Biro (https://github.com/BornaBiro)
 *  @version    1.0.0
 *  @date       28.12.2025.
 *  @license    MIT
 */

// Add a header guard to prevent multiple inclusions.
#ifndef __MELODY_CONTROLLER_H__
#define __MELODY_CONTROLLER_H__

// Add main STM32 HAL library header file.
#include "stm32c0xx_hal.h"

// Include custom tone library header file.
#include "Buzzer.h"

// Include standard C libraries.
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

class MelodyController
{
    public:
        MelodyController();
        void begin(uint16_t **_notes, uint16_t **_durations, uint8_t *_elements, uint8_t _noSongs);
        void play();
        uint8_t update();
        void next();
        void prev();
        void stop();
        void setTempoOffset(int8_t _offset);
        int8_t getTempoOffset();
        void setMute(uint8_t _mute);
        uint8_t getMute();
        uint8_t isPlaying();

    private:
        uint8_t _currentSongElement = 0;
        uint8_t _playing = 0;
        int8_t _currentSong = 0;
        uint8_t _maxElements = 0;
        unsigned long _noteTimer = 0;
        uint8_t _noteState = 0;
        int8_t _tempoOffset = 0;
        uint16_t _currentNoteDuration = 0;

        // Disable mute by default.
        uint8_t _muteStatus = 0;

        uint16_t **_notesInternalArray = NULL;
        uint16_t **_durationsInternalArray = NULL;
        uint8_t *_elementsInternalArray = NULL;
        uint8_t _numberOfSongs = 0;


};

#endif
