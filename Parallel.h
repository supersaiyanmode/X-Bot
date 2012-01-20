#ifndef PARALLEL_H
#define PARALLEL_H

#include <iostream>
#include <unistd.h>
#include <sys/io.h>

class Parallel{
    static unsigned char data;
    static unsigned char status;
    
    unsigned int arr[8], stat[8];
    
    unsigned int& operator[](int);
    const unsigned int& operator[](int) const;
    unsigned int& operator()(int);
    const unsigned int& operator()(int) const;
};

#endif //PARALLEL_H