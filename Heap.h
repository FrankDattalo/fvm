//
// Created by Frank Dattalo on 6/3/18.
//

#ifndef FVM_HEAP_H
#define FVM_HEAP_H


#include <functional>
#include <mutex>
#include <list>
#include <unordered_set>

#include "Object.h"
#include "Debug.h"

class Heap {
private:
    std::mutex                      lock;
    std::unordered_set<HeapObject*> heapObjects;
    std::list<HeapObject*>          markedForDelete;
    int8_t                          heapGeneration;

public:
    HeapObject* newHeapObject(int64_t numberObjects);

    bool isHeapObject(HeapObject* maybeReference);

    HeapObject* getHeapObject(Object object);

    void commitDeleteObjects();

    void freeHeapObjects();

    void beginDeleteHeapObjects();

    void setHeapGeneration(int8_t gen);

    IF_DEBUG(void debugHeap());
};


#endif //FVM_HEAP_H
