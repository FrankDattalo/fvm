//
// Created by Frank Dattalo on 6/1/18.
//

#ifndef FVM_ASSERT_H
#define FVM_ASSERT_H

#include "Panic.h"

#define ENABLE_ASSERT

#ifdef ENABLE_ASSERT
#define ASSERT(b, s) RUNTIME_ASSERT(b, s)
#else
#define ASSERT(b, s) /* assert is disabled */
#endif

#endif //FVM_ASSERT_H
