//
// Created by Frank Dattalo on 6/1/18.
//

#include "Assert.h"
#include "Panic.h"

void assert_(bool mustBeTrue, const std::string & errorMessage) {
    if (!mustBeTrue) {
        panic_(errorMessage);
    }
}
