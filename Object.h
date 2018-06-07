//
// Created by Frank Dattalo on 6/3/18.
//

#ifndef FVM_OBJECT_H
#define FVM_OBJECT_H


#include <cstdint>

class HeapObject;

union Object {
    int64_t     int64;
    double      float64;
    HeapObject* ptr64;
};

#endif //FVM_OBJECT_H
