//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_BYTEBUFFER_H
#define FVM_BYTEBUFFER_H

#include <vector>

#include "Logger.h"

class ByteBuffer {
private:
    std::vector<uint8_t> data;

public:

    void append(uint8_t* bytes, std::size_t size);

    void rewrite(uint8_t* bytes, std::size_t size, std::size_t position);

    std::vector<uint8_t> vector();

    void debugBytes();
};


#endif //FVM_BYTEBUFFER_H
