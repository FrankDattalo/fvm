//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_OPCODE_H
#define FVM_OPCODE_H

#include <cstdint>

class OpCode {
public:
    enum Code {
        Add,
        Subtract,
        Divide,
        Multiply,
        Modulus,
        Push,
        Pop,
        PrintInteger,
        PrintLine,
        Not,
        Negate,
        LessThanOrEqual,
        LessThan,
        GreaterThan,
        GreaterThanOrEqual,
        NotEqual,
        Equal,
        Jump,
        JumpZero,
        Copy,
        Call,
        Return,
        End
    };

    static uint8_t getByte(OpCode::Code code);
};


#endif //FVM_OPCODE_H
