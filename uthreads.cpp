#include "Thread.h"
#include <vector>
#include <list>
#include <stdlib.h>
#include <map>
using namespace std;
#define FAIL -1
#define SUCCESS 0
#define MAIN_THREAD 0
#define MICRO_TO_SECOND 1000000
#define THREAD_ERROR "thread library error: "
#define SYS_ERROR "system error: "
#define NOT_FOUND_ID 0
#define SUSPEND_MAIN 1
#define SET_TIME_ERROR 2
#define WRONG_INPUT 3
#define SIGNAL_ACTION_ERROR 4
#define TOO_MANY_THREADS 5
static int blockedCounter;
static list<int > ready;
static int running; // The "Running" thread.
static map<int, Thread*> _threads;
static int _quantum_counter = 0;
struct itimerval _timer;
struct sigaction _sigAction;
static vector<int> quantums;


/**
 * function for printing each kind of error
 */
void printError(int type, const string& pre)
{
    switch (type)
    {
        case NOT_FOUND_ID:
        {
            cerr << pre << "the requested tid not found" << endl;
            break;
        }
        case SUSPEND_MAIN:
        {
            cerr << pre << "You are trying to suspend the main thread" << endl;
            break;
        }
        case SET_TIME_ERROR:
        {
            cerr << pre << "Unable to set timer" << endl;
            break;
        }
        case WRONG_INPUT:
        {
            cerr << pre << "wrong input" << endl;
            break;
        }
        case SIGNAL_ACTION_ERROR:
        {
            cerr << pre << "signal action error" << endl;
            break;
        }
        case TOO_MANY_THREADS:
        {
            cerr << pre << "too many threads" << endl;
            break;
        }
        default:
            break;
    }
}

/**
 * function for get the next available tid
 */
int nextId()
{
    for(int i =0; i <100; i++)
    {
        if(!_threads.count(i))
        {
            return i;
        }
    }
    return FAIL;
}

/**
 * function for adding the thread with the given tid to the ready queue
 */
void addReady(int th)
{
    if(th == FAIL)
    {
        return;
    }
    ready.push_back(th);

}


/**
 * function to start the timer for recall the signal
 */
static void setTime()
{
    _timer.it_value.tv_sec = (int)(quantums[_threads[running]->getPriority()]/MICRO_TO_SECOND);
    _timer.it_value.tv_usec = quantums[_threads[running]->getPriority()] % MICRO_TO_SECOND;
    if (setitimer(ITIMER_VIRTUAL, &_timer, nullptr) == FAIL)
    {
        printError(SET_TIME_ERROR, SYS_ERROR);
        exit(1);
    }
}


/**
 * function to pop the first thread from the ready queue (robin algorithm)
 */
int popReady(){
    if(ready.empty())
    {
        return FAIL;
    }
    int ret = ready.front();
    ready.pop_front();
    return ret;
}


/**
 * function to remove the thread with the given tid from the queue
 */
int removeFromReady(int id){
    for(int th: ready)
    {
        if(th == id)
        {
            ready.remove(id);
            return SUCCESS;
        }
    }
    return FAIL;
}



/**
 * function to switch between threads
 */
void switchThreads()
{
    int prev = running;
    _threads[prev]->setState(READY);
    int next = popReady();
    if(next != -1)   //that's mean that not only one thread can run now!
    {
        running = next;
    }
    _threads[running]->setState(RUNNING);
    _threads[running]->incQuantums();
    _quantum_counter++;
    setTime();
    if(_threads[prev]->getId() != _threads[running]->getId())
    {
        addReady(prev);
    }
}

/**
 * This function is called when the time is out it replaces the natural behaviour of the signal
 */
static void timeHandler(int)
{
    // When val is not 0 it means we just switched so we will get to the first line with the next
    // thread and its context and return from the handler function to its own function in the point
    // before he last ends(the last time its time finishes)
    bool timeOut = sigsetjmp(_threads[running]->getContext(),1) == 0;
    if (timeOut)
    {
        switchThreads();
        siglongjmp(_threads[running]->getContext(),1);
    }
}



/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * an array of the length of a quantum in micro-seconds for each priority.
 * It is an error to call this function with an array containing non-positive integer.
 * size - is the size of the array.
 * Return value: On success, return 0. On failure, return -1.
*/
int  uthread_init(int *quantum_usecs, int size)

{
    if (size <= 0)
    {
        printError(WRONG_INPUT ,THREAD_ERROR);
        return FAIL;
    }
    for(int i=0 ; i<size; i++)
    {
        if(quantum_usecs[i]<1)
        {
            return FAIL;
        }
    }

    //initialize sigaction
    _sigAction.sa_handler = timeHandler;
    if(sigemptyset (&_sigAction.sa_mask) == FAIL)
    {
        printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
        exit(1);
    }
    if(sigaddset(&_sigAction.sa_mask, SIGVTALRM))
    {
        printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
        exit(1);
    }
    _sigAction.sa_flags = 0;
    if(sigaction(SIGVTALRM,&_sigAction,nullptr) == FAIL)
    {
        printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
        exit(1);
    }
    quantums = std::vector<int> (quantum_usecs, quantum_usecs + size);
    auto* mainTh = new Thread(0, 0, nullptr, READY);
    _threads.insert(pair<int, Thread*>(0, mainTh));
    running = 0;
    mainTh->setState(RUNNING);
    mainTh->incQuantums();
    _quantum_counter++;
    setTime();
    return SUCCESS;
}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * priority - The priority of the new thread.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (* f)(), int pr)
{
    if (_threads.size() < 100)
    {
        int tid = nextId();
        auto *th = new Thread(tid, pr, f, READY);
        _threads.insert(std::pair<int, Thread *>(tid, th));
        addReady(tid);
        return tid;
    }
    else
    {
        printError(TOO_MANY_THREADS,THREAD_ERROR);
        return -1;
    }
}

/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    //terminate main
    if (tid == MAIN_THREAD)
    {
        for (auto & _thread : _threads) {
            delete []_thread.second->getStack();
            delete _thread.second;
        }
        exit(0);
    }
    //not found id
    if (!_threads.count(tid))
    {
        printError(NOT_FOUND_ID, THREAD_ERROR);
        return FAIL;
    }

    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, nullptr);
    //running terminate itself
    if (tid == running)
    {
        int temp = running;
        switchThreads();
        removeFromReady(tid);
        _threads.erase(temp);
        siglongjmp(_threads[running]->getContext(),1);
    }
    // The termination is for a thread that in ready.
    if (_threads.at(tid)->getState() == READY)
    {
        removeFromReady(tid);
        _threads.erase(tid);
    }
    else // Thread is in Block.
    {
        blockedCounter--;
        _threads.erase(tid);
    }
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
    return SUCCESS;
}

/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid)
{
    if (tid == MAIN_THREAD)
    {
        printError(SUSPEND_MAIN, THREAD_ERROR);
        return FAIL;
    }
    if (!_threads.count(tid))
    {
        printError(NOT_FOUND_ID, THREAD_ERROR);
        return FAIL;
    }
    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, nullptr);
    if (tid == running)
    {
        bool weAfterResume = sigsetjmp(_threads[running]->getContext(),1) != 0;
        if(weAfterResume)
        {
            sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
            return SUCCESS;
        }
        else
        {
            _threads[running]->setState(BLOCK);
            blockedCounter++;
            int next = popReady();
            if(next != -1)   //that's mean that not only one thread can run now!
            {
                running = next;
            }
            _threads[running]->setState(RUNNING);
            _threads[running]->incQuantums();
            _quantum_counter++;
            setTime();
            siglongjmp(_threads[running]->getContext(), 1);
        }
    }
    else
    {
        if(removeFromReady(_threads[tid]->getId()) == FAIL)  // in blocked.
        {
            sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
            return SUCCESS;
        }
        // Was in Ready
        removeFromReady(tid);
        _threads[tid]->setState(BLOCK);
        blockedCounter++;
    }
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
    return SUCCESS;
}

/*
 * Description: This function resumes a _blocked thread with ID tid and moves
 * it to the READY state if it's not synced. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    if(!_threads.count(tid))
    {
        printError(NOT_FOUND_ID,THREAD_ERROR);
        return FAIL;
    }
    if(_threads[tid]->getState() != BLOCK )
    {
        return SUCCESS;
    }
    //if not in block, don't resume
    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, nullptr);
    _threads[tid]->setState(READY);
    addReady(tid);
    blockedCounter--;
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
    return SUCCESS;


}

/*
 * Description: This function changes the priority of the thread with ID tid.
 * If this is the current running thread, the effect should take place only the
 * next time the thread gets scheduled.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_change_priority(int tid, int priority){
    if(!_threads.count(tid))
    {
        printError(NOT_FOUND_ID,THREAD_ERROR);
        return FAIL;
    }
    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, nullptr);
    _threads[tid]->setPriority(priority);
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
    return SUCCESS;
}

/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    return running;
}

/*
 * Description: This function returns the total number of _quantums since
 * the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of _quantums.
*/
int uthread_get_total_quantums()
{
    return _quantum_counter;
}

/*
 * Description: This function returns the number of _quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered an error.
 * Return value: On success, return the number of _quantums of the thread with ID tid.
 * 			     On failure, return -1.
*/
int uthread_get_quantums(int tid){
    if (_threads.count(tid))
    {
        sigprocmask(SIG_BLOCK, &_sigAction.sa_mask, nullptr);
        int num = _threads.at(tid)->getQuantumsAmount();
        sigprocmask(SIG_UNBLOCK, &_sigAction.sa_mask, nullptr);
        return num;
    }
    printError(NOT_FOUND_ID,THREAD_ERROR);
    return FAIL;
}
