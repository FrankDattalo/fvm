//
// Created by Frank Dattalo on 6/3/18.
//

#include <set>
#include <thread>
#include <list>
#include <cstdlib>

#include "HeapObject.h"
#include "GarbageCollector.h"
#include "Heap.h"
#include "Panic.h"

namespace {
    const std::string TAG("GarbageCollector");

    void accumulateStackReferences(Stack<Object> & data,
                                   Heap & heap,
                                   std::list<HeapObject *> & outList) {

        ASSERT(outList.empty(), "OutList was not zeroed out!");

        Object outObject{};

        for (size_t i = 0; data.at(i, outObject); i++) {
            if (heap.isHeapObject(outObject.ptr64)) {
                DEBUG(TAG, "Found heap object reference on stack");
                outList.push_back(outObject.ptr64);
            }
        }

        DEBUG(TAG, "Found " + std::to_string(outList.size()) + " heap references on the stack");
    }

    void mark(std::list<HeapObject *> & searchBuffer, Heap & heap, int8_t heapGeneration) {

        std::list<HeapObject*> temp;

        DEBUG(TAG, "Mark started with initial search buffer size of " + std::to_string(searchBuffer.size()));

        IF_DEBUG(int64_t searchIteration = 0);

        while (!searchBuffer.empty()) {

            DEBUG(TAG, "Iteration " + std::to_string(searchIteration)
                               + " starting search buffer size is " + std::to_string(searchBuffer.size()));

            while (!searchBuffer.empty()) {
                HeapObject* item = searchBuffer.front();
                searchBuffer.pop_front();

                if (item->isMarked()) {
                    DEBUG(TAG, "Detected circular reference");
                    continue;
                }

                item->setHeapGeneration(heapGeneration);
                item->setMark(true);

                if (item->hasReferences()) {
                    int64_t numFields = item->size();
                    DEBUG(TAG, "Recursively searching " + std::to_string(numFields) + " fields for heap references");

                    IF_DEBUG(int64_t numFound = 0);

                    for (int64_t i = 0; i < numFields; i++) {
                        Object objectI = item->get(i);
                        if (heap.isHeapObject(objectI.ptr64)) {
                            DEBUG(TAG, "Found heap reference within heap object");
                            temp.push_front(objectI.ptr64);
                            IF_DEBUG(numFound++);
                        }
                    }

                    DEBUG(TAG, "Found " + std::to_string(numFound) + " heap references in members");
                }
            }

            ASSERT(searchBuffer.empty(), "Search buffer was non empty before swap");

            std::swap(temp, searchBuffer);

            IF_DEBUG(searchIteration++);
        }

        DEBUG(TAG, "Mark finished after " + std::to_string(searchIteration) + " iterations");
    }

    void sweep(Heap & heap) {

        heap.beginDeleteHeapObjects();

        heap.freeHeapObjects();

        heap.commitDeleteObjects();
    }
}

void GarbageCollector::run(Stack<Object> &data, Heap & heap, bool &loop) {

    DEBUG(TAG, "GC thread started");

    int8_t heapGeneration = 1;
    std::list<HeapObject *> stackReferences;

    while (loop) {

        ASSERT(stackReferences.empty(), "Stack references should be cleared every iteration!");

        DEBUG(TAG, "Beginning generation " + std::to_string(heapGeneration));

        heap.setHeapGeneration(heapGeneration);

        accumulateStackReferences(data, heap, stackReferences);

        mark(stackReferences, heap, heapGeneration);
        sweep(heap);

        heapGeneration++;
    }

    DEBUG(TAG, "GC thread ending");
}
