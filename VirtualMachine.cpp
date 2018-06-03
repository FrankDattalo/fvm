//
// Created by Frank Dattalo on 6/2/18.
//

#include "VirtualMachine.h"
#include "OpCode.h"
#include "Panic.h"
#include "Stack.h"

namespace  {
    int64_t retrieveInteger(uint8_t* code, std::size_t& ip) {
        uint8_t* base = &code[ip];
        auto * upCastedPointer = reinterpret_cast<int64_t *>(base);
        int64_t  ret = *upCastedPointer;
        ip += sizeof(int64_t);
        return ret;
    }

    std::size_t retrieveInstructionPointer(uint8_t* code, std::size_t& ip) {
        uint8_t* base = &code[ip];
        auto * upCastedPointer = reinterpret_cast<std::size_t *>(base);
        std::size_t  ret = *upCastedPointer;
        ip += sizeof(std::size_t);
        return ret;
    }
}

#define unaryOpOnStack(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint); \
        int64_t value = op data.pop(); \
        Logger::debug(TAG, "Value: " + std::to_string(value)); \
        data.push(value); \
        break;\
    }

#define binaryOpOnStack(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint);\
        int64_t left = data.pop();\
        int64_t right = data.pop();\
        Logger::debug(TAG, "Left: " + std::to_string(left) + ", Right: " + std::to_string(right));\
        int64_t result = left op right;\
        Logger::debug(TAG, "Result: " + std::to_string(result));\
        data.push(result);\
        break;\
    }

void VirtualMachine::interpret(std::vector<uint8_t>& codeVector) {
    uint8_t* code = codeVector.data();
    std::size_t size = codeVector.size();

    std::size_t ip = 0;

    bool loop = true;

    Stack<int64_t> data;
    Stack<std::size_t>  callStack;

    while (loop) {
        if (ip >= size) {
            panic_("Instruction pointer overrun! Ip: " + std::to_string(ip) + ", Size: " + std::to_string(size));
        }

        uint8_t currentByte = code[ip];
        auto opCode = static_cast<OpCode::Code>(currentByte);
        ip++;

        switch (opCode) {
            case OpCode::Code::Push: {
                Logger::debug(TAG, "Push");
                int64_t val = retrieveInteger(code, ip);
                data.push(val);
                break;
            }
            case OpCode::Code::Pop: {
                Logger::debug(TAG, "Pop");
                data.pop();
                break;
            }
            case OpCode::Code::Copy: {
                Logger::debug(TAG, "Copy");
                int64_t value = data.top();
                Logger::debug(TAG, "Value: " + std::to_string(value));
                data.push(value);
                break;
            }
            case OpCode::Code::PrintInteger: {
                Logger::debug(TAG, "PrintInteger");
                int64_t left = data.pop();
                std::cout << left;
                break;
            }
            case OpCode::Code::PrintLine: {
                Logger::debug(TAG, "PrintLine");
                std::cout << std::endl;
                break;
            }
            case OpCode::Code::End: {
                Logger::debug(TAG, "End");
                loop = false;
                break;
            }
            case OpCode::Code::Call: {
                Logger::debug(TAG, "Call");
                std::size_t newIp = retrieveInstructionPointer(code, ip);
                Logger::debug(TAG, "NewIp: " + std::to_string(newIp));
                callStack.push(ip);
                ip = newIp;
                break;
            }
            case OpCode::Code::Return: {
                Logger::debug(TAG, "Return");
                std::size_t restoredIp = callStack.pop();
                Logger::debug(TAG, "Restored Ip: " + std::to_string(restoredIp));
                ip = restoredIp;
                break;
            }
            case OpCode::Code::JumpZero: {
                Logger::debug(TAG, "JumpZero");
                std::size_t newIp = retrieveInstructionPointer(code, ip);
                int64_t value = data.pop();
                if (!value) {
                    Logger::debug(TAG, "Jumping to " + std::to_string(newIp));
                    ip = newIp;
                } else {
                    Logger::debug(TAG, "Not jumping");
                }
                break;
            }
            case OpCode::Code::Jump: {
                Logger::debug(TAG, "Jump");
                std::size_t newIp = retrieveInstructionPointer(code, ip);
                Logger::debug(TAG, "Jumping to " + std::to_string(newIp));
                ip = newIp;
                break;
            }
            binaryOpOnStack(OpCode::Code::Add, "Add", +)
            binaryOpOnStack(OpCode::Code::Subtract, "Subtract", -)
            binaryOpOnStack(OpCode::Code::Multiply, "Multiply", *)
            binaryOpOnStack(OpCode::Code::Divide, "Divide", /)
            binaryOpOnStack(OpCode::Code::Modulus, "Modulus", %)
            binaryOpOnStack(OpCode::Code::GreaterThanOrEqual, "GreaterThanOrEqual", >=)
            binaryOpOnStack(OpCode::Code::GreaterThan, "GreaterThan", >)
            binaryOpOnStack(OpCode::Code::LessThanOrEqual, "LessThanOrEqual", <=)
            binaryOpOnStack(OpCode::Code::LessThan, "LessThan", <)
            binaryOpOnStack(OpCode::Code::Equal, "Equal", ==)
            binaryOpOnStack(OpCode::Code::NotEqual, "NotEqual", !=)
            unaryOpOnStack(OpCode::Code::Not, "Not", !)
            unaryOpOnStack(OpCode::Code::Negate, "Negate", -)
            default: {
                panic_("Unknown opCode: " + std::to_string(currentByte));
            }
        }

        data.debugStack();
    }
}

#undef binaryOpOnStack
#undef unaryOpOnStack