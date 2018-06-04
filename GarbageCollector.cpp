//
// Created by Frank Dattalo on 6/3/18.
//

#include <set>
#include <thread>
#include <list>
#include "GarbageCollector.h"
#include "Heap.h"

namespace {
    const std::string TAG("GarbageCollector");

    void accumulateStackReferences(Stack<Object> & data,
                                   Heap & heap,
                                   std::list<HeapObject *> & outList) {

        assert_(outList.size() == 0, "OutList was not zeroed out!");

        Object outObject{};

        for (size_t i = 0; data.at(i, outObject); i++) {
            if (heap.isHeapObject(outObject.ptr64)) {
                Logger::debug(TAG, "Found heap object reference on stack");
                outList.push_back(outObject.ptr64);
            }
        }

        Logger::debug(TAG, "Found " + std::to_string(outList.size()) + " heap references on the stack");
    }

    void mark(std::list<HeapObject *> & searchBuffer, Heap & heap, int64_t heapGeneration) {

        std::list<HeapObject*> temp;

        Logger::debug(TAG, "Mark started with initial search buffer size of " + std::to_string(searchBuffer.size()));

        int64_t searchIteration = 0;

        while (searchBuffer.size() > 0) {

            Logger::debug(TAG, "Iteration " + std::to_string(searchIteration)
                               + " starting search buffer size is " + std::to_string(searchBuffer.size()));

            while (searchBuffer.size() > 0) {
                HeapObject* item = searchBuffer.front();
                searchBuffer.pop_front();

                if (item->markFlag) {
                    Logger::debug(TAG, "Detected circular reference");
                    continue;
                }

                item->heapGeneration = heapGeneration;
                item->markFlag = true;

                int64_t numFields = item->numberObjects;
                Logger::debug(TAG, "Recursively searching " + std::to_string(numFields) + " fields for heap references");

                int64_t numFound = 0;

                for (int64_t i = 0; i < numFields; i++) {
                    Object objectI = item->get(i);
                    if (heap.isHeapObject(objectI.ptr64)) {
                        Logger::debug(TAG, "Found heap reference within heap object");
                        temp.push_front(objectI.ptr64);
                        numFound++;
                    }
                }

                Logger::debug(TAG, "Found " + std::to_string(numFound) + " heap references in members");
            }

            std::swap(temp, searchBuffer);

            searchIteration++;
        }

        Logger::debug(TAG, "Mark finished after " + std::to_string(searchIteration) + " iterations");
    }

    void sweep(Heap & heap, int64_t heapGeneration) {

        heap.beginDeleteHeapObjects();

        heap.freeHeapObjects(heapGeneration);

        heap.commitDeleteObjects();
    }
}

void GarbageCollector::run(Stack<Object> &data, Heap & heap, bool &loop) {

    Logger::debug(TAG, "GC thread started");

    int64_t heapGeneration = 1;
    std::list<HeapObject *> stackReferences;

    while (loop) {

        assert_(stackReferences.size() == 0, "Stack references should be cleared every iteration!");

        Logger::debug(TAG, "Beginning generation " + std::to_string(heapGeneration));

        accumulateStackReferences(data, heap, stackReferences);

        mark(stackReferences, heap, heapGeneration);
        sweep(heap, heapGeneration);

        heapGeneration++;
    }

    Logger::debug(TAG, "GC thread ending");
}
