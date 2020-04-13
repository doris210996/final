#include "Thread.h"
#define JUMP_VAL 1
#ifdef __x86_64__


typedef unsigned long address_t;
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7
#define MICRO_TO_SEC 1000000

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%gs:0x18,%0\n"
			"rol    $0x9,%0\n"
			: "=g" (ret)
			  : "0" (addr));
	return ret;
}

#endif



Thread::Thread(int id, int pr, void (*f)(void), State state): _id(id), _pr(pr), _f(f), _state(state)
{
    _quantum = 0;
    _stack = new char[STACK_SIZE];
    address_t sp, pc;
    sp = (address_t)_stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)*f;
    sigsetjmp(_context, 1);
    (_context->__jmpbuf)[JB_SP] = translate_address(sp);
    (_context->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&_context->__saved_mask);
}

void Thread::setTimer(int quantum)
{
    _timer.it_value.tv_sec = (int)(quantum/MICRO_TO_SEC);
    _timer.it_value.tv_usec = quantum % MICRO_TO_SEC;
    _timer.it_interval.tv_sec = (int)(quantum/MICRO_TO_SEC);
    _timer.it_interval.tv_usec = quantum % MICRO_TO_SEC;

}

void Thread::setPriority(int pr)
{
    _pr = pr;
}

struct itimerval Thread::getTimer()
{
    return _timer;
}

void Thread::setState(State state)
{
    _state = state;
}

State Thread::getState() const
{
    return _state;
}

Thread::~Thread()
{
    delete[] _stack;
}

int Thread::getId() const
{
    return _id;
}

int Thread::getPriority() const
{
    return _pr;
}

int Thread::getCurQuantum() const
{
    return _quantum;
}

sigjmp_buf& Thread::getContext() {
    return _context;
}

void Thread::updateQuantum()
{
    ++_quantum;
}


