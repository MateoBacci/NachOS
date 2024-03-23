/// Routines for synchronizing threads.
///
/// The implementation for this primitive does not come with base Nachos.
/// It is left to the student.
///
/// When implementing this module, keep in mind that any implementation of a
/// synchronization routine needs some primitive atomic operation.  The
/// semaphore implementation, for example, disables interrupts in order to
/// achieve this; another way could be leveraging an already existing
/// primitive.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "condition.hh"
#include "system.hh"


/// Dummy functions -- so we can compile our later assignments.
///

Condition::Condition(const char *debugName, Lock *conditionLock)
{
    name = debugName;
    lock = conditionLock;
    internalLock = new Lock(NULL);
    sem = new Semaphore(NULL,0);
    threadCount = 0;
}

Condition::~Condition()
{
    delete internalLock;
    delete sem;
}

const char *
Condition::GetName() const
{
    return name;
}

void
Condition::Wait()
{   
    internalLock->Acquire();
    ASSERT(threadCount >= 0);
    threadCount++;
    internalLock->Release();
    DEBUG('s',"Thread '%s' waiting on conditional variable '%s'\n", currentThread->GetName(),GetName());
    sem->P();
}

void
Condition::Signal()
{
    internalLock->Acquire();
    ASSERT(threadCount >= 0);
    DEBUG('s',"Thread '%s' cast signal on conditional variable '%s'\n", currentThread->GetName(),GetName());
    if(threadCount == 0) {
        internalLock->Release();
        return;
    }
    threadCount--;
    sem->V();
    internalLock->Release();
}

void
Condition::Broadcast()
{
    
    internalLock->Acquire();
    ASSERT(threadCount >= 0);
    DEBUG('s',"Thread '%s' cast broadcast on conditional variable '%s'\n", currentThread->GetName(),GetName());
    while (threadCount-- > 0){
        sem->V();
    }
    internalLock->Release();
}
