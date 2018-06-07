//
// Created by Frank Dattalo on 6/6/18.
//

#include <string>
#include "HeapObject.h"
#include "Panic.h"
#include "Debug.h"
#include "Assert.h"


namespace {
    const std::string TAG ( "HeapObject" );
}

HeapObject::HeapObject(int64_t size, int8_t heapGeneration) {

    RUNTIME_ASSERT(size >= 0, "Size was negative!");

    DEBUG(TAG, "Initializing heap object with " + std::to_string(size) + " objects");

    this->_size = size;
    this->objects = new Object[this->_size];
    this->heapGeneration = heapGeneration;
    this->bitFlags = 0;
    this->setShouldFreeMemory(true);
}

HeapObject::HeapObject(int64_t size, int8_t heapGeneration, const char *chars, bool copy) {
    RUNTIME_ASSERT(size >= 0, "Size was negative!");

    DEBUG(TAG, "Initializing heap object with " + std::to_string(size) + " objects");

    this->_size = size;
    this->heapGeneration = heapGeneration;
    this->bitFlags = 0;
    this->setIsString(true);

    if (copy) {
        this->setShouldFreeMemory(true);
        this->chars = new char[size + 1];
        memcpy(this->chars, chars, static_cast<size_t>(size + 1));

    } else {
        this->setShouldFreeMemory(false);
        this->chars = const_cast<char*>(chars);
    }
}

HeapObject::~HeapObject() {

    DEBUG(TAG, "Destroying heap object with " + std::to_string(this->_size) + " objects");

    if (this->shouldFreeMemory()) {
        if (this->isString()) {
            delete[] this->chars;
        } else {
            delete[] this->objects;
        }
    }
}

void HeapObject::set(int64_t offset, const Object & value) {
    RUNTIME_ASSERT(offset >= 0, "Offset was negative!");
    RUNTIME_ASSERT(offset < this->_size, "Offset out of bounds!");
    RUNTIME_ASSERT(!this->isString(), "Tried to call set on a string!");

    this->objects[offset] = value;
}

Object HeapObject::get(int64_t offset) {
    RUNTIME_ASSERT(offset >= 0, "Offset was negative!");
    RUNTIME_ASSERT(offset < this->_size, "Offset out of bounds!");
    RUNTIME_ASSERT(!this->isString(), "Tried to call get on a string!");

    return this->objects[offset];
}

bool HeapObject::getBitFlag(HeapObject::BitFlag flag) {
    auto mask = static_cast<uint8_t>(1u << static_cast<uint8_t>(flag));
    uint8_t ret = this->bitFlags & mask;
    return ret != 0;
}

void HeapObject::setBitFlag(HeapObject::BitFlag flag, bool value) {

    auto set = static_cast<uint8_t>(1u << static_cast<uint8_t>(flag));

    if (value) {
        this->bitFlags = this->bitFlags | set;
    } else {
        set = ~set;
        this->bitFlags = this->bitFlags & set;
    }
}

void HeapObject::setMark(bool value) {
    setBitFlag(HeapObject::BitFlag::IsMarked, value);
}

bool HeapObject::isMarked() {
    return getBitFlag(HeapObject::BitFlag::IsMarked);
}

void HeapObject::setHasReferences(bool value) {
    setBitFlag(HeapObject::BitFlag::HasReferences, value);
}

bool HeapObject::hasReferences() {
    return getBitFlag(HeapObject::BitFlag::HasReferences);
}

void HeapObject::setGarbageCollectionAllowed(bool value) {
    setBitFlag(HeapObject::BitFlag::IsGarbageCollectionAllowed, value);
}

bool HeapObject::isGarbageCollectionAllowed() {
    return getBitFlag(HeapObject::BitFlag::IsGarbageCollectionAllowed);
}

int8_t HeapObject::getHeapGeneration() {
    return this->heapGeneration;
}

void HeapObject::setHeapGeneration(int8_t value) {
    this->heapGeneration = value;
}

bool HeapObject::isString() {
    return getBitFlag(HeapObject::BitFlag::IsString);
}

int64_t HeapObject::size() {
    return this->_size;
}

char * HeapObject::string() {
    RUNTIME_ASSERT(this->isString(), "Tried to call string on a non-string");

    return this->chars;
}

void HeapObject::setShouldFreeMemory(bool value) {
    setBitFlag(HeapObject::BitFlag::ShouldFreeMemory, value);
}

bool HeapObject::shouldFreeMemory() {
    return getBitFlag(HeapObject::BitFlag::ShouldFreeMemory);
}

void HeapObject::setIsString(bool value) {
    setBitFlag(HeapObject::BitFlag::IsString, value);
}
