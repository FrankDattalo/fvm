//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_PANIC_H
#define FVM_PANIC_H

#include <string>
#include <iostream>

#define RUNTIME_ASSERT(val, reason) do { \
    if (!(val)) { \
        std::cout << (reason) << std::endl; \
        exit(1); \
    } \
} while (false)


#endif //FVM_PANIC_H
