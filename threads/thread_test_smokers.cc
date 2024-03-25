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
Lock *lockDecision;

int remainingRes;

void smoke(int id){
    printf("Smoker '%d' is smoking\n",id);
    remainingRes = 1;
}


void smokerFire(void* id){
    printf("smokerFire\n");    
    while(1){
    semDecision->P();
    if(remainingRes == 1){    
        semTobacco->V();
    }
    
    semTobacco->P();
    semPaper->P();
    smoke(1);
    semAgain->V();
    }
}

void smokerPaper(void* id){
    printf("smokerPaper\n");    
    while(1){
    semDecision->P();
    if(remainingRes == 0){    
        semDecision->V();
        continue;    
    }
    semFire->P();
    semTobacco->P();
    smoke(2);
    semAgain->V();
    }
}


void smokerTobacco(void* id){
    printf("smokerTobacco\n");    
    while(1){
    semDecision->P();
    if(remainingRes == 0){    
        semDecision->V();
        continue;    
    }
    semPaper->P();
    semFire->P();
    smoke(3);
    semAgain->V();
    }
}


void agent(){
   
    int resource; 
    while(1){
        DEBUG('s',"in agent");
        semAgain->P();
        resource = random()%3;
        if (resource!=0) semFire->V();
        if (resource!=1) semPaper->V();
        if (resource!=2) semTobacco->V();
        remainingRes =0;
        currentThread->Yield();
    }   
}

void ThreadTestSmokers(){
    printf("en test\n");
    semPaper = new Semaphore(NULL, 0);
    semTobacco = new Semaphore(NULL, 0);
    semFire = new Semaphore(NULL, 0);
    semAgain = new Semaphore(NULL, 2);

    semDecision = new Semaphore(NULL,1);
    remainingRes = 1;

    printf("semaforos listos\n");

    Thread* smoker1 = new Thread("Smoker1");
    smoker1->Fork(smokerFire, NULL);
    Thread* smoker2 = new Thread("Smoker2");
    smoker2->Fork(smokerPaper, NULL); 
    Thread* smoker3 = new Thread("Smoker3");
    smoker3->Fork(smokerTobacco, NULL);

    printf("hilos enviados\n");
    agent();
    
    delete semPaper;
    delete semTobacco;
    delete semFire;
    delete semAgain;
    delete semDecision;
}

