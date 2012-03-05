#ifndef PARALLEL_H
#define PARALLEL_H

#include <iostream>
#include <unistd.h>
#include <sys/io.h>

class Parallel{
    static unsigned char data;
    static unsigned char status;
    
    void setHighNibble(unsigned char);
    void setLowNibble(unsigned char);
    unsigned char getHighNibble();
    unsigned char getLowNibble();
    
    //status pins..
    void setStatus1();
    void setStatus2();
    void setStatus3();
    
    unsigned int readStatus1();
    unsigned int readStatus2();
    unsigned int readStatus3();

};

#endif //PARALLEL_H