//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_BYTEBUFFER_H
#define FVM_BYTEBUFFER_H

#include <vector>

#include "Logger.h"

class ByteBuffer {
private:
    std::vector<uint8_t>& data;

public:
    explicit ByteBuffer(std::vector<uint8_t>& outData): data {outData} {};

    void append(uint8_t* bytes, std::size_t size);

    void rewrite(uint8_t* bytes, std::size_t size, std::size_t position);

    void debugBytes();
};


#endif //FVM_BYTEBUFFER_H
