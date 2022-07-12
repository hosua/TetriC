#include "queue.h"

uint8_t _queue_limit = 7;

Node* new_Node(T_Type t_type){
	Node* new_node = malloc(sizeof(Node));
	new_node->t_type = t_type;
	new_node->next = NULL;
	return new_node;
}

Queue init_Queue(){
	Queue queue;
	queue.tail = queue.head = NULL;
	return queue;
}

T_Type peek(Queue queue){
	return queue.head->t_type;
}

// Enqueue to tail
void enqueue(T_Type t_type, Queue* queue){
	Node* new_node = new_Node(t_type);
	if (!queue->tail){
		queue->tail = queue->head = new_node;
	} else {
		queue->tail->next = new_node;
		queue->tail = new_node;
	}
}

// Dequeue from head
void dequeue(Queue* queue){
	Node *temp = NULL;	
	if (queue->head){
		temp = queue->head;
		queue->head = queue->head->next;
	}
	free(temp);
}

void print_Queue(Node* head){
	if (head){
		printf("T_Type: %s\n", T_Type_to_str(head->t_type));
		print_Queue(head->next);
	}

}
