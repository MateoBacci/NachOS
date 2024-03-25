#include "thread_test_smokers.hh"
#include "lock.hh"
#include "condition.hh"
#include "thread.hh"
#include "system.hh"
#include "semaphore.hh"

#include <stdio.h>
#include <stdlib.h>


Semaphore *semTobacco, *semFire, *semPaper, *semAgain;
Semaphore *semDecision;


void smoke(int id){
    printf("Smoker '%d' is smoking\n",id);
}

void smokerFire(void* id){
    while(1){
    semDecision->P();
    if(semTobacco->TryP() != 0){   
        DEBUG('s',"SmokerFire tried to get tobacco\n"); 
        semDecision->V();
        currentThread->Yield();
        continue;
    }
    if(semPaper->TryP() != 0){
        DEBUG('s',"SmokerFire tried to get paper\n"); 
        semTobacco->V();
        semDecision->V();
        currentThread->Yield();
        continue;
    }
    smoke(1);
    semDecision->V();
    semAgain->V();
    }
}

void smokerPaper(void* id){    
    while(1){
    semDecision->P();

    if(semFire->TryP() != 0){     
        DEBUG('s',"smokerPaper tried to get fire\n");
        semDecision->V();
        currentThread->Yield();
        continue;
    }
    if(semTobacco->TryP() != 0){
        DEBUG('s',"smokerPaper tried to get tobacco\n");
        semFire->V();
        semDecision->V();
        currentThread->Yield();
        continue;
    }
    smoke(2);
    semDecision->V();
    semAgain->V();
    }
}
void smokerTobacco(void* id){    
    while(1){
    semDecision->P();
    if(semPaper->TryP() != 0){     
        DEBUG('s',"smokerTobacco tried to get paper\n");
        semDecision->V();
        currentThread->Yield();
        continue;
    }
    if(semFire->TryP() != 0){
        DEBUG('s',"smokerTobacco tried to get fire\n");
        semPaper->V();
        semDecision->V();
        currentThread->Yield();
        continue;
    }
    smoke(3);
    semDecision->V();
    semAgain->V();
    }
}



void agent(){
   
    int resource; 
    while(1){
        semAgain->P();
        resource = random()%3;
        if (resource!=0){ semFire->V();
        DEBUG('s',"agent released fire\n");
        }
        if (resource!=1){ semPaper->V();
        DEBUG('s',"agent released paper\n");
        }
        if (resource!=2){ semTobacco->V();
        DEBUG('s',"agent released tobacco\n");
        }
    }   
}

void ThreadTestSmokers(){
    semPaper = new Semaphore(NULL, 0);
    semTobacco = new Semaphore(NULL, 0);
    semFire = new Semaphore(NULL, 0);
    semAgain = new Semaphore(NULL, 1);

    semDecision = new Semaphore(NULL,1);

    Thread* smoker1 = new Thread("Smoker1");
    smoker1->Fork(smokerFire, NULL);
    Thread* smoker2 = new Thread("Smoker2");
    smoker2->Fork(smokerPaper, NULL); 
    Thread* smoker3 = new Thread("Smoker3");
    smoker3->Fork(smokerTobacco, NULL);

    agent();
    
    delete semPaper;
    delete semTobacco;
    delete semFire;
    delete semAgain;
    delete semDecision;
}

