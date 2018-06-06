//
// Created by Frank Dattalo on 6/3/18.
//

#include "Heap.h"
#include "Assert.h"
#include "Debug.h"

namespace {
    const std::string TAG ("Heap");
}

HeapObject * Heap::newHeapObject(int64_t numberObjects) {

    DEBUG(TAG, "newHeapObject - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "newHeapObject - acquired lock");

    DEBUG(TAG, "Creating new heap object");

    auto * heapObject = new HeapObject(numberObjects, static_cast<int8_t>(this->heapGeneration - 1));

    this->heapObjects.insert(heapObject);

    return heapObject;
}

bool Heap::isHeapObject(HeapObject *maybeReference) {

    DEBUG(TAG, "isHeapObject - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "isHeapObject - acquired lock");

    auto find = this->heapObjects.find(maybeReference);
    return find != this->heapObjects.end();
}

HeapObject * Heap::getHeapObject(Object object) {
    DEBUG(TAG, "getHeapObject - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "getHeapObject - acquired lock");

    DEBUG(TAG, "Retrieving object from heap");

    auto find = this->heapObjects.find(object.ptr64);

    RUNTIME_ASSERT(find != this->heapObjects.end(), "Tried invalid dereference!");

    return *find;
}

void Heap::commitDeleteObjects() {
    DEBUG(TAG, "commitDeleteObjects - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "commitDeleteObjects - acquired lock");

    for (HeapObject* heapObject : this->markedForDelete) {

        IF_DEBUG(std::string desc(std::to_string(reinterpret_cast<int64_t>(heapObject))));

        DEBUG(TAG, "Freeing heap object " + desc);

        DEBUG(TAG, "Trying to erase heap object from set");

        this->heapObjects.erase(heapObject);

        DEBUG(TAG, "Heap object " + desc + " erased from set");

        delete heapObject;

        DEBUG(TAG, "Heap object " + desc + " freed from memory");
    }
}

void Heap::beginDeleteHeapObjects() {

    DEBUG(TAG, "beginDeleteHeapObjects - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "beginDeleteHeapObjects - acquired lock");

    this->markedForDelete.clear();
}

IF_DEBUG(void Heap::debugHeap() {
    DEBUG(TAG, "debugHeap - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "debugHeap - acquired lock");

    LOG_BEGIN();

    LOG_STREAM(std::endl);

    for (HeapObject* heapObject : this->heapObjects) {
        LOG_STREAM(reinterpret_cast<int64_t>(heapObject)
               << "  GCAllowed: " << heapObject->gcAllowed
               << ", Marked: " << heapObject->markFlag
               << ", NumObjects: " << heapObject->numberObjects
               << ", HeapGen: " << heapObject->heapGeneration
               << std::endl);
    }

    LOG_END(TAG);
})

void Heap::freeHeapObjects() {

    DEBUG(TAG, "freeHeapObjects - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    DEBUG(TAG, "freeHeapObjects - acquired lock");

    int8_t nextHeapGeneration = static_cast<int8_t>(heapGeneration + 1);

    for (HeapObject* object: this->heapObjects) {
        if (object->isGarbageCollectionAllowed() && heapGeneration == object->getHeapGeneration() && !object->isMarked()) {

            DEBUG(TAG, "Found unreachable object at "
                               + std::to_string(reinterpret_cast<int64_t>(object)));

            this->markedForDelete.push_back(object);

        } else {

            DEBUG(TAG, "Heap object at "
                               + std::to_string(reinterpret_cast<int64_t>(object))
                               + " is reachable");

            object->setHeapGeneration(nextHeapGeneration);
            object->setMark(false);
        }
    }
}
