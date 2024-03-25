/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "thread_test_simple.hh"
#include "system.hh"

#include <stdio.h>
#include <string.h>

#include "semaphore.hh"

static const unsigned int numThreads = 4; // Cantidad de hilos extra a ejecutar
static const char *threadNames[numThreads] = { "2nd", "3nd", "4nd", "5nd" };
static bool threadsDone[numThreads]; // Indica que hilos han terminado la ejecucion

#ifdef SEMAPHORE_TEST
Semaphore* sem = new Semaphore("test semaphore", 3);
#endif

void
SimpleThread(void *arg_)
{

    // If the lines dealing with interrupts are commented, the code will
    // behave incorrectly, because printf execution may cause race
    // conditions.
    unsigned i = (unsigned long) arg_;
    for (unsigned num = 0; num < 10; num++) {
        #ifdef SEMAPHORE_TEST
        sem->P();
        DEBUG('s', "\\\\ Thread `%s` realizo un P() al semaforo `%s`\n", currentThread->GetName(), sem->GetName());
        #endif
        printf("*** Thread `%s` is running: iteration %u\n", currentThread->GetName(), num);
        #ifdef SEMAPHORE_TEST
        sem->V();
        DEBUG('s', "// Thread `%s` realizo un V() al semaforo `%s`\n", currentThread->GetName(), sem->GetName());
        #endif
        currentThread->Yield();
    }
    

    if (strcmp(currentThread->GetName(),"main") != 0) {
      threadsDone[i] = true;
    }
    printf("!!! Thread `%s` has finished SimpleThread\n", currentThread->GetName());
}

static unsigned countThreadsDone() {
  int done = 0;
  for (unsigned i = 0; i < numThreads; i++)
    done += threadsDone[i];
  return done;
}

/// Set up a ping-pong between several threads.
///
/// Do it by launching one thread which calls `SimpleThread`, and finally
/// calling `SimpleThread` on the current thread.
void
ThreadTestSimple()
{
    for (unsigned i = 0; i < numThreads; i++)
      threadsDone[i] = false;

    // Crear y ejecutar los hilos
    for (unsigned i = 0; i < numThreads; i++) {
      Thread *thread = new Thread(threadNames[i]);
      thread->Fork(SimpleThread, (void*) (long) i); // Evitar errores de casteo
    }

    //the "main" thread also executes the same function
    SimpleThread(NULL);

    // Esperar al resto de hilos
    while (countThreadsDone() < numThreads) {
        currentThread->Yield(); 
    }

    #ifdef SEMAPHORE_TEST
    delete sem;
    #endif
    
    printf("Test finished\n");
}
