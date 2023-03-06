#pragma once

#include <AceRoutine.h>

#define BLOCKING_COROUTINE_DELAY(X) if(_isBlocking) delay(X); else COROUTINE_DELAY(X);

class IBlockingCoroutine : public ace_routine::Coroutine
{
    protected:
    bool _isBlocking;

    public:
    IBlockingCoroutine();
    void runBlocking();
};