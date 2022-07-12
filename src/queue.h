#ifndef QUEUE_H
#define QUEUE_H
#include "tetris.h"

typedef struct Node {
	T_Type t_type;
	struct Node* next;
} Node;

typedef struct Queue {
	Node *head;
	Node *tail;
	uint8_t limit;
} Queue;

Queue init_Queue(uint8_t queue_limit);

Node* new_Node(T_Type t_type);
void enqueue(T_Type t_type, Queue* queue);
T_Type dequeue(Queue* queue);

void print_Queue(Node* head);

#endif // QUEUE_H

