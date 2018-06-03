//
// Created by Frank Dattalo on 6/2/18.
//

#ifndef FVM_STACK_H
#define FVM_STACK_H

#include "Assert.h"
#include "Logger.h"

namespace {
    const std::string TAG ("Stack");
}

template<typename T>
class Stack {
private:
    std::vector<T> data;
    int64_t _top;
public:
    Stack() : _top(0) {}

    void push(T t) {
        this->_top++;
        this->data.push_back(t);
    }

    T pop() {
        assert_(this->_top > 0, "Pop called on an empty stack!");

        T ret = this->data[this->_top - 1];
        this->_top--;
        this->data.pop_back();

        return ret;
    }

    T top() {
        assert_(this->_top > 0, "Top called on an empty stack!");

        T ret = this->data[this->_top - 1];
        return ret;
    }

    void debugStack() {

        auto stream = Logger::debugStream();

        stream << std::endl;

        if (_top == 0) {
            stream << "  [~]" << std::endl;
        } else {
            for (auto i = _top - 1; i >= 0; i--) {
                stream << "  [" << i << "] - " << this->data[i] << std::endl;
            }
        }

        Logger::debug(TAG, stream);
    }
};


#endif //FVM_STACK_H
