#ifndef QUEUE_H
#define QUEUE_H
#include "tetris.h"

extern uint8_t _queue_limit;

typedef struct Node {
	T_Type t_type;
	struct Node* next;
} Node;

typedef struct Queue {
	Node *head;
	Node *tail;
} Queue;


Queue init_Queue();

Node* new_Node(T_Type t_type);
void enqueue(T_Type t_type, Queue* queue);
void dequeue(Queue* queue);

T_Type peek(Queue queue);

void print_Queue(Node* head);

#endif

