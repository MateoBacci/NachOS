#include "channel.hh"
#include "system.hh"
#include <cstring>

static const int THREADS_NUMBER = 3;
static const int LOOPS = 5;
Channel *channels[THREADS_NUMBER];

static void Communication(void *i) {
    int msgReceived;
    int threadNumber = (int)i;
    for (int i = 0; i < LOOPS; i ++) {
        if (threadNumber == 0) {
            channels[0]->Send(i);
            channels[2]->Receive(&msgReceived);
            ASSERT(msgReceived == i);
        } else {
            channels[threadNumber - 1]->Receive(&msgReceived);
            channels[(threadNumber + 1) % THREADS_NUMBER]->Send(msgReceived);
        }
    }
}

void ChannelTestRing() {
    char *positions[3] = {"1st", "2nd", "3rd"};
    for (int i = 0; i < THREADS_NUMBER; i++) {
        channels[i] = new Channel(positions[i]);
        Thread *t = new Thread(positions[i]);
        t->Fork(Communication, (void *)(unsigned)i);
    }
    for (int i = 0; i < THREADS_NUMBER; i++) {
        delete channels[i];
    }
}