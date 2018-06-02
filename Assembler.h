//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_ASSEMBLER_H
#define FVM_ASSEMBLER_H

#include <cstdint>
#include <string>
#include <map>

namespace Assembler {
    std::vector<uint8_t> assemble(const std::string & input);
};


#endif //FVM_ASSEMBLER_H
