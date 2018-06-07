//
// Created by Frank Dattalo on 6/6/18.
//

#ifndef FVM_HEAPOBJECT_H
#define FVM_HEAPOBJECT_H

#include <cstdint>

#include "Object.h"

class HeapObject {
private:
    union {
        Object* objects;
        char*   chars;
    };
    int8_t      heapGeneration;
    uint8_t     bitFlags;
    int64_t     _size;

    enum class BitFlag {
        IsString, ShouldFreeMemory, HasReferences, IsMarked, IsGarbageCollectionAllowed
    };

    bool getBitFlag(BitFlag flag);

    void setBitFlag(BitFlag flag, bool value);

public:

    explicit HeapObject(int64_t size, int8_t heapGeneration);

    explicit HeapObject(int64_t size, int8_t heapGeneration, const char* chars, bool copy);

    ~HeapObject();

    void setShouldFreeMemory(bool value);

    bool shouldFreeMemory();

    void set(int64_t offset, const Object & value);

    Object get(int64_t offset);

    void setMark(bool value);

    bool isMarked();

    void setHasReferences(bool value);

    bool hasReferences();

    void setGarbageCollectionAllowed(bool value);

    bool isGarbageCollectionAllowed();

    int8_t getHeapGeneration();

    void setHeapGeneration(int8_t value);

    bool isString();

    void setIsString(bool value);

    int64_t size();

    char* string();
};


#endif //FVM_HEAPOBJECT_H
