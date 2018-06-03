//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_ASSEMBLER_H
#define FVM_ASSEMBLER_H

#include <cstdint>
#include <string>
#include <map>

namespace Assembler {
    void assemble(const std::string & input, std::vector<uint8_t> & out);
};


#endif //FVM_ASSEMBLER_H
