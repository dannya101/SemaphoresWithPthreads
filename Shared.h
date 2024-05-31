//
// Created by Danny Aguilar on 4/13/24.
//

#ifndef ASSIGNMENT4_SHARED_H
#define ASSIGNMENT4_SHARED_H
#include <queue>
#include <semaphore.h>
#include "fooddelivery.h"


using namespace std;

//all variables need for all the pthreads to run in unison and efficiently
class Shared {
public:
    sem_t availableSlots;
    sem_t availableSandwiches;
    queue<RequestType> buffer;
    sem_t mutex;
    sem_t unconsumed;
    sem_t precedenceConstraint;
    int pizzaPTime;
    int sandwichPTime;
    int deliveryATime;
    int deliveryBTime;
    unsigned int* deliveryAConsume;
    unsigned int* deliveryBConsume;
    unsigned int pizzasProduced;
    unsigned int sandwichProduced;
    int productionLimit;
    int counter;
    int currentPizzas;
    int currentSandwiches;
    //defining the shared constructor that will represent shared data between pthreads
    Shared(queue<RequestType> theQ, int pizzaPTime, int sandwichPTime, int deliveryATime, int deliveryBTime, int productionLimit);
};


#endif //ASSIGNMENT4_SHARED_H
