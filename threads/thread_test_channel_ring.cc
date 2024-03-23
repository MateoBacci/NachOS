#include "channel.hh"
#include "system.hh"
#include <cstring>
#include <cstdio>
#include <iostream>

static const int THREADS_NUMBER = 3;
static const char *threadName[THREADS_NUMBER] = {"1st", "2nd", "3rd"};
static const char *channelsName[THREADS_NUMBER] = {"1to2", "2to3", "3to1"};
static bool done[THREADS_NUMBER];

Channel *channels[THREADS_NUMBER];
static const int LOOPS = 5;

static void Communication(void *i) {
    int msgReceived;
    int threadNumber = (unsigned long)i;
    for (int j = 0; j < LOOPS; j++) {
        if (threadNumber == 0) {
            std::cout << "Thread " << currentThread->GetName() << " sending " << j << std::endl;
            channels[0]->Send(j);
            //currentThread->Yield();
            channels[THREADS_NUMBER - 1]->Receive(&msgReceived);
            ASSERT(msgReceived == j);
            std::cout << "Thread " << currentThread->GetName() << " received " << msgReceived << std::endl;
        } else {
            channels[threadNumber - 1]->Receive(&msgReceived);
            std::cout << "Thread " << currentThread->GetName() << " received " << msgReceived << std::endl;
            //currentThread->Yield();
            std::cout << "Thread " << currentThread->GetName() << " sending " << msgReceived << std::endl;
            channels[threadNumber]->Send(msgReceived);
        }
        //currentThread->Yield();
    }

    done[threadNumber] = true;
}

  
void ThreadTestChannelRing() {
    for (int i = 0; i < THREADS_NUMBER; i++) {
        channels[i] = new Channel(channelsName[i]);
        ASSERT(channels[i] != NULL);
    }
    for (int i = 0; i < THREADS_NUMBER; i++) {
        done[i] = false;
        Thread *t = new Thread(threadName[i]);
        ASSERT(t != NULL);
        std::cout << "Thread '" << threadName[i] << "' created." << std::endl;
        t->Fork(Communication, (void *)(unsigned long)i);
    }
    puts("Start communication!\n\n");
    for (unsigned i = 0; i < THREADS_NUMBER; i++) {
      while (!done[i]) {
        currentThread->Yield();
      }
    }
    for (int i = 0; i < THREADS_NUMBER; delete channels[i++]);
}
