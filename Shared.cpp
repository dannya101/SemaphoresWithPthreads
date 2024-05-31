//
// Created by Danny Aguilar on 4/13/24.
//

#include "Shared.h"
#include "fooddelivery.h"


Shared::Shared(queue<RequestType> theQ, int pizzaPTime, int sandwichPTime, int deliveryATime, int deliveryBTime, int productionLimit) {
    //buffer queue is set to the queue passed in
    buffer = theQ;
    //availableSlots and availableSandwiches used to check that foods can be produced
    //mutex allows the buffer to be exclusively accessed 1 representing that the lock has not been accessed and 0 representing the mutex is currently being used
    //unconsumed mirrors availableSlots
    //precedenceConstraint is set to 0 and waits in main until signaled after all pthead completion
    sem_init(&availableSlots, 0, 20);
    sem_init(&availableSandwiches, 0, 8);
    sem_init(&mutex, 0, 1);
    sem_init(&unconsumed, 0, 0);
    sem_init(&precedenceConstraint, 0, 0);
    //values user inputs
    this->pizzaPTime = pizzaPTime;
    this->sandwichPTime = sandwichPTime;
    this->deliveryATime = deliveryATime;
    this->deliveryBTime = deliveryBTime;
    this->productionLimit = productionLimit;
    //counting total pizzas and sandwiched produced overall and both together
    this->pizzasProduced = 0;
    this->sandwichProduced = 0;
    this->counter = 1;
    //keeps track of current sandwich and pizzas in buffer
    this->currentPizzas = 0;
    this->currentSandwiches = 0;
    //initializing the array that keeps track of how much delivery A and B consume
    deliveryAConsume = new unsigned int[2];
    for(int idx = 0; idx < 2; idx++)
    {
        deliveryAConsume[idx] = 0;
    }
    deliveryBConsume = new unsigned int[2];
    for(int idx = 0; idx < 2; idx++)
    {
        deliveryBConsume[idx] = 0;
    }
}
