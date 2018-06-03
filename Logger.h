//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_LOGGER_H
#define FVM_LOGGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace Logger {

    void setDebug(bool debug);

    void debug(const std::string & tag, const std::string & string);

    std::stringstream debugStream();

    void debug(const std::string & tag, const std::stringstream& stringstream);
};

#endif //FVM_LOGGER_H
