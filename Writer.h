//
// Created by Frank Dattalo on 6/3/18.
//

#ifndef FVM_WRITER_H
#define FVM_WRITER_H


#include <cstdint>
#include <ostream>

class Writer {
private:
    std::ostream & out;

public:
    explicit Writer(std::ostream & ostream): out{ostream} {}

    void writeInteger(int64_t value);

    void writeDouble(double value);

    void writeLine();

    std::ostream& stream();
};


#endif //FVM_WRITER_H
