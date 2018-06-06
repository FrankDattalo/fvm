//
// Created by Frank Dattalo on 6/1/18.
//

#include "Debug.h"
IF_DEBUG(
namespace {
    std::mutex _lock;
}

void Logger::logDebug(const std::string & tag, const std::string & string) {
    std::lock_guard<std::mutex> guard(_lock);

    std::cout << "[DEBUG]-[" << std::setw(20) << tag << "]-[" << string << "]" << std::endl;
}

void Logger::logWithStream(const std::string & tag, const std::stringstream & stream) {
    Logger::logDebug(tag, stream.str());
}
)