//
// Created by Frank Dattalo on 6/1/18.
//

#include "Logger.h"

namespace {
    bool LoggerDebug = false;
}

void Logger::setDebug(bool debug) {
    LoggerDebug = debug;
}

void Logger::debug(std::string tag, std::string string) {
    if (LoggerDebug) {
        std::cout << "[DEBUG]-[" << std::setw(10) << tag << "]-[" << string << "]" << std::endl;
    }
}

std::stringstream Logger::debugStream() {
    return std::stringstream();
}

void Logger::debug(std::string tag, const std::stringstream& stringstream) {
    Logger::debug(tag, stringstream.str());
}
