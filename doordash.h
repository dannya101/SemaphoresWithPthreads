//
// Created by Danny Aguilar on 4/14/24.
//

#ifndef ASSIGNMENT4_DOORDASH_H
#define ASSIGNMENT4_DOORDASH_H


#include "fooddelivery.h"
#include "Shared.h"
//these producer and consumer functions used in the pthreads
void *producerForPizza(void *);
void *producerForSandwich(void *);
void *deliveryA(void *);
void *deliveryB(void *);

#endif //ASSIGNMENT4_DOORDASH_H
