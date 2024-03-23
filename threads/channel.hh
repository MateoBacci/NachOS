// Implementacion de channel's para la comunicacion entre
// procesos.


#ifndef NACHOS_THREAD_CHANNEL_HH
#define NACHOS_THREAD_CHANNEL_HH
#endif

#include "thread.hh"
#include "semaphore.hh"
#include "lock.hh"

class Channel{
    public: 
        Channel(const char *debugName);

        ~Channel();

        const char* GetName();

        void Send(int message);

        void Receive(int* message);

    private:
        const char* name;

        int buffer;

        Semaphore *receiveSem, *senderSem;

        Lock *senderLock; 
};
