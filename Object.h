//
// Created by Frank Dattalo on 6/3/18.
//

#ifndef FVM_OBJECT_H
#define FVM_OBJECT_H


#include <cstdint>
#include <cstddef>

class HeapObject;

union Object {
    int64_t     int64;
    double      float64;
    HeapObject* ptr64;
};

class HeapObject {
private:
    union {
        Object* objects;
        char* chars;
    };
    int8_t      heapGeneration;
    int8_t      bitFlags;
    int64_t     _size;

    enum class BitFlag {
        IsString, ShouldFreeMemory, HasReferences, IsMarked, IsGarbageCollectionAllowed
    };

    bool getBitFlag(BitFlag flag);

    void setBitFlag(BitFlag flag, bool value);

public:

    explicit HeapObject(int64_t size, int8_t heapGeneration);

    explicit HeapObject(int64_t size, int8_t heapGeneration, char* chars, bool shouldFreeMemory);

    ~HeapObject();

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

    int64_t size();

    char* string();
};


#endif //FVM_OBJECT_H
