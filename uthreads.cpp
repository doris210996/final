#include "uthreads.h"
#include "Thread.h"
#include <vector>
#include <queue>
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <list>
using namespace std;
#define FAIL -1
#define SUCCESS 0
#define MAIN_THREAD 0
#define MICRO_TO_SECOND 1000000
#define THREAD_ERROR "thread library error: "
#define SYS_ERROR "system error: "
static list<Thread*> ready;
static Thread* running;
static map<int, Thread*> _threads;
static int _quantum_counter = 0;
struct itimerval _timer;
struct sigaction _sigAction;
static std::vector<int> _quantums;
static int blockedCounter;


void helper(){
    cout<< "\n";
    cout<<"ready queue:\n";
    for(Thread* th: ready)
    {
        cout<<th->getId()<<"\n";
    }

    cout<<"all threads set:\n";
    for (std::map<int, Thread*>::iterator it = _threads.begin(); it != _threads.end(); ++it)
    {
        cout<<it->first<<"\n";
    }

    cout<<"How many blocked:"<<blockedCounter<<"\n\n";

}




int nextId(){
    for(int i=0; i<MAX_THREADS; i++)
    {
        if(!_threads.count(i))
        {
            return i;
        }
    }
    return -1;
}


void pushReady(Thread* th)
{
    if(th == nullptr)
    {
        return;
    }

    ready.push_back(th);
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

static void setTime()
{
    _timer.it_value.tv_sec = (int) (_quantums[running->getPriority()] / MICRO_TO_SECOND);
    _timer.it_value.tv_usec = _quantums[running->getPriority()] % MICRO_TO_SECOND;
    if (setitimer(ITIMER_VIRTUAL, &_timer, NULL) == FAIL)
    {
        exit(1);
    }
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
//    cout<<"swich:\n";
//    for(Thread* th:ready)
//    {
//        cout<<th->getId()<<"\n";
//    }

    Thread *prev = running;
    prev->setState(READY);
    Thread* next = popReady();
    if(next != nullptr)
    {
        running = next;
    }
    running->setState(RUNNING);
    running->incQuantums();
    _quantum_counter++;
    setTime();
    if ((int) _threads.size() - 1 == blockedCounter)
    {
        return;
    }
    pushReady(prev);
}

static void timeHandler(int signum)
{
    bool timeOut = sigsetjmp(running->getContext(),1) == 0;
    if (timeOut)
    {
        switchThreads();
        siglongjmp(running->getContext(),1);
    }
}




int uthread_init(int *quantum_usecs,int size)
{
    if (size <= 0)
    {
        return -1;
    }
    for(int i=0; i<size; i++)
    {
        if(quantum_usecs[i]<=0)
        {
            return -1;
        }
    }
    _sigAction.sa_handler = timeHandler;
    if(sigemptyset (&_sigAction.sa_mask) == FAIL)
    {
        exit(1);
    }
    if(sigaddset(&_sigAction.sa_mask, SIGVTALRM))
    {
        exit(1);
    }
    _sigAction.sa_flags = 0;
    if(sigaction(SIGVTALRM,&_sigAction,NULL) == FAIL)
    {
        exit(1);
    }
    _quantums = std::vector<int> (quantum_usecs, quantum_usecs + size);
    std::sort(_quantums.rbegin(), _quantums.rend()); // reversed order, From now we will refer to
    // priority kth quantum
    // as the value of the kth index (when priority 0 which considered highest -has the longest
    // time)
    _timer.it_value.tv_sec = (int)(size/MICRO_TO_SECOND);
    _timer.it_value.tv_usec = size % MICRO_TO_SECOND;
    auto* mainTh = new Thread(0, 0, nullptr, READY);
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
    //can't add any more!!
    if(_threads.size() == 100)
    {
        return FAIL;
    }

    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, NULL);

    int tid = nextId();
    auto* th = new Thread(tid, pr, f, READY);
    _threads.insert(pair<int, Thread*>(tid, th));
    if (running == NULL)  //for the first running (the main thread)
    {
        Thread* tmp = popReady();
        if(tmp != nullptr)   //that's mean that not only one thread can run now!
        {
            running = tmp;
        }
        running->setState(RUNNING);
        running->incQuantums();
        _quantum_counter++;
        setTime();;
    }
    else
    {
        pushReady(th);
    }
    sigprocmask(SIG_UNBLOCK,&_sigAction.sa_mask, NULL);
    return tid;
}


int uthread_terminate(int tid)
{
    if (tid == MAIN_THREAD)
    {
        _threads.clear();
        exit(0);
    }
    if (!_threads.count(tid))
    {
        return FAIL;
    }
    if (tid == running->getId())
    {
        Thread* temp = running;
        switchThreads();
        removeFromReady(tid);
        blockedCounter--;
        _threads.erase(temp->getId());
        siglongjmp(running->getContext(),1);
    }
    if (_threads.at(tid)->getState() == READY)
    {
        removeFromReady(tid);
        _threads.erase(tid);
    }
    else
    {
        blockedCounter--;
        _threads.erase(tid);
    }
    return SUCCESS;
}

int uthread_block(int tid)
{
    if (tid == MAIN_THREAD)
    {
        return FAIL;
    }
    if (!_threads.count(tid))
    {
        return FAIL;
    }
    sigprocmask(SIG_BLOCK,&_sigAction.sa_mask, NULL);
    if (tid == running->getId())
    {
        sigsetjmp(running->getContext(),1);
        running->setState(BLOCK);
        blockedCounter++;
        running = nullptr;
        Thread* tmp = popReady();
        if(tmp != nullptr)
        {
            running = tmp;
        }
        running->setState(RUNNING);
        running->incQuantums();
        _quantum_counter++;
        setTime();
        siglongjmp(running->getContext(),1);
    }
    if(removeFromReady(_threads[tid]->getId()) == FAIL)
    {
        return FAIL;
    }
    _threads[tid]->setState(BLOCK);
    blockedCounter++;
    return SUCCESS;
}


int uthread_resume(int tid)
{
    if(!_threads.count(tid))
    {
        return FAIL;
    }
    if(_threads.at(tid)->getState()==BLOCK)
    {
        _threads.at(tid)->setState(READY);
        pushReady(_threads.at(tid));
        blockedCounter--;
        return SUCCESS;
    }
    else
    {
        return SUCCESS;
    }
}

int uthread_get_tid()
{
    return running->getId();
}

int uthread_get_total_quantums()
{
    return _quantum_counter;
}

int uthread_change_priority(int tid, int priority){
    if(!_threads.count(tid))
    {
        return -1;
    }
    _threads[tid]->setPriority(priority);
    return 0;
}
int uthread_get_quantums(int tid)
{
    int valToRet;
    if (!_threads.count(tid))
    {
        valToRet = FAIL;
    }
    else
    {
        valToRet = _threads.at(tid)->getQuantumsAmount();
    }
    return valToRet;
}

void f(void) {
    int i = 0;
    while (1) {
        ++i;
        if (i % 100000000 == 0) {
            printf("in f (%d)\n", i);


    }
}}


void g(void) {
    int i = 0;
    while (1) {
        ++i;
        if (i % 100000000 == 0) {
            printf("in g (%d)\n", i);
        }
    }
}

void h(void) {
    int i = 0;
    while (1)
        ++i;
        if (i % 100000000 == 0) {
            printf("in h (%d)\n", i);
        }
    }


int main() {
    // It works properly when all have same quantum , but fails with different times
    int quan[8] = {1000000,1000000,1000000};
    uthread_init(quan, 3);
    int i = 0;
            printf("%d\n", uthread_spawn(&f, 1));
            printf("%d\n", uthread_spawn(&g, 2));
            printf("%d\n", uthread_spawn(&h, 1));
            printf("in main\n");

    while (true) {}


    return 0;
}
