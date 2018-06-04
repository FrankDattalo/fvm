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

class Heap {
private:
    std::mutex                      lock;
    std::unordered_set<HeapObject*> heapObjects;
    std::list<HeapObject*>          markedForDelete;

public:
    HeapObject* newHeapObject(int64_t numberObjects);

    bool isHeapObject(HeapObject* maybeReference);

    HeapObject* getHeapObject(Object object);

    void commitDeleteObjects();

    void beginDeleteHeapObjects();

    void freeHeapObjects(int64_t heapGeneration);

    void debugHeap();
};


#endif //FVM_HEAP_H
