//
// Created by Frank Dattalo on 6/1/18.
//

#include "Panic.h"

void panic_(std::string reason) {
    std::cerr << reason << std::endl;
    exit(1);
}