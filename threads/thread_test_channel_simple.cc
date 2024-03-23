#include "thread_test_channel_simple.hh"
#include "channel.hh"
#include "system.hh"
#include "thread.hh"
#include <cstring>
#include <iostream>

Channel *channel;
static bool done[2];

static void Echo(void *_a) {
  int msg; 
  if (!strcmp(currentThread->GetName(), "1st")) {
    std::cout << "Thread " << currentThread->GetName() << " sending " << 1 << std::endl;
    channel->Send(1);
    channel->Receive(&msg);
    std::cout << "Thread " << currentThread->GetName() << " received " << msg << std::endl;
    ASSERT(msg == 1);
    done[0] = true;
  } else {
    channel->Receive(&msg);
    std::cout << "Thread " << currentThread->GetName() << " received " << msg << std::endl;
    std::cout << "Thread " << currentThread->GetName() << " sending " << msg << std::endl;
    channel->Send(msg);
    done[1] = true;
  }
}

void ThreadTestChannelSimple() {
  done[0] = done[1] = false;
  channel = new Channel("Channel");
  Thread *t1 = new Thread("1st");
  t1->Fork(Echo, (void *)NULL);
  Thread *t2 = new Thread("2nd");
  t2->Fork(Echo, (void *)NULL);
  for (unsigned i = 0; i < 2; i++) {
    while (!done[i]) {
      currentThread->Yield();
    }
  }
  delete channel;
}







