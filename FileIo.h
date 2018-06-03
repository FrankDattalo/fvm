//
// Created by Frank Dattalo on 6/2/18.
//

#ifndef FVM_FILEIO_H
#define FVM_FILEIO_H

#include <string>
#include <vector>
#include <fstream>

namespace FileIo {

    void readFileToString(const std::string & fileName, std::string & outContents);

    void readFileToVector(const std::string & fileName, std::vector<uint8_t> & outContents);

    void writeFileContents(const std::vector<uint8_t> & contents, const std::string & fileName);

};


#endif //FVM_FILEIO_H
