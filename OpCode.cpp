//
// Created by Frank Dattalo on 6/1/18.
//

#include "OpCode.h"

uint8_t OpCode::getByte(OpCode::Code code) {
    return static_cast<uint8_t>(code);
}
