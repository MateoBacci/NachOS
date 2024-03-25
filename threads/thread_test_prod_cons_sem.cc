/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "thread_test_prod_cons_sem.hh"
#include "synch_list.hh"
#include "system.hh"

#include <stdio.h>

typedef int Product;
static SynchList<Product> *queue;

#define NUM_PRODUCERS 10
#define NUM_CONSUMERS 10
#define QUEUE_SIZE 5

#include "semaphore.hh"

static Semaphore *consumerSem;
static Semaphore *producerSem;

static void
Produce(unsigned n) {
  queue->Append(n);
  printf("*** Producer `%s` writes %u\n", currentThread->GetName(), n);
}

static unsigned
Consume() {
  unsigned n;
  n = queue->Pop();
  printf("*** Consumer `%s` reads %u\n", currentThread->GetName(), n);
  return n;
}

static void
Producer(void* _arg) {
  unsigned *n = (unsigned*) _arg;
  for (;;) {
    producerSem->P();
    Produce(*n);
    currentThread->Yield();
    consumerSem->V();
    currentThread->Yield();
  }
}

static void
Consumer(void* _arg) {
  for (;;) {
    consumerSem->P();
    currentThread->Yield();
    Consume();
    producerSem->V();
    currentThread->Yield();
  }
}

void
ThreadTestProdConsSem()
{
    char** consumerNames = new char*[NUM_CONSUMERS];
    char** producerNames = new char*[NUM_PRODUCERS];
    int* products = new int[NUM_PRODUCERS];
    queue = new SynchList<int>;
    consumerSem = new Semaphore("Consumer", 0);
    producerSem = new Semaphore("Producer", QUEUE_SIZE);

    for (unsigned i = 0; i < NUM_CONSUMERS; i++) {
      consumerNames[i] = new char [4];
      snprintf(consumerNames[i], 4, "%u", i);
      Thread *t = new Thread(consumerNames[i]);
      t->Fork(Consumer, nullptr);
    }
    
    for (unsigned i = 0; i < NUM_PRODUCERS; i++) {
      producerNames[i] = new char [4];
      snprintf(producerNames[i], 4, "%u", i);
      products[i] = i;
      Thread *t = new Thread(producerNames[i]);
      t->Fork(Producer, products + i);
    }

    while (1) // El main no trabaja 
      currentThread->Yield();

    // No se llega nunca, pero queda bien ponerlo
    delete consumerSem;
    delete producerSem;

    for (unsigned i = 0; i < NUM_CONSUMERS; i++)
      delete consumerNames[i];
    delete [] consumerNames;

    for (unsigned i = 0; i < NUM_PRODUCERS; i++)
      delete producerNames[i];
    delete [] producerNames;

    delete queue;
}
