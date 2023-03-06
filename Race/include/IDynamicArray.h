#pragma once

#include "Arduino.h"
#include "Defines.h"

template <class T>
class IDynamicArray
{
private:
    T *_array;
    size_t _used;
    size_t _allocated;

public:
    IDynamicArray(size_t initialSize)
    {
        Init(initialSize);
    }

    ~IDynamicArray()
    {
        Clear();
    }

    void Add(T element)
    {
        if (_array == NULL)
        {
            Init(DYNAMIC_CHUNK_SIZE);
        }
        else if (_used == _allocated)
        {
            _allocated += DYNAMIC_CHUNK_SIZE;
            _array = (T *)realloc(_array, _allocated * sizeof(T));
        }
        _array[_used] = element;
        ++_used;
    }

    void Remove(size_t index)
    {
        //Replace item at "index" with the item at the end of the array
        //This does not preserve array order!
        if (index < _used)
        {
            if (index >= 0)
            {
                _array[index] = _array[_used - 1];
            }
            --_used;
        }
    }

    void Clear()
    {
        if (_array != NULL)
        {
            free(_array);
            _array = NULL;
            _used = _allocated = 0;
        }
    }

    void Sort()
    {
        if( _used <= 1 )
        {
            return;
        }
        
        //Bubble sort (since we have very small arrays)
        for (byte step = 0; step < _used - 1; ++step)
        {
            for (byte i = 0; i < _used - step - 1; ++i)
            {
                if (isSmaller(_array[i + 1], _array[i]))
                {
                    T tmp = _array[i];
                    _array[i] = _array[i + 1];
                    _array[i + 1] = tmp;
                }
            }
        }
    }

    T &operator[](size_t i)
    {
        return _array[i];
    }

    size_t Count() const
    {
        return _used;
    }

protected:
    virtual bool isSmaller(const T &first, const T &second) = 0;

private:
    void Init(size_t initialSize)
    {
        _used = 0;
        _allocated = initialSize;
        _array = (T *)malloc(initialSize * sizeof(T));
    }
};
