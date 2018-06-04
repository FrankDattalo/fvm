//
// Created by Frank Dattalo on 6/1/18.
//

#include "Logger.h"

namespace {
    bool LoggerDebug = false;
    std::mutex lock;
}

void Logger::setDebug(bool debug) {
    LoggerDebug = debug;
}

void Logger::debug(const std::string & tag, const std::string & string) {
    if (LoggerDebug) {
        std::lock_guard<std::mutex> guard(lock);

        std::cout << "[DEBUG]-[" << std::setw(20) << tag << "]-[" << string << "]" << std::endl;
    }
}

std::stringstream Logger::debugStream() {
    return std::stringstream();
}

void Logger::debug(const std::string & tag, const std::stringstream& stringstream) {
    Logger::debug(tag, stringstream.str());
}
