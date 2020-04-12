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
#define SAME_LOCATION_JMP 0
#define SET_TIME_ERROR 3
#define ERROR_PREFIX "thread library error: "
static std::vector<int> quantums;
static std::list<Thread*> ready;
static std::list<Thread*> blocked;
static std::map<int, Thread*> _threads;
static Thread* _running;
static int _priorityAmount;
static struct sigaction _sigAction;



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
    return _threads.begin()->first;
}

static void moveToReady(Thread* th)
{
    if (th == nullptr)
    {
        return;
    }
    ready.push_back(th);
}

// This function
static void startTimer(struct itimerval timer){
    if (setitimer(ITIMER_VIRTUAL, &timer, nullptr) == -1)
    {
        printErrors(SET_TIME_ERROR);
        exit(1);
    }
}


static void myTimeOutFunc(int id){
    bool flag = sigsetjmp(_running->getContext(),1) == SAME_LOCATION_JMP;
    if (flag)
    {
        //witchThreads();
        siglongjmp(_running->getContext(),1);
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
    _sigAction.sa_handler = myTimeOutFunc;
    if(sigaction(SIGVTALRM,&_sigAction,nullptr) < 0)
    {
        printErrors(SIGACTION_ERROR);
        return -1;
    }
    quantums = std::vector<int> (quantum_usecs, quantum_usecs + size);
    std::sort(quantums.rbegin(),quantums.rend()); // reversed order, From now we will refer to
    // priority kth quantum
    // as the value of the kth index (when priority 0 which considered highest -has the longest
    // time)
    _priorityAmount = size; // save the number of priors.

    // set timer for each thread when create it except the main default who gets the longest
    Thread* mainDefault = new Thread(0,HIGHEST_PRIORITY, nullptr, READY);
    mainDefault->setTimer(quantums[HIGHEST_PRIORITY]);
    _threads.insert(std::pair<int, Thread*>(0, mainDefault));
    _running = mainDefault;
    mainDefault->setState(RUNNING);
    mainDefault->updateQuantum();
    //counter++

    // The default main timer starts running
    startTimer(_running->getTimer());
    return 0;
}



int main()
{
    int  x[] = {1,3,2};
    uthread_init(x, 3);
    for(int element:quantums)
    {
        std::cout << element << std::endl;
    }

    // 0 is false anything else is true

    return 0;
}