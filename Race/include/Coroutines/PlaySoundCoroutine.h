#pragma once

#include "Coroutines/IBlockingCoroutine.h"

class PlaySoundCoroutine : public IBlockingCoroutine
{
    protected:
        word _frequency;
        word* _frequencyArray;
        uint32_t _delay;
        byte _numSounds;
        byte _index;
    
    public:
        void setParameters( word frequency, uint32_t delay);
        void setParameters( word* frequencyArray, byte numSounds, uint32_t delay);
        int runCoroutine() override;
};