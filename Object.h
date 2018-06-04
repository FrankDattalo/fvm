//
// Created by Frank Dattalo on 6/3/18.
//

#ifndef FVM_OBJECT_H
#define FVM_OBJECT_H


#include <cstdint>
#include <cstddef>

class HeapObject;

union Object {
    int64_t     int64;
    double      float64;
    HeapObject* ptr64;
};

class HeapObject {
private:
    Object*     objects;
public:
    int64_t     heapGeneration;
    bool        markFlag;
    bool        gcAllowed;
    int64_t     numberObjects;

    explicit HeapObject(int64_t numberObjects);

    ~HeapObject();

    void set(int64_t offset, Object value);

    Object get(int64_t offset);
};


#endif //FVM_OBJECT_H
