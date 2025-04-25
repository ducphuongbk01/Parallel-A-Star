#ifndef __SET_HPP__
#define __SET_HPP__

#include "dependency.hpp"

class Set {
    private:
        std::mutex mtx;
        std::set<long> obj;

    public:
        void insert(long id) {
            std::lock_guard<std::mutex> lock(mtx);
            this->obj.insert(id);
        }

        bool contain(long id) {
            std::lock_guard<std::mutex> lock(mtx);
            return this->obj.find(id) != this->obj.end();
        }
};

#endif