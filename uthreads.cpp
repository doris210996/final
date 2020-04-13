#include <iostream>
#include "uthreads.h"
#include "Thread.h"
#include <queue>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <cmath>
#include <map>
#include <algorithm>
#include <list>
#define INVALID_QUANTUM_VAL 0
#define INVALID_PRIOR_NUM 1
#define SIGACTION_ERROR 2
#define TIME_OUT 0
#define SET_TIME_ERROR 3
#define ERROR_PREFIX "thread library error: "
static std::vector<int> _quantums;
static std::list<Thread*> _ready;
static std::list<Thread*> _blocked;
static std::map<int, Thread*> _threads;
static Thread* _running;
static int _priorityAmount;
static struct sigaction _sigAction = {0};
static int _qCounter;
static struct itimerval _timer;


int flag = 0;


static void printErrors(int type){
    switch(type)
    {
        case INVALID_QUANTUM_VAL:
        {
            std::cerr << ERROR_PREFIX << "Invalid value for a quantum!";
        }

        case INVALID_PRIOR_NUM:
        {
            std::cerr << ERROR_PREFIX << "Invalid value for priority amount!";
        }

        case SIGACTION_ERROR:
        {
            std::cerr << ERROR_PREFIX << "Sigaction raised an unacceptable error!";
        }

        case SET_TIME_ERROR:
        {
            std::cerr << ERROR_PREFIX << "Set time error!";
        }
        default:
            break;
    }
}



int nextId(){
    return _threads.begin()->first; // since a map is sorted by keys
}

static void pushReady(Thread* th)
{
    if (th == nullptr)
    {
        return;
    }
    _ready.push_back(th);
}

static Thread* popReady(){
    Thread* ret = _ready.front();
    _ready.pop_front();
    return ret;
}

int removeReady(int id){
    for(Thread* th: _ready)
    {
        if(th->getId()==id)
        {
            _ready.remove(_threads.at(id));
            return 0;
        }
    }
    return -1;
}

int removeBlocked(int id){
    for(Thread* th: _blocked)
    {
        if(th->getId()==id)
        {
            _blocked.remove(_threads.at(id));
            return 0;
        }
    }
    return -1;

}
// This function starts the virtual timer (of the chosen thread)
static void startTimer(){
    if(flag == 0)

    if (setitimer(ITIMER_VIRTUAL, &_timer, NULL))
    {
        printErrors(SET_TIME_ERROR);
        exit(1);
    }
}




void runThread()
{
    Thread* tmp = popReady();
    if(tmp != nullptr)   //that's mean that not only one thread can run now!
    {
        _running = tmp;
    }
    _running->setState(RUNNING);
    _running->updateQuantum();
    _qCounter++;
    _timer = _running->getTimer();
    startTimer();
}

void switchThreads()
{
    Thread *tmp = _running;
    if(tmp != nullptr)   // if sent from suspend, so running is nullptr
    {
        tmp->setState(READY);
    }
    runThread();
    // if there are x threads in _threads and x + 1 threads in blocked, than it means that there is nothing
    // in ready, so the current running thread should not get into ready and keep running.
    if(_threads.size() - 1 != _blocked.size())
    {
        pushReady(tmp);
    }

}
static void timerHandler(int sig){
    flag = 1;
    printf("time expired\n");
    // Checks if the value is zero - so it means we are here because of time out ,and not as a
    // part of a running thread only in that case we should look for switching
    bool realTimeOut = sigsetjmp(_running->getContext(), 1) == TIME_OUT;
    if (realTimeOut)
    {
        switchThreads();
        siglongjmp(_running->getContext(),1); // Note: here _running is now maybe another one
        // if we indeed switched in the above function so we now will jump to its own function
    }

}

int uthread_init(int *quantum_usecs, int size){
    if(size < 1)
    {
        printErrors(INVALID_PRIOR_NUM);
        return -1;
    }
    for(int i=0; i<size; i++)
    {
        if(quantum_usecs[i] <= 0)
        {
            printErrors(INVALID_QUANTUM_VAL);
            return -1;
        }
    }


    // Change the default behave of SIGVTALRM to call our function
    _sigAction.sa_handler = &timerHandler;
    if(sigaction(SIGVTALRM,&_sigAction,NULL) < 0)
    {
        printErrors(SIGACTION_ERROR);
        return -1;
    }
    _quantums = std::vector<int> (quantum_usecs, quantum_usecs + size);
    std::sort(_quantums.rbegin(), _quantums.rend()); // reversed order, From now we will refer to
    // priority kth quantum
    // as the value of the kth index (when priority 0 which considered highest -has the longest
    // time)
    _priorityAmount = size; // save the number of priors.

    // set timer for each thread when create it except the main default who gets the longest
    Thread* mainDefault = new Thread(0,HIGHEST_PRIORITY, nullptr, READY);
    mainDefault->setTimer(_quantums[HIGHEST_PRIORITY]);
    _threads.insert(std::pair<int, Thread*>(0, mainDefault));
    _running = mainDefault;
    mainDefault->setState(RUNNING);
    mainDefault->updateQuantum();
    _qCounter++;
    // The default main timer starts running
    _timer = _running->getTimer();
    startTimer();
    return 0;
}

int uthread_get_total_quantums(){
    return _qCounter;
}

int uthread_spawn(void (*f)(void), int priority){
    //can't add
    if(_threads.size() != MAX_THREAD_NUM)
    {
        int id = nextId();
        Thread *th = new Thread(id, priority, f, READY);
        th->setTimer(_quantums[priority]);
        _threads.insert(std::pair<int, Thread *>(id, th));
        pushReady(th);
        return id;
    }
    //printErrors
    return -1;
}

int uthread_change_priority(int tid, int priority){
    if(!_threads.count(tid))
    {
        //printErrors
        return -1;
    }
    _threads[tid]->setPriority(priority);
    _threads[tid]->setTimer(_quantums[priority]);
    return 0;
}
int uthread_get_tid(){
    return _running->getId();
}

int uthread_get_quantums(int tid){
    sigprocmask(SIG_BLOCK, &_sigAction.sa_mask, nullptr);
    if (_threads.count(tid))
    {
        int num = _threads.at(tid)->getCurQuantum();
        sigprocmask(SIG_UNBLOCK, &_sigAction.sa_mask, nullptr);
        return num;
    }
    //printErrors
    return -1;
}

int uthread_terminate(int tid){
    if(tid == 0) // The main th
    {
        exit(0);
    }

    if(!_threads.count(tid)) // not exist
    {
        //printError
        return -1;
    }

    if (tid == _running->getId())
    {
        // Make sure the th with the given id will never run again
        if (_threads.at(tid)->getState() == READY) // in _ready list
        {
            removeReady(tid);
            _threads.erase(tid);
        }

        if (_threads.at(tid)->getState() == BLOCKED) // in _blocked list
        {
            removeBlocked(tid);
            _threads.erase(tid);
        }

        // Switch to the next one
        switchThreads();

        // Start running the next one
        siglongjmp(_running->getContext(),1);
    }

    if (_threads.at(tid)->getState() == READY) // in _ready list
    {
        removeReady(tid);
        _threads.erase(tid);
    }

    if (_threads.at(tid)->getState() == BLOCKED) // in _blocked list
    {
        removeBlocked(tid);
        _threads.erase(tid);
    }
    return 0;


    
}







void f(){
    if(flag)
    {
        printf("hi");
    }
    flag = 0;
}
int main(void) {
    int x[] = {1,2,3};
    uthread_init(x,3);
    uthread_spawn(f,1);



//    // Install timer_handlr as the signal handler for SIGVTALRM.
//    _sigAction.sa_handler = &timerHandler;
//    if (sigaction(SIGVTALRM, &_sigAction,NULL) < 0) {
//        printf("sigaction error.");
//    }
//
//    // Configure the timer to expire after 1 sec... */
//    _timer.it_value.tv_sec = 1;		// first time interval, seconds part
//    _timer.it_value.tv_usec = 0;		// first time interval, microseconds part
//
//    // configure the timer to expire every 3 sec after that.
//    _timer.it_interval.tv_sec = 3;	// following time intervals, seconds part
//    _timer.it_interval.tv_usec = 0;	// following time intervals, microseconds part
//
//    // Start a virtual timer. It counts down whenever this process is executing.
//    if (setitimer (ITIMER_VIRTUAL, &_timer, NULL)) {
//        printf("setitimer error.");
//    }

    for(;;) {

    }
}


