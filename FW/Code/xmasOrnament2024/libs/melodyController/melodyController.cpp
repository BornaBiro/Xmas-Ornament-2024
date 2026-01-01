/*
 *  @brief      Source file of the melory controller aka melody player.
 *              It controls and uses extracted melodies.
 *              Uses my custom made Buzzer library for tone generation.
 *
 *  @file       melodyController.cpp
 *  @author     Borna Biro (https://github.com/BornaBiro)
 *  @version    1.0.0
 *  @date       28.12.2025.
 *  @license    MIT
 */

// Include the main header file of the library.
#include "melodyController.h"

MelodyController::MelodyController()
{
    // Empty...for now. :)
}

void MelodyController::begin(uint16_t **_notes, uint16_t **_durations, uint8_t *_elements, uint8_t _noSongs)
{
    // Copy to the internal private variables and pointers.
    // Copy the pointer to the array of all notes of all melodies.
    this->_notesInternalArray = _notes;

    // Copy list of all note durations of all melodies.
    this->_durationsInternalArray = _durations;

    // Copy list the number of the elements of  every and each melody.
    this->_elementsInternalArray = _elements;

    // Copy the number of all melodies.
    this->_numberOfSongs = _noSongs;
}

void MelodyController::play()
{
    // Set the current song element to the 0, so the melody can start from the beginning.
    _currentSongElement = 0;

    // Set the first song.
    _currentSong = 0;

    // Get the current timestamp.
    _noteTimer = HAL_GetTick();

    // Generate a first note.
    if (!_muteStatus) buzzer_tone(*((uint16_t*)(_notesInternalArray[_currentSong]) + _currentSongElement), TIM_CHANNEL_1);

    // Set flag for status of the melody player.
    _playing = 1;

    // Get the current note duration (delay). Also calculate the tempo offset.
    _currentNoteDuration = (uint16_t)(*((uint16_t*)(_durationsInternalArray[_currentSong]) + _currentSongElement) * (1 + _tempoOffset / 100.0));
}

uint8_t MelodyController::update()
{
    // Check if the code needs to produce melody. If not, go back!
    if (!_playing) return 0;

    // Variable for the return value. 0 - no change, 1 - note has changed, 2 - song has changed.
    // By default, set it to 0 (no note change).
    _noteState = 0;

    // Check if the note needs to be changed.
    if ((unsigned long)(HAL_GetTick() - _noteTimer) >= _currentNoteDuration)
    {
        // Increment song element.
        _currentSongElement++;

        // Set the variable for the note change to 1.
        _noteState = 1;

        // Capture new time.
        _noteTimer = HAL_GetTick();

        // Check if the melody needs to be changed.
        if (_currentSongElement >= *((uint8_t*)(_elementsInternalArray) + _currentSong))
        {
            // Set the next song.
            next();

            // If the song / melody needs to be changed (it got till the end of the song / melody), set variable to 2.
            _noteState = 2;
        }
        else
        {
            // Play the note.
            if (!_muteStatus) buzzer_tone(*((uint16_t*)(_notesInternalArray[_currentSong]) + _currentSongElement), TIM_CHANNEL_1);
        }

        // Get the current note duration (delay). Also calculate the tempo offset.
        _currentNoteDuration = (uint16_t)(*((uint16_t*)(_durationsInternalArray[_currentSong]) + _currentSongElement) * (1 + _tempoOffset / 100.0));
    }

    // Return the state.
    return _noteState;
}

void MelodyController::next()
{
    // First, stop it!
    stop();

    // Wait a little bit.
    HAL_Delay(200);

    // Go to the next melody.
    _currentSong++;

    // Check if is the last melody. If it is, wrap it around.
    if (_currentSong >= _numberOfSongs) _currentSong = 0;

    // Reset the element counter.
    _currentSongElement = 0;

    // Get the new timestamp.
    _noteTimer = HAL_GetTick();

    // Generate the new note.
    if (!_muteStatus) buzzer_tone(*(((uint16_t*)(_notesInternalArray[_currentSong]) + _currentSongElement)), TIM_CHANNEL_1);

    // Get the current note duration (delay). Also calculate the tempo offset.
    _currentNoteDuration = (uint16_t)(*((uint16_t*)(_durationsInternalArray[_currentSong]) + _currentSongElement) * (1 + _tempoOffset / 100.0));

    // Set new state of the flag for the melody player.
    _playing = 1;
}

void MelodyController::prev()
{
    // First, stop it!
    stop();

    // Wait a little bit.
    HAL_Delay(200);

    // Go to the next melody.
    _currentSong--;

    // Check if is the last melody. If it is, wrap it around.
    if (_currentSong <= _numberOfSongs) _currentSong = _numberOfSongs;

    // Reset the element couter.
    _currentSongElement = 0;

    // Get the new timestamp.
    _noteTimer = HAL_GetTick();

    // Generate the new note.
    if (!_muteStatus) buzzer_tone(*((uint16_t*)(_notesInternalArray[_currentSong]) + _currentSongElement), TIM_CHANNEL_1);

    // Get the current note duration (delay). Also calculate the tempo offset.
    _currentNoteDuration = (uint16_t)((*(uint16_t*)(_durationsInternalArray[_currentSong]) + _currentSongElement) * (1 + _tempoOffset / 100.0));

    // Set new state of the flag for the melody player.
    _playing = 1;
}

void MelodyController::stop()
{
    // To stop the melody, mute the buzzer pin.
    buzzer_noTone(TIM_CHANNEL_1);

    // Reset the element counter.
    _currentSongElement = 0;

    // Set new state of the flag for the melody player.
    _playing = 0;
}

void MelodyController::setTempoOffset(int8_t _offset)
{
    // Limit to +/- 75%.
    if ((_offset > 75) || (_offset < -75)) return;

    // Save it to the internal variable. And also reverse is (positive tempo numbers should speed up the melody).
    _tempoOffset = -_offset;
}

int8_t MelodyController::getTempoOffset()
{
    return -_tempoOffset;
}

void MelodyController::setMute(uint8_t _mute)
{
    // Mute can only be enablec or disabled.
    _mute &= 1;

    // Store it into internal variable.
    _muteStatus = _mute;

    // If mute is enabled, stop any ongoing sound.
    buzzer_noTone(TIM_CHANNEL_1);
}

uint8_t MelodyController::getMute()
{
    // Return the local variable.
    return _muteStatus;
}

uint8_t MelodyController::isPlaying()
{
    // Return the local variable.
    return _playing;
}
