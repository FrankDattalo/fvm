//
// Created by Frank Dattalo on 6/1/18.
//

#include "ByteBuffer.h"
#include "Assert.h"

namespace {
    constexpr std::string TAG ("ByteBuffer");
}

void ByteBuffer::rewrite(uint8_t *bytes, std::size_t size, std::size_t position) {

    std::size_t end = position + size;

    assert_(position < this->data.size() && end <= this->data.size(),
            "Rewrite position out of bounds!");

    Logger::debug(TAG, "Rewriting " + std::to_string(size) + " bytes at position " + std::to_string(position));

    for (std::size_t i = position, j = 0; i < end; ++i, ++j) {
        this->data[i] = bytes[j];
    }
}

void ByteBuffer::append(uint8_t *bytes, std::size_t size) {
    Logger::debug(TAG, "Grew buffer by " + std::to_string(size) + " bytes");

    for (std::size_t i = 0; i < size; ++i) {
        this->data.push_back(bytes[i]);
    }
}

void ByteBuffer::debugBytes() {

    auto stream = Logger::debugStream();

    std::size_t size = this->data.size();
    for (std::size_t i = 0; i < size; ++i) {
        stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(this->data[i]);

        if (i != size - 1) {
            stream << " ";
        }
    }

    Logger::debug(TAG, stream);
}

std::vector<uint8_t> ByteBuffer::vector() {
    return this->data;
}
