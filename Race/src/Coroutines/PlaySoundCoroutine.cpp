#include "Coroutines/PlaySoundCoroutine.h"

#include "Defines.h"

#ifdef ESP32
#include <ESP32Tone.h>
#endif

void PlaySoundCoroutine::setParameters(word frequency, uint32_t delay)
{
    reset();
    noTone(PIN_AUDIO);
    _frequency = frequency;
    _numSounds = 0;
    _delay = delay;
}

void PlaySoundCoroutine::setParameters(word *frequencyArray, byte numSounds, uint32_t delay)
{
    reset();
    noTone(PIN_AUDIO);
    _frequencyArray = frequencyArray;
    _index = 0;
    _numSounds = numSounds;
    _delay = delay;
}

int PlaySoundCoroutine::runCoroutine()
{
    COROUTINE_BEGIN();

    if (_numSounds == 0)
    {
        tone(PIN_AUDIO, _frequency);
        BLOCKING_COROUTINE_DELAY(_delay);
    }
    else
    {
        while( _index < _numSounds)
        {
            tone(PIN_AUDIO, _frequencyArray[_index]);
            ++_index;
            BLOCKING_COROUTINE_DELAY(_delay);
        }
    }
    noTone(PIN_AUDIO);

    COROUTINE_END();
}