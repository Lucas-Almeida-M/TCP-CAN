#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

// Define a node structure for the queue
struct QueueNode {
    struct MyStruct data;
    struct QueueNode* next;
};

// Define a queue structure
struct Queue {
    struct QueueNode* front;
    struct QueueNode* rear;
};

// Function to initialize an empty queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    if (queue == NULL) {
        perror("Queue creation failed");
        exit(EXIT_FAILURE);
    }
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to check if the queue is empty
int isEmpty(struct Queue* queue) {
    return (queue->front == NULL);
}

// Function to add an element to the rear of the queue
void enqueue(struct Queue* queue, struct MyStruct item) {
    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    if (newNode == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->data = item;
    newNode->next = NULL;

    if (isEmpty(queue)) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Function to remove and return an element from the front of the queue
struct MyStruct dequeue(struct Queue* queue) {
    if (isEmpty(queue)) {
//        perror("Queue is empty");
//        exit(EXIT_FAILURE);
    	struct MyStruct t = {0};
    	return  t;
    }

    struct QueueNode* temp = queue->front;
    struct MyStruct item = temp->data;
    queue->front = queue->front->next;
    free(temp);

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return item;
}

// Function to free the memory used by the queue
void freeQueue(struct Queue* queue) {
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}
