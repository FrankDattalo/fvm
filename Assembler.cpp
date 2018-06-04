//
// Created by Frank Dattalo on 6/1/18.
//

#include <regex>
#include <list>

#include "Assembler.h"
#include "ByteBuffer.h"
#include "Panic.h"
#include "OpCode.h"

namespace {
    const std::string TAG ("Assembler");

    struct Position {
        std::size_t bufferIndex;
        std::size_t tokensIndex;
    };


    void tokenize(const std::string &input, std::vector<std::string> & ret) {

        std::regex regex("(\\s+|//.*)");

        std::sregex_token_iterator iterator(input.begin(), input.end(), regex, -1);
        std::sregex_token_iterator end;

        for (; iterator != end; ++iterator) {
            std::string current = *iterator;
            ret.push_back(current);
        }
    }

    bool isInteger(const std::string &str) {
        const char *cstr = str.c_str();

        int64_t val = atoll(cstr);
        bool isZero = !strcmp("0", cstr) || !strcmp("-0", cstr);

        if (val == 0 && !isZero) {
            return false;
        }

        char buffer[50];

        sprintf(buffer, "%lld", val);

        bool sameAsOriginal = !strncmp(buffer, cstr, 50);

        return sameAsOriginal;
    }

    bool floatEquals(double left, double right) {

        static double epsilion = 1e-8;

        double diff = left - right;

        diff = diff < 0 ? -diff : diff;

        return diff < epsilion;
    }

    bool isFloat(const std::string & str) {
        const char *cstr = str.c_str();
        double val = atof(cstr);

        bool isZero = !strcmp("0.0", cstr) || !strcmp("-0.0", cstr);

        return !(floatEquals(val, 0) && !isZero);

    }

    template<typename T>
    void append(ByteBuffer &buffer, T value, std::size_t &bufferIndex) {
        std::size_t size = sizeof(T);
        uint8_t *bytes = reinterpret_cast<uint8_t *>(&value);
        buffer.append(bytes, size);
        bufferIndex += size;
    }

    void resolveLocations(ByteBuffer &buffer,
                          std::unordered_map<std::string, std::size_t> &labelBufferLocations,
                          std::list<Position> &unresolvedLocations,
                          std::vector<std::string> &tokens) {

        for (const Position& pos : unresolvedLocations) {
            std::string token = "@" + tokens[pos.tokensIndex];

            auto find = labelBufferLocations.find(token);
            if (find == labelBufferLocations.end()) {
                panic_("No declaration found for " + token);
            }

            std::size_t position = find->second;

            Logger::debug(TAG, "Resolved location of label " + token + " to location " + std::to_string(position));

            buffer.rewrite(reinterpret_cast<uint8_t *>(&position), sizeof(std::size_t), pos.bufferIndex);
        }
    }

    void handleLabelDeclaration(std::string &current,
                                std::unordered_map<std::string, std::size_t> &labelBufferLocations,
                                std::size_t bufferIndex) {

        auto find = labelBufferLocations.find(current);
        if (find != labelBufferLocations.end()) {
            panic_("Duplicate declaration of " + current);
        }

        labelBufferLocations[current] = bufferIndex;
    }

    void handleLabelUsage(std::string &current,
                          std::unordered_map<std::string, std::size_t> &labelBufferLocations,
                          ByteBuffer &buffer,
                          std::list<Position> &unresolvedLocations,
                          std::size_t& bufferIndex,
                          std::size_t tokensIndex) {

        std::size_t position = std::numeric_limits<std::size_t>::max();

        std::string fullLabel = "@" + current;

        auto find = labelBufferLocations.find(fullLabel);
        if (find != labelBufferLocations.end()) {

            Logger::debug(TAG, "Declared location: " + current);
            position = find->second;
        } else {

            Logger::debug(TAG, "Unresolved location: " + current);

            Position p { bufferIndex, tokensIndex};
            unresolvedLocations.push_back(p);
        }

        append<std::size_t>(buffer, position, bufferIndex);
    }

    void handleIntegerUsage(std::string &current,
                            ByteBuffer &buffer,
                            std::size_t& bufferIndex) {

        int64_t val = atoll(current.c_str());
        append<int64_t>(buffer, val, bufferIndex);
    }

    void handleFloatUsage(std::string &current,
                          ByteBuffer &buffer,
                          std::size_t& bufferIndex) {

        double val = atof(current.c_str());
        append<double>(buffer, val, bufferIndex);
    }

    void match(const std::string &current,
               ByteBuffer &buffer,
               std::size_t& bufferIndex,
               bool &found,
               const std::string& str,
               OpCode::Code opcode) {

        if (current == str) {

            Logger::debug(TAG, "Matched " + str);

            if (found) {
                panic_("Multiple matching rules for " + current);
            }

            uint8_t  byte = OpCode::getByte(opcode);

            append<uint8_t>(buffer, byte, bufferIndex);

            found = true;
        }
    }

    void handleStaticStringOrError(std::string &current,
                                   ByteBuffer &buffer,
                                   std::size_t& bufferIndex) {

        bool found = false;

        match(current, buffer, bufferIndex, found, "Add",                           OpCode::Code::Add);
        match(current, buffer, bufferIndex, found, "Subtract",                      OpCode::Code::Subtract);
        match(current, buffer, bufferIndex, found, "Divide",                        OpCode::Code::Divide);
        match(current, buffer, bufferIndex, found, "Multiply",                      OpCode::Code::Multiply);
        match(current, buffer, bufferIndex, found, "Modulus",                       OpCode::Code::Modulus);
        match(current, buffer, bufferIndex, found, "Push",                          OpCode::Code::Push);
        match(current, buffer, bufferIndex, found, "Pop",                           OpCode::Code::Pop);
        match(current, buffer, bufferIndex, found, "PrintInteger",                  OpCode::Code::PrintInteger);
        match(current, buffer, bufferIndex, found, "PrintLine",                     OpCode::Code::PrintLine);
        match(current, buffer, bufferIndex, found, "Not",                           OpCode::Code::Not);
        match(current, buffer, bufferIndex, found, "Negate",                        OpCode::Code::Negate);
        match(current, buffer, bufferIndex, found, "LessThanOrEqual",               OpCode::Code::LessThanOrEqual);
        match(current, buffer, bufferIndex, found, "LessThan",                      OpCode::Code::LessThan);
        match(current, buffer, bufferIndex, found, "GreaterThan",                   OpCode::Code::GreaterThan);
        match(current, buffer, bufferIndex, found, "GreaterThanOrEqual",            OpCode::Code::GreaterThanOrEqual);
        match(current, buffer, bufferIndex, found, "NotEqual",                      OpCode::Code::NotEqual);
        match(current, buffer, bufferIndex, found, "Equal",                         OpCode::Code::Equal);
        match(current, buffer, bufferIndex, found, "Jump",                          OpCode::Code::Jump);
        match(current, buffer, bufferIndex, found, "JumpZero",                      OpCode::Code::JumpZero);
        match(current, buffer, bufferIndex, found, "Copy",                          OpCode::Code::Copy);
        match(current, buffer, bufferIndex, found, "Call",                          OpCode::Code::Call);
        match(current, buffer, bufferIndex, found, "Return",                        OpCode::Code::Return);
        match(current, buffer, bufferIndex, found, "End",                           OpCode::Code::End);
        match(current, buffer, bufferIndex, found, "FloatAdd",                      OpCode::Code::FloatAdd);
        match(current, buffer, bufferIndex, found, "FloatSubtract",                 OpCode::Code::FloatSubtract);
        match(current, buffer, bufferIndex, found, "FloatDivide",                   OpCode::Code::FloatDivide);
        match(current, buffer, bufferIndex, found, "FloatMultiply",                 OpCode::Code::FloatMultiply);
        match(current, buffer, bufferIndex, found, "PrintFloat",                    OpCode::Code::PrintFloat);
        match(current, buffer, bufferIndex, found, "FloatNegate",                   OpCode::Code::FloatNegate);
        match(current, buffer, bufferIndex, found, "FloatLessThanOrEqual",          OpCode::Code::FloatLessThanOrEqual);
        match(current, buffer, bufferIndex, found, "FloatLessThan",                 OpCode::Code::FloatLessThan);
        match(current, buffer, bufferIndex, found, "FloatGreaterThan",              OpCode::Code::FloatGreaterThan);
        match(current, buffer, bufferIndex, found, "FloatGreaterThanOrEqual",       OpCode::Code::FloatGreaterThanOrEqual);
        match(current, buffer, bufferIndex, found, "FloatNotEqual",                 OpCode::Code::FloatNotEqual);
        match(current, buffer, bufferIndex, found, "FloatEqual",                    OpCode::Code::FloatEqual);
        match(current, buffer, bufferIndex, found, "IntegerToFloat",                OpCode::Code::IntegerToFloat);
        match(current, buffer, bufferIndex, found, "FloatToInteger",                OpCode::Code::FloatToInteger);
        match(current, buffer, bufferIndex, found, "LoadAddress",                   OpCode::Code::LoadAddress);
        match(current, buffer, bufferIndex, found, "StoreAddress",                  OpCode::Code::StoreAddress);
        match(current, buffer, bufferIndex, found, "MarkAddressCollectable",        OpCode::Code::MarkAddressCollectable);
        match(current, buffer, bufferIndex, found, "New",                           OpCode::Code::New);

        if (!found) {
            panic_("No assembler rule for input: " + current);
        }
    }

}

void Assembler::assemble(const std::string & input, std::vector<uint8_t> & out) {

    ByteBuffer buffer(out);
    std::size_t bufferIndex = 0;
    std::vector<std::string> tokens;
    std::unordered_map<std::string, std::size_t> labelBufferLocations;
    std::list<Position> unresolvedLocations;

    tokenize(input, tokens);

    std::size_t size = tokens.size();
    for (std::size_t tokensIndex = 0; tokensIndex < size; tokensIndex++) {

        std::string current = tokens[tokensIndex];
        if (current.length() == 0) {
            Logger::debug(TAG, "Skipping empty string");
            continue;
        }

        if (!strncmp(current.c_str(), "@@", 2)) {
            Logger::debug(TAG, "Label definition found: " + current);
            handleLabelDeclaration(current, labelBufferLocations, bufferIndex);

        } else if (!strncmp(current.c_str(), "@", 1)) {
            Logger::debug(TAG, "Label usage found: " + current);
            handleLabelUsage(current, labelBufferLocations, buffer, unresolvedLocations, bufferIndex, tokensIndex);

        } else if (isInteger(current)) {
            Logger::debug(TAG, "Integer usage found: " + current);
            handleIntegerUsage(current, buffer, bufferIndex);

        } else if (isFloat(current)) {
            Logger::debug(TAG, "Float usage found: " + current);
            handleFloatUsage(current, buffer, bufferIndex);

        } else {
            handleStaticStringOrError(current, buffer, bufferIndex);
        }

        buffer.debugBytes();
    }

    Logger::debug(TAG, "Resolving forward declarations");
    buffer.debugBytes();
    resolveLocations(buffer, labelBufferLocations, unresolvedLocations, tokens);
    buffer.debugBytes();
}

