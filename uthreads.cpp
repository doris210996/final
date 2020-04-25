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
static list<Thread*> ready;
static Thread* running; // The "Running" thread.
static map<int, Thread*> _threads;
static int _quantum_counter = 0;
struct itimerval _timer;
struct sigaction _sigAction;
int* sig;
static vector<int> quantums;


/**
 * function responsable for printing each kind of error
 */
void printError(int type, string pre)
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


int nextId()
{
    for(int i =0; i <100; i++)
    {
        if(!_threads.count(i))
        {
            return i;
        }
    }
    return -1;
}


void addReady(Thread* th)
{
    if(th == nullptr)
    {
        return;
    }
    ready.push_back(th);

}



static void setTime()
{
    _timer.it_value.tv_sec = (int)(quantums[running->getPriority()]/MICRO_TO_SECOND);
    _timer.it_value.tv_usec = quantums[running->getPriority()] % MICRO_TO_SECOND;
    if (setitimer(ITIMER_VIRTUAL, &_timer, NULL) == FAIL)
    {
        printError(SET_TIME_ERROR, SYS_ERROR);
        exit(1);
    }
}



Thread* popReady(){
    if(ready.empty())
    {
        return nullptr;
    }
    Thread* ret = ready.front();
    ready.pop_front();
    return ret;
}



int removeFromReady(int id){
    for(Thread* th: ready)
    {
        if(th->getId()==id)
        {
            ready.remove(_threads.at(id));
            return 0;
        }
    }
    return -1;
}




void switchThreads()
{
    Thread *prev = running;
    if(prev != nullptr)   // if sent from suspend, so running is nullptr
    {
        prev->setState(READY);
    }
    Thread* next = popReady();
    if(next != nullptr)   //that's mean that not only one thread can run now!
    {
        running = next;
    }
    running->setState(RUNNING);
    running->incQuantums();
    _quantum_counter++;
    setTime();
    addReady(prev);
}


static void timeHandler(int signum)
{
    // When val is not 0 it means we just switched so we will get to the first line with the next
    // thread and its context and return from the handler function to its own function in the point
    // before he last ends(the last time its time finishes)
    bool timeOut = sigsetjmp(running->getContext(),1) == 0;
    if (timeOut)
    {
        switchThreads();
        siglongjmp(running->getContext(),1);
    }
}



/* Initialize the thread library */
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
    if(sigaction(SIGVTALRM,&_sigAction,NULL) == FAIL)
    {
        printError(SIGNAL_ACTION_ERROR, SYS_ERROR);
        exit(1);
    }

    quantums = std::vector<int> (quantum_usecs, quantum_usecs + size);
    Thread* mainTh = new Thread(0, 0, nullptr, READY);
    _threads.insert(pair<int, Thread*>(0, mainTh));
    running = mainTh;
    mainTh->setState(RUNNING);
    mainTh->incQuantums();
    _quantum_counter++;
    setTime();
    return SUCCESS;
}


int uthread_spawn(void (* f)(void), int pr)
{
    if (_threads.size() < 100)
    {
        int tid = nextId();
        Thread *th = new Thread(tid, pr, f, READY);
        _threads.insert(std::pair<int, Thread *>(tid, th));
        addReady(th);
        return tid;
    }
    else
    {
        //printErrors
        return -1;
    }
}

/* Terminates a thread */
int uthread_terminate(int tid)
{
    //terminate main
    if (tid == MAIN_THREAD)
    {
        _threads.clear();
        exit(0);
    }

    //not found id
    if (!_threads.count(tid))
    {
        printError(NOT_FOUND_ID, THREAD_ERROR);
        return FAIL;
    }

    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, NULL);
    //running terminate itself
    if (tid == running->getId())
    {
        Thread* temp = running;
        switchThreads();
        removeFromReady(tid);
        _threads.erase(temp->getId());
        siglongjmp(running->getContext(),1);
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
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, NULL);
    return SUCCESS;
}

/* Suspend a thread */
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
    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, NULL);
    if (tid == running->getId())
    {
        bool weAfterResume = sigsetjmp(running->getContext(),1) != 0;
        if(weAfterResume)
        {
            return 0;
        }
        else
        {
            running->setState(BLOCK);
            blockedCounter++;
            Thread* next = popReady();
            if(next != nullptr)   //that's mean that not only one thread can run now!
            {
                running = next;
            }
            running->setState(RUNNING);
            running->incQuantums();
            _quantum_counter++;
            setTime();
            siglongjmp(running->getContext(), 1);
        }
    }
    else
    {
        if(removeFromReady(_threads[tid]->getId()) == FAIL)  // in blocked.
        {
            sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, nullptr);
            return 0;
        }
        // Was in Ready
        removeFromReady(tid);
        _threads[tid]->setState(BLOCK);
        blockedCounter++;
    }
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, NULL);
    return SUCCESS;
}

/* Resume a thread */
int uthread_resume(int tid)
{
    if(!_threads.count(tid))
    {
        return FAIL;
    }
    if(_threads[tid]->getState() != BLOCK )
    {
        return SUCCESS;
    }
    //if not in block, don't resume
    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, NULL);
    _threads[tid]->setState(READY);
    addReady(_threads[tid]);
    blockedCounter--;
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, NULL);
    return SUCCESS;


}
int uthread_change_priority(int tid, int priority){
    if(!_threads.count(tid))
    {
        return -1;
    }
    _threads[tid]->setPriority(priority);
    return 0;
}

int uthread_get_tid()
{
    return running->getId();
}

int uthread_get_total_quantums()
{
    return _quantum_counter;
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
//
//void f(void) {
////    uthread_block(1);
//
//    int i = 0;
//    while (1) {
//        ++i;
//
//        if (i % 100000000 == 0) {
//
//            printf("in f (%d)\n", i);
//
//
//        }
//
//    }
//}
//
//
//void g(void) {
//    int i = 0;
//
//    while (1) {
//
//        ++i;
//        if (i % 100000000 == 0) {
//            printf("in g (%d)\n", i);
//
//        }
//
//    }
//}
//
//void h(void) {
////    uthread_resume(1);
//
//    int i = 0;
//    while (1) {
//        ++i;
//        if (i % 100000000 == 0) {
//            printf("in h (%d)\n", i);
//
//
//        }
//    }
//}
//
//int main() {
//    int quan[8] = {99999, 900000, 800000, 100000 - 20000, 100000 - 30000, 100000 - 40000, 100000 - 50000,
//                   100000 - 60000};
//    uthread_init(quan, 8);
//    int i = 0;
////    while (1) {
////        if (i % 3 == 0) {
//            printf("%d\n", uthread_spawn(&f, 1));
////        } else if (i % 3 == 1) {
//            printf("%d\n", uthread_spawn(&g, 2));
////        } else {
//            printf("%d\n", uthread_spawn(&h, 1));
////        }
////        uthread_block(2);
//        printf("in main");
//        ++i;
////    }
//    while (true) {
//    }
//
//
//    return 0;
//}