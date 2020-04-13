#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include "uthreads.h"
enum State {READY, RUNNING, BLOCKED};
class Thread
{
public:
    Thread(int id,int priority,void (*f)(void),State state);
    ~Thread();
    void setState(State state);
    [[nodiscard]] State  getState() const;
    [[nodiscard]] int getId() const;
    [[nodiscard]] int getPriority() const;
    [[nodiscard]] int getCurQuantum() const;
    sigjmp_buf& getContext();
    void setTimer(int quantum);
    struct itimerval getTimer();
    void updateQuantum();
    void setPriority(int pr);

    // sigsetjmp(Thread.getContext,1) == saveBuff
    // siglongjmo(Thread.getContext,1) == loadBuff

private:
    struct itimerval _timer;
    int _id;
    int _pr;
    int _quantum;
    void (*_f)(void);
    State _state;
    char* _stack;
    sigjmp_buf _context;
};


#endif //EX2_THREAD_H
