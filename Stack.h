//
// Created by Frank Dattalo on 6/2/18.
//

#ifndef FVM_STACK_H
#define FVM_STACK_H

#include <vector>
#include "Assert.h"
#include "Logger.h"

template<typename T>
class Stack {
private:
    std::vector<T> data;
    int64_t _top;
    std::mutex lock;
public:
    Stack() : _top(0) {}

    void push(T t) {
        std::lock_guard<std::mutex> guard(lock);

        this->_top++;
        this->data.push_back(t);
    }

    T pop() {
        std::lock_guard<std::mutex> guard(lock);

        assert_(this->_top > 0, "Pop called on an empty stack!");

        T ret = this->data[this->_top - 1];
        this->_top--;
        this->data.pop_back();

        return ret;
    }

    T top() {
        std::lock_guard<std::mutex> guard(lock);

        assert_(this->_top > 0, "Top called on an empty stack!");

        T ret = this->data[this->_top - 1];
        return ret;
    }

    bool at(size_t index, T& out) {
        std::lock_guard<std::mutex> guard(lock);

        size_t size = this->data.size();

        if (index >= size) {
            return false;
        }

        out = this->data[index];

        return true;
    }

    size_t size() {
        std::lock_guard<std::mutex> guard(lock);
        return this->data.size();
    }

    void debugStack() {

        auto stream = Logger::debugStream();

        stream << std::endl;

        if (_top == 0) {
            stream << "  [~]" << std::endl;
        } else {
            for (auto i = _top - 1; i >= 0; i--) {
                stream << "  [" << i << "] - " << this->data[i].int64 << std::endl;
            }
        }

        Logger::debug("Stack", stream);
    }
};


#endif //FVM_STACK_H
