//
// Created by Frank Dattalo on 6/1/18.
//

#include <regex>

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
        bool isZero = !strcmp("0", cstr);

        if (val == 0 && !isZero) {
            return false;
        }

        char buffer[50];

        sprintf(buffer, "%lld", val);

        bool sameAsOriginal = !strncmp(buffer, cstr, 50);

        return sameAsOriginal;
    }

    template<typename T>
    void append(ByteBuffer &buffer, T value, std::size_t &bufferIndex) {
        std::size_t size = sizeof(T);
        uint8_t *bytes = reinterpret_cast<uint8_t *>(&value);
        buffer.append(bytes, size);
        bufferIndex += size;
    }

    void resolveLocations(ByteBuffer &buffer,
                          std::map<std::string, std::size_t> &labelBufferLocations,
                          std::vector<Position> &unresolvedLocations,
                          std::vector<std::string> &tokens) {

        auto iter = unresolvedLocations.cbegin();
        auto end = unresolvedLocations.cend();

        for (; iter != end; ++iter) {
            std::string token = "@" + tokens[iter->tokensIndex];

            auto find = labelBufferLocations.find(token);
            if (find == labelBufferLocations.end()) {
                panic_("No declaration found for " + token);
            }

            std::size_t position = find->second;

            Logger::debug(TAG, "Resolved location of label " + token + " to location " + std::to_string(position));

            buffer.rewrite(reinterpret_cast<uint8_t *>(&position), sizeof(std::size_t), iter->bufferIndex);
        }
    }

    void handleLabelDeclaration(std::string &current,
                                std::map<std::string, std::size_t> &labelBufferLocations,
                                std::size_t bufferIndex) {

        auto find = labelBufferLocations.find(current);
        if (find != labelBufferLocations.end()) {
            panic_("Duplicate declaration of " + current);
        }

        labelBufferLocations[current] = bufferIndex;
    }

    void handleLabelUsage(std::string &current,
                          std::map<std::string, std::size_t> &labelBufferLocations,
                          ByteBuffer &buffer,
                          std::vector<Position> &unresolvedLocations,
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

        match(current, buffer, bufferIndex, found, "Add",                OpCode::Code::Add);
        match(current, buffer, bufferIndex, found, "Subtract",           OpCode::Code::Subtract);
        match(current, buffer, bufferIndex, found, "Divide",             OpCode::Code::Divide);
        match(current, buffer, bufferIndex, found, "Multiply",           OpCode::Code::Multiply);
        match(current, buffer, bufferIndex, found, "Modulus",            OpCode::Code::Modulus);
        match(current, buffer, bufferIndex, found, "Push",               OpCode::Code::Push);
        match(current, buffer, bufferIndex, found, "Pop",                OpCode::Code::Pop);
        match(current, buffer, bufferIndex, found, "PrintInteger",       OpCode::Code::PrintInteger);
        match(current, buffer, bufferIndex, found, "PrintLine",          OpCode::Code::PrintLine);
        match(current, buffer, bufferIndex, found, "Not",                OpCode::Code::Not);
        match(current, buffer, bufferIndex, found, "Negate",             OpCode::Code::Negate);
        match(current, buffer, bufferIndex, found, "LessThanOrEqual",    OpCode::Code::LessThanOrEqual);
        match(current, buffer, bufferIndex, found, "LessThan",           OpCode::Code::LessThan);
        match(current, buffer, bufferIndex, found, "GreaterThan",        OpCode::Code::GreaterThan);
        match(current, buffer, bufferIndex, found, "GreaterThanOrEqual", OpCode::Code::GreaterThanOrEqual);
        match(current, buffer, bufferIndex, found, "NotEqual",           OpCode::Code::NotEqual);
        match(current, buffer, bufferIndex, found, "Equal",              OpCode::Code::Equal);
        match(current, buffer, bufferIndex, found, "Jump",               OpCode::Code::Jump);
        match(current, buffer, bufferIndex, found, "JumpZero",           OpCode::Code::JumpZero);
        match(current, buffer, bufferIndex, found, "Copy",               OpCode::Code::Copy);
        match(current, buffer, bufferIndex, found, "Call",               OpCode::Code::Call);
        match(current, buffer, bufferIndex, found, "Return",             OpCode::Code::Return);
        match(current, buffer, bufferIndex, found, "End",                OpCode::Code::End);

        if (!found) {
            panic_("No assembler rule for input: " + current);
        }
    }

}

void Assembler::assemble(const std::string & input, std::vector<uint8_t> & out) {

    ByteBuffer buffer(out);
    std::size_t bufferIndex = 0;
    std::vector<std::string> tokens;
    std::map<std::string, std::size_t> labelBufferLocations;
    std::vector<Position> unresolvedLocations;

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

        } else {
            handleStaticStringOrError(current, buffer, bufferIndex);
        }

        buffer.debugBytes();
    }

    Logger::debug(TAG, "Resolving forward declarations");
    buffer.debugBytes();
    resolveLocations(buffer, labelBufferLocations, unresolvedLocations, tokens);
    buffer.debugBytes();

    out = buffer.vector();
}

