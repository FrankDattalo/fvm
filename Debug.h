//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_LOGGER_H
#define FVM_LOGGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#define ENABLE_DEBUG_1

#ifdef ENABLE_DEBUG
#define DEBUG(TAG, X) Logger::logDebug(TAG, X)
#define LOG_BEGIN()  std::stringstream __DEBUG_LOG_STREAM
#define LOG_STREAM(X) __DEBUG_LOG_STREAM << X
#define LOG_END(TAG) Logger::logWithStream(TAG, __DEBUG_LOG_STREAM)
#define IF_DEBUG(...) __VA_ARGS__
#else
#define DEBUG(TAG, X) /* disabled */
#define LOG_BEGIN() /* disabled */
#define LOG_STREAM(X) /* disabled */
#define LOG_END(TAG) /* disabled */
#define IF_DEBUG(...) /* disabled */
#endif

IF_DEBUG(namespace Logger {

    void logDebug(const std::string & tag, const std::string & string);

    void logWithStream(const std::string & tag, const std::stringstream & stream);
});

#endif //FVM_LOGGER_H
