//
// Created by Frank Dattalo on 6/3/18.
//

#include <string>

#include "Object.h"
#include "Debug.h"
#include "Panic.h"

namespace {
    const std::string TAG ( "HeapObject" );
}

HeapObject::HeapObject(int64_t size, int8_t heapGeneration) {

    RUNTIME_ASSERT(size >= 0, "Size was negative!");

    DEBUG(TAG, "Initializing heap object with " + std::to_string(numberObjects) + " objects");


}

HeapObject::~HeapObject() {

    DEBUG(TAG, "Destroying heap object with " + std::to_string(this->numberObjects) + " objects");

    delete[] this->objects;
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
