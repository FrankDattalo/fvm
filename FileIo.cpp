//
// Created by Frank Dattalo on 6/2/18.
//

#include "FileIo.h"
#include "Panic.h"
#include "Logger.h"

namespace {
    std::string TAG = "FileIo";
}

std::string FileIo::readFileToString(std::string fileName) {
    std::ifstream input;

    Logger::debug(TAG, "Trying to open: " + fileName);

    input.open(fileName);

    if (!input) {
        panic_("Could not open file " + fileName);
    }

    input.seekg(0, input.end);
    int64_t size = input.tellg();
    Logger::debug(TAG, "File size: " + std::to_string(size));
    input.seekg(0, input.beg);

    char buffer[size + 1];
    input.read(buffer, size);
    buffer[size] = '\0';

    if (!input) {
        panic_("Could not real whole file " + fileName);
    }

    Logger::debug(TAG, "Read file: " + fileName);

    std::string ret(buffer);

    input.close();

    return ret;
}

std::vector<uint8_t> FileIo::readFileToVector(std::string fileName) {

    std::ifstream input;

    Logger::debug(TAG, "Trying to open: " + fileName);

    input.open(fileName);

    if (!input) {
        panic_("Could not open file " + fileName);
    }

    input.seekg(0, input.end);
    int64_t size = input.tellg();
    Logger::debug(TAG, "File size: " + std::to_string(size));
    input.seekg(0, input.beg);

    char buffer[size];
    input.read(buffer, size);

    if (!input) {
        panic_("Could not real whole file " + fileName);
    }

    Logger::debug(TAG, "Read file: " + fileName);

    input.close();

    std::vector<uint8_t> ret;

    for (std::size_t i = 0; i < size; i++) {
        uint8_t byte = static_cast<uint8_t>(buffer[i]);
        ret.push_back(byte);
    }

    return ret;
}

void FileIo::writeFileContents(std::vector<uint8_t> contents, std::string fileName) {

    std::ofstream output;

    Logger::debug(TAG, "Trying to write to " + fileName);

    output.open(fileName);

    if (!output) {
        panic_("Could not open " + fileName);
    }

    output.write(reinterpret_cast<const char *>(contents.data()), contents.size());

    Logger::debug(TAG, "Wrote to " + fileName);

    output.close();
}
