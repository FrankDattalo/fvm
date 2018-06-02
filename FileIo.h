//
// Created by Frank Dattalo on 6/2/18.
//

#ifndef FVM_FILEIO_H
#define FVM_FILEIO_H

#include <string>
#include <vector>
#include <fstream>

namespace FileIo {

    std::string readFileToString(std::string fileName);

    std::vector<uint8_t> readFileToVector(std::string fileName);

    void writeFileContents(std::vector<uint8_t> contents, std::string fileName);
};


#endif //FVM_FILEIO_H
