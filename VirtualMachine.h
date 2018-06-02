//
// Created by Frank Dattalo on 6/2/18.
//

#ifndef FVM_VIRTUALMACHINE_H
#define FVM_VIRTUALMACHINE_H

#include <cstdint>
#include <vector>

namespace VirtualMachine {

    void interpret(std::vector<uint8_t>& code);
};


#endif //FVM_VIRTUALMACHINE_H
