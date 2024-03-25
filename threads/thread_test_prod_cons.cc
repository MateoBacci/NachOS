/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.

#include "thread_test_prod_cons.hh"
#include "synch_list.hh"
#include "system.hh"

#include <stdio.h>

typedef int Product;
static SynchList<Product> *queue;

#define NUM_PRODUCERS 10
#define NUM_CONSUMERS 10
#define QUEUE_SIZE 5

#include "condition.hh"

static unsigned count = 0;
static Lock *consumerLock;
static Lock *producerLock;
static Condition *consumeCond;
static Condition *produceCond;

static void
Produce(unsigned n) {
  count++;
  queue->Append(n);
  printf("*** Producer `%s` writes %u\n", currentThread->GetName(), n);
}

static unsigned
Consume() {
  unsigned n;
  count--;
  n = queue->Pop();
  printf("*** Consumer `%s` reads %u\n", currentThread->GetName(), n);
  return n;
}

static void
Producer(void* _arg) {
  unsigned *n = (unsigned*) _arg;
  for (;;) {
    producerLock->Acquire();
    while (count == QUEUE_SIZE)
      produceCond->Wait();
    Produce(*n);
    currentThread->Yield();
    consumeCond->Signal();
    producerLock->Release();
    currentThread->Yield();
  }
}

static void
Consumer(void* _arg) {
  for (;;) {
    consumerLock->Acquire();
    while (count == 0)
      consumeCond->Wait();
    Consume();
    produceCond->Signal();
    consumerLock->Release();
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
    consumerLock = new Lock("Consumer");
    producerLock = new Lock("Producer");
    consumeCond = new Condition("Consumer", consumerLock);
    produceCond = new Condition("Producer", producerLock);

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
    delete consumerLock;
    delete producerLock;
    delete consumeCond;
    delete produceCond;
    for (unsigned i = 0; i < NUM_CONSUMERS; i++)
      delete consumerNames[i];

    for (unsigned i = 0; i < NUM_PRODUCERS; i++)
      delete producerNames[i];

    delete [] consumerNames;
    delete [] producerNames;
    delete queue;
}
