#ifndef __BASE_HPP__
#define __BASE_HPP__

#include <iostream>
#include <shared_mutex>
#include <omp.h>

class OpenMPLock {
    private:
        omp_lock_t lock;
    
    public:
        OpenMPLock() {
            omp_init_lock(&lock);
        };

        ~OpenMPLock() {
            omp_destroy_lock(&lock);
        };

        void acquire() {
            omp_set_lock(&lock);
        };

        void release() {
            omp_unset_lock(&lock);
        };
};

// class Set {
//     public:     
//         virtual bool contains(float val);
//         virtual bool add(float val);
//         virtual bool remove(float val);
// };


#endif // __BASE_HPP__