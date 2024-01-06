/*
 * fila.h
 *
 *  Created on: Oct 14, 2023
 *      Author: lucas
 */

#ifndef INC_FILA_H_
#define INC_FILA_H_

#include "Parse.h"


// Define your custom struct
struct MyStruct {
//    int data;
    uint8_t data[9];
    // Add more fields as needed
};

// Define a queue structure
struct Queue;

// Declare the queue variable as extern


// Function to initialize an empty queue
struct Queue* createQueue();

// Function to check if the queue is empty
int isEmpty(struct Queue* queue);

// Function to add an element to the rear of the queue
void enqueue(struct Queue* queue, struct MyStruct item);

// Function to remove and return an element from the front of the queue
struct MyStruct dequeue(struct Queue* queue);

// Function to free the memory used by the queue
void freeQueue(struct Queue* queue);

#endif
