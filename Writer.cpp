//
// Created by Frank Dattalo on 6/3/18.
//

#include "Writer.h"

void Writer::writeInteger(int64_t value) {
    this->out << value;
}

void Writer::writeDouble(double value) {
    this->out << value;
}

void Writer::writeLine() {
    this->out << std::endl;
}

std::ostream & Writer::stream() {
    return this->out;
}
