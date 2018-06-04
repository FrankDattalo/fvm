//
// Created by Frank Dattalo on 6/3/18.
//

#ifndef FVM_GARBAGECOLLECTOR_H
#define FVM_GARBAGECOLLECTOR_H


#include <cstdint>
#include <cstddef>
#include "Object.h"
#include "Stack.h"
#include "Heap.h"

namespace GarbageCollector {
    void run(Stack<Object> &data, Heap & heap, bool &loop);
};


#endif //FVM_GARBAGECOLLECTOR_H
