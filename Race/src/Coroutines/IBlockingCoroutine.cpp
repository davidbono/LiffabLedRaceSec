#include "Coroutines/IBlockingCoroutine.h"

IBlockingCoroutine::IBlockingCoroutine() : _isBlocking(false)
{
}

void IBlockingCoroutine::runBlocking()
{
    _isBlocking = true;
    runCoroutine();
    _isBlocking = false;
}