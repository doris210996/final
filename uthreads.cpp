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
#define MICRO_TO_SECOND 1000000
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


/*
 * This function finds & returns the next smallest nonnegative integer not already taken by an existing thread,
 * or -1 if there are no available free ids.
 */
int nextId(){
    return _threads.begin()->first; // since a map is sorted by keys
}


void pushReady(Thread* th)
{
    if(th == nullptr)
    {
        return;
    }
    _ready.push_back(th);
}



static void startTimer()
{
    _timer.it_value.tv_sec = (int) (_quantums[_running->getPriority()] / MICRO_TO_SECOND);
    _timer.it_value.tv_usec = _quantums[_running->getPriority()] % MICRO_TO_SECOND;
    _timer.it_interval.tv_sec = (int) (_quantums[_running->getPriority()] / MICRO_TO_SECOND);
    _timer.it_interval.tv_usec = _quantums[_running->getPriority()] % MICRO_TO_SECOND;
    if (setitimer(ITIMER_VIRTUAL, &_timer, NULL) == -1)
    {
        printErrors(SET_TIME_ERROR);
        exit(1);
    }

}

Thread* popReady(){
    if(_ready.empty())
    {
        return nullptr;
    }
    Thread* ret = _ready.front();
    _ready.pop_front();
    return ret;
}

void runThread()
{
    Thread* tmp = popReady();
    if(tmp != nullptr)
    {
        _running = tmp;
    }
    _running->setState(RUNNING);
    _running->incQuantums();
    _qCounter++;
    startTimer();
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


void switchThreads()
{
    Thread *prev = _running;
    prev->setState(READY);
    Thread* next = popReady();
    _running = next;
    _running->setState(RUNNING);
    _running->incQuantums();
    _qCounter++;
    startTimer();
    pushReady(prev);
}


static void timeHandler(int signum)
{
    flag = 1;
    if(_running->getPriority()==1)
    {
        printf("Handler enter with SecondTh\n");
    }
    if(_running->getPriority()==0)
    {
        printf("Handler enter with FirstTh\n");
    }


    bool timeOut = sigsetjmp(_running->getContext(),1) == 0;
    if (timeOut)
    {
        switchThreads();
        siglongjmp(_running->getContext(),1);
    }
}




int uthread_init(int *quantum_usecs,int size)
{
    if (size <= 0)
    {
        printErrors(INVALID_PRIOR_NUM);
        return -1;
    }

    for(int i=0; i<size; i++)
    {
        if(quantum_usecs[i]<=0)
        {
            printErrors(INVALID_QUANTUM_VAL);
            return -1;
        }
    }

    //initialize sigaction
    _sigAction.sa_handler = timeHandler;

    if(sigaction(SIGVTALRM,&_sigAction,NULL) < 0)
    {
        printErrors(SIGACTION_ERROR);
        exit(1);
    }

    _quantums = std::vector<int> (quantum_usecs, quantum_usecs + size);
    std::sort(_quantums.rbegin(), _quantums.rend()); // reversed order, From now we will refer to
    // priority kth quantum
    // as the value of the kth index (when priority 0 which considered highest -has the longest
    // time)

    //initialize timer
    _timer.it_value.tv_sec = (int)(_quantums[0]/MICRO_TO_SECOND);
    _timer.it_value.tv_usec = _quantums[0] % MICRO_TO_SECOND;
    _timer.it_interval.tv_sec = (int)(_quantums[0]/MICRO_TO_SECOND);
    _timer.it_interval.tv_usec = _quantums[0] % MICRO_TO_SECOND;

    Thread* mainTh = new Thread(0,0, nullptr, READY);
    _threads.insert(std::pair<int, Thread*>(0, mainTh));
    _running = mainTh;
    mainTh->setState(RUNNING);
    mainTh->incQuantums();
    _qCounter++;
    startTimer();
    return 0;
}

int uthread_spawn(void (* f)(void), int pr)
{
    if (_threads.size() < 100)
    {
        int tid = nextId();
        Thread *th = new Thread(tid, pr, f, READY);
//        _timer.it_value.tv_sec = (int) (_quantums[pr] / MICRO_TO_SECOND);
//        _timer.it_value.tv_usec = _quantums[pr] % MICRO_TO_SECOND;
//        _timer.it_interval.tv_sec = (int) (_quantums[pr] / MICRO_TO_SECOND);
//        _timer.it_interval.tv_usec = _quantums[pr] % MICRO_TO_SECOND;
        _threads.insert(std::pair<int, Thread *>(tid, th));
        pushReady(th);
        return tid;
    }
    else
    {
        //printErrors
        return -1;
    }
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
        _threads.erase(tid);
        switchThreads();
        siglongjmp(_running->getContext(),1);
    }

    if (_threads.at(tid)->getState() == READY) // in _ready list
    {
        removeReady(tid);
        _threads.erase(tid);
    }

    if (_threads.at(tid)->getState() == BLOCK) // in _blocked list
    {
        removeBlocked(tid);
        _threads.erase(tid);
    }
    return 0;
}


int uthread_get_tid()
{
    return _running->getId();
}

int uthread_get_total_quantums()
{
    return _qCounter;
}

int uthread_change_priority(int tid, int priority){
    if(!_threads.count(tid))
    {
        //printErrors
        return -1;
    }
    _threads[tid]->setPriority(priority);
    return 0;
}


int uthread_get_quantums(int tid){
    if (_threads.count(tid))
    {
        sigprocmask(SIG_BLOCK, &_sigAction.sa_mask, nullptr);
        int num = _threads.at(tid)->getQuantumsAmount();
        sigprocmask(SIG_UNBLOCK, &_sigAction.sa_mask, nullptr);
        return num;
    }
    //printErrors
    return -1;
}

void f(){
    while(1)
    {
        if(flag)
        {
            printf("hi\n");
            flag =0;
        }

    }
}
int main()
{
    int x[] = {3000000, 3000000, 3000000};
    uthread_init(x, 3);
    uthread_spawn(f,1);



    while (1)
    {

    }
}
//1.get inside the handler after 3 secs with the mainTh(actually in an infinte loop)
//2.right after it switches and get to f (almost same time)
//3. after 3 secs in f (with a single print) go back to handler
//4. now we in the handler with the second handler switching to the first and the first stay in
// the infinite loop for 3 secs and  we get the following pattern when first two lines same time:
//> Handler enter with FirstTh
//> hi
//> Handler enter with SecondTh

