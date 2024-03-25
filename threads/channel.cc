#include "channel.hh"
#include "system.hh"

Channel::Channel(const char* debugname){
    name = debugname;
    receiveSem = new Semaphore("ReceiverSem",0);
    senderSem = new Semaphore("SenderSem",0);
    senderLock = new Lock("SenderLock");
}

Channel::~Channel(){
    delete receiveSem;
    delete senderSem;
    delete senderLock;
}

const char*
Channel::GetName(){
    return name;
}

void
Channel::Send(int message){
    senderLock->Acquire();
    DEBUG('c',"Thread '%s' sent message '%d' through channel '%s'.\n",
          currentThread->GetName(), message, name);
    receiveSem->V();
    currentThread->Yield();
    senderSem->P();
    senderLock->Release();
}

void
Channel::Receive(int* message){
    ASSERT(message != NULL);
    receiveSem->P();
    *message = buffer;
    DEBUG('c',"Thread '%s' received message '%d' through channel '%s'\n",
            currentThread->GetName(), buffer, name);
    senderSem->V();
}
