//
// Created by Danny Aguilar on 4/14/24.
//

#include "doordash.h"
#include <ctime>
#include <unistd.h>
#include "Shared.h"
#include "log.h"

#define MSPERSEC	1000
//function that produces pizzas at a rate dependent on sleep time for pizza production
void *producerForPizza(void *voidPtr) {
    //creates a request added object and gains access to the shared data
    RequestAdded *item = nullptr;
    Shared *sharedData = (Shared *) voidPtr;
    while (true) {
        if(sharedData->counter >= sharedData->productionLimit){
            break;
        }
        item = new RequestAdded(); //Producing an item
        item->type = Pizza;
        //initializes new produced and inBrokerQueue for each request added
        item->produced = new unsigned int[2];
        item->inBrokerQueue = new unsigned int[2];
        //function will sleep based on time provided from user input for pizza
        usleep(sharedData->pizzaPTime * MSPERSEC);

        //checks if there are availableSlots before waiting on mutex
        sem_wait(&sharedData->availableSlots); //down
        sem_wait(&sharedData->mutex); //down
        //sandwich is pushed onto queue and mutex and unconsumed are signaled
        sharedData->buffer.push(item->type);
        sem_post(&sharedData->mutex); //up
        sem_post(&sharedData->unconsumed);
        //pizza produced is incremented and put into produced queue as well as sandwiches produced overall
        sharedData->pizzasProduced++;
        item->produced[Pizza] = sharedData->pizzasProduced;
        item->produced[Sandwich] = sharedData->sandwichProduced;
        //current pizzas is incremented and broker queue is set to current values of pizzas and sandwiches
        sharedData->currentPizzas++;
        item->inBrokerQueue[Pizza] = sharedData->currentPizzas;
        item->inBrokerQueue[Sandwich] = sharedData->currentSandwiches;
        //print statement to print requirements of RequestAdded added to buffer
        log_added_request(*item);
        //counter that represents how many items we produce
        sharedData->counter++;
    }
    return voidPtr;
}
void *producerForSandwich(void *voidPtr) {
    //creates a request added object and gains access to the shared data
    RequestAdded* item = nullptr;
    Shared *sharedData = (Shared *) voidPtr;
    while (true) {
        //breaks if production limit is reached
        if(sharedData->counter >= sharedData->productionLimit){
            break;
        }
        item = new RequestAdded();
        item->type = Sandwich;
        //initializes new produced and inBrokerQueue for each request added
        item->produced = new unsigned int[2];
        item->inBrokerQueue = new unsigned int[2];
        //function will sleep based on time provided from user input for sandwiches
        usleep(sharedData->sandwichPTime * MSPERSEC);
        //checks if there are availableSlots and availableSandwiches before waiting on mutex
        sem_wait(&sharedData->availableSandwiches);
        sem_wait(&sharedData->availableSlots); //down
        sem_wait(&sharedData->mutex);
        //sandwich is pushed onto queue and mutex and unconsumed are signaled
        sharedData->buffer.push(item->type);
        sem_post(&sharedData->mutex); //up
        sem_post(&sharedData->unconsumed);
        //sandwiches produced is incremented and put into produced queue as well as pizzas produced overall
        sharedData->sandwichProduced++;
        item->produced[Sandwich] = sharedData->sandwichProduced; //sets total produced
        item->produced[Pizza] = sharedData->pizzasProduced;
        //current sandwiches is incremented and broker queue is set to current values of pizzas and sandwiches
        sharedData->currentSandwiches++;
        item->inBrokerQueue[Pizza] = sharedData->currentPizzas;
        item->inBrokerQueue[Sandwich] = sharedData -> currentSandwiches;
        //print statement to print requirements of RequestAdded added to buffer
        log_added_request(*item);
        //counter that represents how many items we produce
        sharedData->counter++;
    }
    return voidPtr;
}
//delivers either pizzas or sandwiches based on the sleep time of delivery service A
void *deliveryA(void *voidPtr){
    //creates a request removed object and gains access to the shared data
    RequestRemoved* item = nullptr;
    Shared *sharedData = (Shared *) voidPtr;
    while (true) {
        //breaks if production limit is reached and queue is empty
        if(sharedData->counter > sharedData->productionLimit && sharedData->buffer.empty()){
            sem_post(&sharedData->precedenceConstraint);
            break;
        }
        //function will sleep based on time provided from user input for delivery A
        usleep(sharedData->deliveryATime * MSPERSEC);
        //checks if there are things in the queue using unconsumed
        sem_wait(&sharedData->unconsumed); //down
        //initializes new consumed and inBrokerQueue for each request removed
        item = new RequestRemoved();
        item->consumed = new unsigned int[2];
        item->inBrokerQueue = new unsigned int[2];
        //Access buffer exclusively and pops from buffer
        sem_wait(&sharedData->mutex); //down
        item->type = sharedData->buffer.front();
        item->consumer = DeliveryServiceA;
        sharedData->buffer.pop();
        //available slots and mutex are signaled after buffer is used
        sem_post(&sharedData->mutex); //up
        sem_post(&sharedData->availableSlots); //up
        if(item->type == Pizza) {
            //consumed pizzas are incremented and consumed is set to pizzas and sandwiches consumed overall
            sharedData->deliveryAConsume[Pizza]++;
            item->consumed[Pizza] = sharedData->deliveryAConsume[Pizza];
            item->consumed[Sandwich] = sharedData->deliveryAConsume[Sandwich];
            //current pizzas is decremented and broker queue is update with current pizzas and sandwiches
            sharedData->currentPizzas--;
            item->inBrokerQueue[Pizza] = sharedData->currentPizzas;
            item->inBrokerQueue[Sandwich] = sharedData->currentSandwiches;
        }
        else{
            //consumed pizzas are incremented and consumed is set to pizzas and sandwiches consumed overall
            sharedData->deliveryAConsume[Sandwich]++;
            item->consumed[Sandwich] = sharedData->deliveryAConsume[Sandwich];
            item->consumed[Pizza] = sharedData->deliveryAConsume[Pizza];
            //current sandwiches is decremented and available Sandwiches is signalled to represent that you are allowed one more available sandwich
            sem_post(&sharedData->availableSandwiches);
            sharedData->currentSandwiches--;
            //broker queue is updated with current pizzas and sandwiches
            item->inBrokerQueue[Pizza] = sharedData->currentPizzas;
            item->inBrokerQueue[Sandwich] = sharedData -> currentSandwiches;

        }
        //print statement to print requirements of RequestRemoved that was removed from buffer
        log_removed_request(*item);
    }
    return voidPtr;
}
//delivers either pizzas or sandwiches based on the sleep time of delivery service B
void *deliveryB(void *voidPtr){
    //creates a request removed object and gains access to the shared data
    RequestRemoved* item = nullptr;
    Shared *sharedData = (Shared *) voidPtr;
    while (true) {
        //breaks if production limit is reached and queue is empty
        if(sharedData->counter > sharedData->productionLimit && sharedData->buffer.empty()){
            sem_post(&sharedData->precedenceConstraint);
            break;
        }
        //function will sleep based on time provided from user input for delivery B
        usleep(sharedData->deliveryBTime * MSPERSEC);
        //checks if there are things in the queue using unconsumed
        sem_wait(&sharedData->unconsumed); //down
        //initializes new consumed and inBrokerQueue for each request removed
        item = new RequestRemoved();
        item->consumed = new unsigned int[2];
        item->inBrokerQueue = new unsigned int[2];
        // Access buffer exclusively and pops from buffer
        sem_wait(&sharedData->mutex); //down
        item->type = sharedData->buffer.front();
        item->consumer = DeliveryServiceB;
        sharedData->buffer.pop();
        //available slots and mutex are signaled after buffer is used
        sem_post(&sharedData->mutex); //up
        sem_post(&sharedData->availableSlots); //up
        if(item->type == Pizza) {
            //consumed pizzas are incremented and consumed is set to pizzas and sandwiches consumed overall
            sharedData->deliveryBConsume[Pizza]++;
            item->consumed[Pizza] = sharedData->deliveryBConsume[Pizza];
            item->consumed[Sandwich] = sharedData->deliveryBConsume[Sandwich];
            //current pizzas is decrement and broker queue is update with current pizzas and sandwiches
            sharedData->currentPizzas--;
            item->inBrokerQueue[Pizza] = sharedData->currentPizzas;
            item->inBrokerQueue[Sandwich] = sharedData -> currentSandwiches;
        }
        else{
            //consumed sandwiches are incremented and consumed is set to pizzas and sandwiches consumed overall
            sharedData->deliveryBConsume[Sandwich]++;
            item->consumed[Sandwich] = sharedData->deliveryBConsume[Sandwich];
            item->consumed[Pizza] = sharedData->deliveryBConsume[Pizza];
            //current sandwiches is decremented and available Sandwiches is signalled to represent that you are allowed one more available sandwich  ;
            sem_post(&sharedData->availableSandwiches);
            //broker queue is updated with current pizzas and sandwiches
            sharedData->currentSandwiches--;
            item->inBrokerQueue[Pizza] = sharedData->currentPizzas;
            item->inBrokerQueue[Sandwich] = sharedData -> currentSandwiches;
        }
        //print statement to print requirements of RequestRemoved that was removed from buffer
        log_removed_request(*item);
    }
    return voidPtr;
}