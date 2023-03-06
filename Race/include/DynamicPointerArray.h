#include "IDynamicArray.h"

template <class T>
class DynamicPointerArray : public IDynamicArray<T>
{
    public:
    DynamicPointerArray(size_t initialSize = DYNAMIC_CHUNK_SIZE) : IDynamicArray<T>(initialSize)
    {

    }
protected:
    virtual bool isSmaller(const T &first, const T &second) override
    {
        return *first < *second;
    }
};