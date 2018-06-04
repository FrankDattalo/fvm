//
// Created by Frank Dattalo on 6/3/18.
//

#include "Heap.h"
#include "Assert.h"
#include "Logger.h"

namespace {
    const std::string TAG ("Heap");
}

HeapObject * Heap::newHeapObject(int64_t numberObjects) {

    Logger::debug(TAG, "newHeapObject - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "newHeapObject - acquired lock");

    Logger::debug(TAG, "Creating new heap object");

    auto * heapObject = new HeapObject(numberObjects);

    this->heapObjects.insert(heapObject);

    return heapObject;
}

bool Heap::isHeapObject(HeapObject *maybeReference) {

    Logger::debug(TAG, "isHeapObject - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "isHeapObject - acquired lock");

    auto find = this->heapObjects.find(maybeReference);
    return find != this->heapObjects.end();
}

HeapObject * Heap::getHeapObject(Object object) {
    Logger::debug(TAG, "getHeapObject - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "getHeapObject - acquired lock");

    Logger::debug(TAG, "Retrieving object from heap");

    auto find = this->heapObjects.find(object.ptr64);

    assert_(find != this->heapObjects.end(), "Tried invalid dereference!");

    return *find;
}

void Heap::commitDeleteObjects() {
    Logger::debug(TAG, "commitDeleteObjects - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "commitDeleteObjects - acquired lock");

    for (HeapObject* heapObject : this->markedForDelete) {

        std::string desc(std::to_string(reinterpret_cast<int64_t>(heapObject)));

        Logger::debug(TAG, "Freeing heap object " + desc);

        Logger::debug(TAG, "Trying to erase heap object from set");

        this->heapObjects.erase(heapObject);

        Logger::debug(TAG, "Heap object " + desc + " erased from set");

        delete heapObject;

        Logger::debug(TAG, "Heap object " + desc + " freed from memory");
    }
}

void Heap::beginDeleteHeapObjects() {

    Logger::debug(TAG, "beginDeleteHeapObjects - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "beginDeleteHeapObjects - acquired lock");

    this->markedForDelete.clear();
}

void Heap::debugHeap() {
    Logger::debug(TAG, "debugHeap - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "debugHeap - acquired lock");

    auto stream = Logger::debugStream();

    stream << std::endl;

    for (HeapObject* heapObject : this->heapObjects) {
        stream << reinterpret_cast<int64_t>(heapObject)
               << "  GCAllowed: " << heapObject->gcAllowed
               << ", Marked: " << heapObject->markFlag
               << ", NumObjects: " << heapObject->numberObjects
               << ", HeapGen: " << heapObject->heapGeneration
               << std::endl;
    }

    Logger::debug(TAG, stream);
}

void Heap::freeHeapObjects(int64_t heapGeneration) {

    Logger::debug(TAG, "freeHeapObjects - waiting to acquire lock");

    std::lock_guard<std::mutex> guard(lock);

    Logger::debug(TAG, "freeHeapObjects - acquired lock");

    int64_t nextHeapGeneration = heapGeneration + 1;

    for (HeapObject* object: this->heapObjects) {
        if (object->gcAllowed && heapGeneration == object->heapGeneration && !object->markFlag) {

            Logger::debug(TAG, "Found unreachable object at "
                               + std::to_string(reinterpret_cast<int64_t>(object)));

            this->markedForDelete.push_back(object);

        } else {

            Logger::debug(TAG, "Heap object at "
                               + std::to_string(reinterpret_cast<int64_t>(object))
                               + " is reachable");

            object->heapGeneration = nextHeapGeneration;
            object->markFlag = false;
        }
    }
}
