//
// Created by Frank Dattalo on 6/3/18.
//

#include <string>
#include "Object.h"
#include "Logger.h"
#include "Assert.h"

namespace {
    const std::string TAG ( "HeapObject" );
}

HeapObject::HeapObject(int64_t numberObjects) {

    assert_(numberObjects >= 0, "Number of objects was negative!");

    Logger::debug(TAG, "Initializing heap object with " + std::to_string(numberObjects) + " objects");

    this->gcAllowed = false;
    this->markFlag = false;
    this->heapGeneration = 0;
    this->numberObjects = numberObjects;
    this->objects = new Object[this->numberObjects];
}

HeapObject::~HeapObject() {

    Logger::debug(TAG, "Destroying heap object with " + std::to_string(this->numberObjects) + " objects");

    delete[] this->objects;
}

void HeapObject::set(int64_t offset, Object value) {
    assert_(offset >= 0, "Offset was negative!");
    assert_(offset < this->numberObjects, "Offset out of bounds!");

    this->objects[offset] = value;
}

Object HeapObject::get(int64_t offset) {
    assert_(offset >= 0, "Offset was negative!");
    assert_(offset < this->numberObjects, "Offset out of bounds!");

    return this->objects[offset];
}
