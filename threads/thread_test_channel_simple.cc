#include "thread_test_channel_simple.hh"
#include "channel.hh"
#include "system.hh"
#include "thread.hh"
#include <cstring>
#include <iostream>

static const int THREADS_NUMBER = 4;
static const char *threadName[THREADS_NUMBER] = {"1st", "2nd", "3rd", "4th"};
static bool done[THREADS_NUMBER];
Channel *channel;

static void Communication(void *a) {
  int msg; 
  int threadNumber = (unsigned long)a;
  if (threadNumber <= 1) {
    std::cout << "Thread '" << currentThread->GetName() << "' sending " << threadNumber << std::endl;
    channel->Send(threadNumber);
    channel->Receive(&msg);
    std::cout << "Thread '" << currentThread->GetName() << "' received " << msg << std::endl;
  } else {
    channel->Receive(&msg);
    std::cout << "Thread '" << currentThread->GetName() << "' received " << msg << std::endl;
    std::cout << "Thread '" << currentThread->GetName() << "' sending " << msg << std::endl;
    channel->Send(msg);
  }
  done[threadNumber] = true;
}

void ThreadTestChannelSimple() {
    channel = new Channel("Channel");
    for (int i = 0; i < THREADS_NUMBER; i++) {
        done[i] = false;
        Thread *t = new Thread(threadName[i]);
        ASSERT(t != NULL);
        std::cout << "Thread '" << threadName[i] << "' created." << std::endl;
        t->Fork(Communication, (void *)(unsigned long)i);
    }
    for (unsigned i = 0; i < THREADS_NUMBER; i++) {
      while (!done[i]) {
        currentThread->Yield();
      }
    }
    delete channel;
}







