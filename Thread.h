
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include "uthreads.h"

enum State {READY, RUNNING, BLOCK};


class Thread
{

public:

    Thread(int tid, int pr, void (*f)(void), State state);

    ~Thread();


    void setState(State state);


    State getState() const;


    int getId() const;


    int getPriority() const;

    void setPriority(int pr);


    void incQuantums();


    int getQuantumsAmount() const;


    sigjmp_buf& getContext();





private:
    int _tid;
    int _pr;
    int _soFarQSycels;
    void (*_f)(void);
    State _state;
    char* _stack;
    sigjmp_buf _jmp_buf;
};