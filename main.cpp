#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include "fooddelivery.h"
#include "doordash.h"
#include "Shared.h"
#include <unistd.h>
#include "log.h"



using namespace std;

int main(int argc, char **argv) {
    //setting default values in case not defined in user input
    int productionLimit = 100;
    int pizzaSleepTime = 0;
    int sandwichSleepTime = 0;
    int deliveryAtime = 0;
    int deliveryBtime = 0;
    int option;
    //getopt used to get arguments from user input
    while ( (option = getopt(argc, argv, "n:a:b:p:s:")) != -1) {
        switch (option) {
            case 'n':
                productionLimit = atoi(optarg);
                break;
            case 'a':
                deliveryAtime = atoi(optarg);
                break;
            case 'b':
                deliveryBtime = atoi(optarg);
                break;
            case 'p':
                pizzaSleepTime = atoi(optarg);
                break;
            case 's':
                sandwichSleepTime = atoi(optarg);
                break;
            default:
                exit(1);
        }
    }
    //queue filled with RequestType from fooddelivery.h
    queue<RequestType> buffer;
    pthread_t producer1;
    pthread_t producer2;
    pthread_t delivA;
    pthread_t delivB;
    //new shared object that creates all shared data using user input or default values
    Shared *sharedData = new Shared(buffer, pizzaSleepTime, sandwichSleepTime, deliveryAtime, deliveryBtime, productionLimit);
    //creating all pthreads at the same time
    if (pthread_create(&producer1, NULL, &producerForPizza, (void*)sharedData))
    {
        //error handling
    }
    if (pthread_create(&producer2, NULL, &producerForSandwich, (void*)sharedData))
    {
        //error handling
    }
    if (pthread_create(&delivA, NULL, &deliveryA, (void*)sharedData))
    {
        //error handling
    }
    if (pthread_create(&delivB, NULL, &deliveryB, (void*)sharedData))
    {
        //error handling
    }
    //precedenceConstraint is able to wait when the consumer thread is done which is when it will be signalled to pass
    sem_wait(&sharedData->precedenceConstraint);
    //making produced and consumed are made using information from sharedData to print the history
    unsigned int produced[2] = {sharedData->pizzasProduced, sharedData->sandwichProduced};
    unsigned int *consumed[2] = {sharedData->deliveryAConsume, sharedData->deliveryBConsume};
    log_production_history(produced, consumed);
    return 0;
}

