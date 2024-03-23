#include "channel.hh"
#include "system.hh"

Channel::Channel(const char* debugname){
    name = debugname;
    receiveSem = new Semaphore(NULL,0);
    senderLock = new Lock(NULL);
}

Channel::~Channel(){
    delete receiveSem;
    delete senderLock;
}

const char*
Channel::GetName(){
    return name;
}

void
Channel::Send(int message){
    senderLock->Acquire();
    buffer = message;
    DEBUG('c',"Thread '%s' sent message '%d' through channel '%s'.\n",
            currentThread->GetName(), buffer, name);
    receiveSem->V();
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
