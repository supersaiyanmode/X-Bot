#include <time.h>
#include <iostream>
#include <algorithm>
#include "Thread.h"

template <typename T, typename P>
Thread<T,P>::Thread(T& obj, void (T::*fn)(P), P arg):object(obj),function(fn),argument(arg){
    
}

template <typename T, typename P>
#ifdef WIN32
DWORD __stdcall Thread<T,P>::createThread(void *arg){
#else
void* Thread<T,P>::createThread(void *arg){
#endif
    Thread<T,P>& threadt = *(Thread<T,P>*)(arg);
    T& obj = threadt.object;
    P args = threadt.argument;
    void (T::*fn)(P);
    fn = threadt.function;
    delete &threadt;
    (obj.*fn)(args);
    return 0;
}

template <typename T, typename P>
void Thread<T,P>::start(){
    std::cout<<"Before!"<<std::endl;
    Thread<T,P>* obj = new Thread<T,P>(*this);
#if WIN32
    thread = CreateThread(0, 0, createThread, (void*)obj, 0, 0);
#else
    pthread_create(&thread, NULL, createThread, (void*)obj);
#endif
}

template <typename T, typename P>
void Thread<T,P>::sleep(int milli){
#ifdef WIN32
    Sleep(milli);
#else
    timespec ts = {milli/1000, (milli%1000)*1000000};
    nanosleep(&ts,NULL);
#endif
}

template <typename T, typename P>
void Thread<T,P>::join(){
#if WIN32
    ::WaitForSingleObject(thread, INFINITE);
#else
    pthread_join(thread,NULL);
#endif
}

template <typename T, typename P>
void Thread<T,P>::detach(){
#if WIN32
    ::TerminateThread(thread, 0);
#else
    pthread_detach(thread);
#endif
}

#if 0
#include <iostream>

class Temp{
public:
    void function(int arg){
        std::cout<<"Argument: "<<arg<<std::endl;
    }
};

int main(){
    Temp t;
    int x = 121;
    Thread<Temp,int> tt(t, &Temp::function,x);
    
    tt.start();
    tt.sleep(1000);
    return 0;
}

#endif
