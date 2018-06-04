//
// Created by Frank Dattalo on 6/2/18.
//

#include <thread>
#include "VirtualMachine.h"
#include "OpCode.h"
#include "Panic.h"
#include "Stack.h"
#include "Writer.h"
#include "Object.h"
#include "Heap.h"
#include "GarbageCollector.h"

namespace  {
    const std::string TAG("VirtualMachine");

    Object retrieveImmediate(uint8_t* code, std::size_t& ip) {
        uint8_t* base = &code[ip];
        auto * upCastedPointer = reinterpret_cast<Object *>(base);
        Object  ret = *upCastedPointer;
        ip += sizeof(Object);
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

#define unaryIntegerOpOnStack(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint); \
        Object val = data.pop(); \
        val.int64 = op val.int64; \
        Logger::debug(TAG, "Value: " + std::to_string(val.int64)); \
        data.push(val); \
        break;\
    }

#define binaryIntegerOpOnStack(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint);\
        Object left = data.pop();\
        Object right = data.pop();\
        Logger::debug(TAG, "Left: " + std::to_string(left.int64) + ", Right: " + std::to_string(right.int64));\
        int64_t result = left.int64 op right.int64;\
        Logger::debug(TAG, "Result: " + std::to_string(result));\
        Object o {};\
        o.int64 = result;\
        data.push(o);\
        break;\
    }

#define unaryFloatOpOnStack(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint); \
        Object val = data.pop(); \
        val.float64 = op val.float64; \
        Logger::debug(TAG, "Value: " + std::to_string(val.float64)); \
        data.push(val); \
        break;\
    }

#define binaryFloatOpOnStack(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint);\
        Object left = data.pop();\
        Object right = data.pop();\
        Logger::debug(TAG, "Left: " + std::to_string(left.float64) + ", Right: " + std::to_string(right.float64));\
        double result = left.float64 op right.float64;\
        Logger::debug(TAG, "Result: " + std::to_string(result));\
        Object o {};\
        o.float64 = result;\
        data.push(o);\
        break;\
    }

#define binaryFloatOpOnStackToBoolean(opCode, toPrint, op) \
    case opCode: { \
        Logger::debug(TAG, toPrint);\
        Object left = data.pop();\
        Object right = data.pop();\
        Logger::debug(TAG, "Left: " + std::to_string(left.float64) + ", Right: " + std::to_string(right.float64));\
        bool result = left.float64 op right.float64;\
        int64_t upcasted = result; \
        Logger::debug(TAG, "Result: " + std::to_string(upcasted));\
        Object o {};\
        o.int64 = upcasted;\
        data.push(o);\
        break;\
    }

void VirtualMachine::interpret(std::vector<uint8_t>& codeVector, Writer & writer) {
    uint8_t* code = codeVector.data();
    std::size_t size = codeVector.size();

    std::size_t ip = 0;

    bool loop = true;

    Stack<Object> data;
    Heap heap;
    Stack<std::size_t>  callStack;

    Logger::debug(TAG, "Spawning GC thread");

    std::thread gcThread([&data, &heap, &loop]() {
        GarbageCollector::run(data, heap, loop);
    });

    Logger::debug(TAG, "GC thread spawned");

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
                Object o = retrieveImmediate(code, ip);
                data.push(o);
                break;
            }
            case OpCode::Code::Pop: {
                Logger::debug(TAG, "Pop");
                data.pop();
                break;
            }
            case OpCode::Code::Copy: {
                Logger::debug(TAG, "Copy");
                Object value = data.top();
                Logger::debug(TAG, "Value: " + std::to_string(value.int64));
                data.push(value);
                break;
            }
            case OpCode::Code::PrintInteger: {
                Logger::debug(TAG, "PrintInteger");
                Object o = data.pop();
                writer.writeInteger(o.int64);
                break;
            }
            case OpCode::Code::PrintFloat: {
                Logger::debug(TAG, "PrintFloat");
                Object o = data.pop();
                writer.writeDouble(o.float64);
                break;
            }
            case OpCode::Code::PrintLine: {
                Logger::debug(TAG, "PrintLine");
                writer.writeLine();
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
                Object o = data.pop();
                if (!o.int64) {
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
            case OpCode::Code::IntegerToFloat: {
                Logger::debug(TAG, "IntegerToFloat");
                Object o = data.pop();
                o.float64 = static_cast<double>(o.int64);
                Logger::debug(TAG, "Converted: " + std::to_string(o.float64));
                data.push(o);
                break;
            }
            case OpCode::Code::FloatToInteger: {
                Logger::debug(TAG, "FloatToInteger");
                Object o = data.pop();
                o.int64 = static_cast<int64_t>(o.float64);
                Logger::debug(TAG, "Converted: " + std::to_string(o.int64));
                data.push(o);
                break;
            }
            case OpCode::Code::LoadAddress: {
                Logger::debug(TAG, "LoadAddress");
                Object offset = data.pop();
                Object address = data.pop();
                Logger::debug(TAG, "Address: " + std::to_string(address.int64));
                Logger::debug(TAG, "Offset: " + std::to_string(offset.int64));
                HeapObject* obj = heap.getHeapObject(address);
                Object o = obj->get(offset.int64);
                Logger::debug(TAG, "Result: " + std::to_string(o.int64));
                data.push(o);
                break;
            }
            case OpCode::Code::StoreAddress: {
                Logger::debug(TAG, "StoreAddress");
                Object toStore = data.pop();
                Object offset = data.pop();
                Object address = data.pop();
                Logger::debug(TAG, "Address: " + std::to_string(address.int64));
                Logger::debug(TAG, "Offset: " + std::to_string(offset.int64));
                Logger::debug(TAG, "ToStore: " + std::to_string(toStore.int64));
                HeapObject* obj = heap.getHeapObject(address);
                obj->set(offset.int64, toStore);
                break;
            }
            case OpCode::Code::MarkAddressCollectable: {
                Logger::debug(TAG, "MarkAddressCollectable");
                Object address = data.pop();
                Logger::debug(TAG, "Address: " + std::to_string(address.int64));
                HeapObject* obj = heap.getHeapObject(address);
                obj->gcAllowed = true;
                break;
            }
            case OpCode::Code::New: {
                Logger::debug(TAG, "New");
                Object itemSize = data.pop();
                Logger::debug(TAG, "Size: " + std::to_string(itemSize.int64));
                HeapObject* obj = heap.newHeapObject(itemSize.int64);
                Object result{};
                result.ptr64 = obj;
                Logger::debug(TAG, "Created new heap object at " + std::to_string(result.int64));
                data.push(result);
                break;
            }
            binaryIntegerOpOnStack(OpCode::Code::Add, "Add", +)
            binaryIntegerOpOnStack(OpCode::Code::Subtract, "Subtract", -)
            binaryIntegerOpOnStack(OpCode::Code::Multiply, "Multiply", *)
            binaryIntegerOpOnStack(OpCode::Code::Divide, "Divide", /)
            binaryIntegerOpOnStack(OpCode::Code::Modulus, "Modulus", %)
            binaryIntegerOpOnStack(OpCode::Code::GreaterThanOrEqual, "GreaterThanOrEqual", >=)
            binaryIntegerOpOnStack(OpCode::Code::GreaterThan, "GreaterThan", >)
            binaryIntegerOpOnStack(OpCode::Code::LessThanOrEqual, "LessThanOrEqual", <=)
            binaryIntegerOpOnStack(OpCode::Code::LessThan, "LessThan", <)
            binaryIntegerOpOnStack(OpCode::Code::Equal, "Equal", ==)
            binaryIntegerOpOnStack(OpCode::Code::NotEqual, "NotEqual", !=)
            unaryIntegerOpOnStack(OpCode::Code::Not, "Not", !)
            unaryIntegerOpOnStack(OpCode::Code::Negate, "Negate", -)
            binaryFloatOpOnStack(OpCode::Code::FloatAdd, "FloatAdd", +)
            binaryFloatOpOnStack(OpCode::Code::FloatSubtract, "FloatSubtract", -)
            binaryFloatOpOnStack(OpCode::Code::FloatDivide, "FloatDivide", /)
            binaryFloatOpOnStack(OpCode::Code::FloatMultiply, "FloatMultiply", *)
            binaryFloatOpOnStackToBoolean(OpCode::Code::FloatLessThanOrEqual, "FloatLessThanOrEqual", <=)
            binaryFloatOpOnStackToBoolean(OpCode::Code::FloatLessThan, "FloatLessThan", <)
            binaryFloatOpOnStackToBoolean(OpCode::Code::FloatGreaterThan, "FloatGreaterThan", >)
            binaryFloatOpOnStackToBoolean(OpCode::Code::FloatGreaterThanOrEqual, "FloatGreaterThanOrEqual", >=)
            binaryFloatOpOnStackToBoolean(OpCode::Code::FloatNotEqual, "FloatNotEqual", !=)
            binaryFloatOpOnStackToBoolean(OpCode::Code::FloatEqual, "FloatEqual", ==)
            unaryFloatOpOnStack(OpCode::Code::FloatNegate, "FloatNegate", -)
            default: {
                panic_("Unknown opCode: " + std::to_string(currentByte));
            }
        }

        data.debugStack();
        heap.debugHeap();
    }

    Logger::debug(TAG, "Awaiting join of GC thread");

    gcThread.join();

    Logger::debug(TAG, "GC thread joined");
}