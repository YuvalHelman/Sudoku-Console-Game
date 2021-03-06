#include <stdio.h>
#include <stdlib.h>
#include "move_list.h"

/*
		This module implements the sudoku's double-linked List and any functions regarding manipulating data on the list.
		Adding new nodes, removing nodes, removing all of the list, traversing the list etc.
*/
/*						Implementation:
		HEAD--> 1st move--> 2nd move--> ........
	The head is meant to signal the "first view" of the sudoku before any moves were made.
	"curr_node == HEAD" means no more UNDO's are possible

	the nodes are implemented in a way that lets us traverse the list and change values accordingly
	so we can also change the game_board each time while using maximum efficiency.
*/

/* Global Variables: */
List *move_list = NULL;


/* Private functions decleration */


/*
*   This function is used for deleting a node while free'ing all its content.
*   should be used in any function that deletes a part of the list.
*
*   @returns: EXIT_SUCCESS(0) on deleting the node successfuly.
*			 on any error returns EXIT_FAILURE(1) and prints the error.
*/
int node_delete(Node *node);

/* Private functions */

int node_delete(Node *node) {
	if (!node) {
		return EXIT_SUCCESS;
	}

	free(node->values);
	free(node);

	return EXIT_SUCCESS;
}


/* Public functions */


node_vals* redo_list(int* num_of_values) {
	Node* curr_node;

	if (sudoku.game_mode == init) {/* only available in edit/solve mode */
		printf("ERROR: invalid command\n");
		return NULL;
	}
	if (move_list->current_Node_move == move_list->tail) { /* case d */
		printf("Error: no moves to redo\n");
		return NULL;
	}

	/* Set the current_node_move pointer to the next node in the list */
	move_list->current_Node_move = move_list->current_Node_move->next;

	curr_node = (Node*)move_list->current_Node_move;

	(*num_of_values) = curr_node->num_of_values;
	return (node_vals*)curr_node->values;
}

node_vals* undo_list(int* num_of_values, int reset_flag) {
	Node* curr_node;

	curr_node = move_list->current_Node_move;

	if (sudoku.game_mode == init) { /* only available in edit/solve mode */
		printf("ERROR: invalid command\n");
		return NULL;
	}
	if (curr_node == move_list->head) {
		if (reset_flag == true) {
			(*num_of_values) = -1;
			return NULL;
		}
		else {
			printf("Error: no moves to undo\n");
			return NULL;
		}
	}

	/* Set the node pointer to its previous node in the list */
	move_list->current_Node_move = move_list->current_Node_move->prev;

	(*num_of_values) = curr_node->num_of_values;
	return (node_vals*)curr_node->values;
	
}

int delete_list_full()
{
	Node *curr, *next;
	/* Check for atleast one Node in the list (other then the head) */
	if (move_list == NULL || move_list->head == NULL || move_list->head->next == NULL) {
		return EXIT_SUCCESS;
	}

	curr = move_list->head->next;
	while (curr != NULL) { /* free all of the nodes in the list */
		next = curr->next;
		node_delete(curr);
		curr = next;
	}

	move_list->head->next = NULL;
	move_list->current_Node_move = move_list->head;
	move_list->tail = move_list->head;

	return EXIT_SUCCESS;
}

int delete_list_from_the_current_node() {
	Node *curr, *next;

	/* Check for atleast one Node in the list (besides the head) */
	if (move_list == NULL ||
		move_list->head == NULL ||
		move_list->head->next == NULL ||
		move_list->current_Node_move == move_list->tail) /* if the current_node is the tail, there's nothing to erase */{
		return EXIT_SUCCESS;
	}

	/* free all of the nodes in the list positioned right from the current_Node_move */
	curr = move_list->current_Node_move->next;
	while (curr != NULL) { 
		next = curr->next;
		node_delete(curr);
		curr = next;
	}

	/* Change the current_Node_move to be the new tail of the list */
	move_list->current_Node_move->next = NULL;
	move_list->tail = move_list->current_Node_move;

	return EXIT_SUCCESS;
}

int add_new_node(int row_arg, int column_arg, int prev_val_arg, int updated_val_arg) {
	Node* node_ptr;
	node_vals *vals_array;

	/* Build a new node and attach to list */
	node_ptr = (Node*)malloc(SIZE_OF_NODE);
	if (!node_ptr) {
		printf("malloc failed in add_new_node() function. Exiting\n");
		exit(EXIT_FAILURE);
	}
	

	node_ptr->values = (node_vals*)calloc(1, SIZE_OF_NODE_VALS);
	if (!node_ptr->values) {
		printf("malloc failed in add_new_node() function. Exiting\n");
		exit(EXIT_FAILURE);
	}
	node_ptr->num_of_values = 1;

	vals_array = node_ptr->values;

	/* Edit the arguments for the value that has been changed */
	vals_array[0].row = row_arg;
	vals_array[0].column = column_arg;
	vals_array[0].prev_val = prev_val_arg;
	vals_array[0].updated_val = updated_val_arg;


	/* Delete a part of the list if the current_node isn't the tail */
	delete_list_from_the_current_node();

	/* Connect the node to the list */
	node_ptr->next = NULL;
	node_ptr->prev = move_list->tail;

	move_list->tail->next = node_ptr; /* set the tail's next pointer to be the new node */
	move_list->tail = node_ptr; /* Set as new Tail of the move list */
	move_list->current_Node_move = move_list->tail; /* set the current pointer to be the new tail */

	return EXIT_SUCCESS;

}

int add_val_to_current_node(int row_arg, int column_arg, int prev_val_arg, int updated_val_arg) {
	Node* node_ptr;
	node_vals *vals_array;
	int last_array_index;

	node_ptr = move_list->current_Node_move;
	last_array_index = node_ptr->num_of_values; /* the index of the last spot in the array (after the realloc) */

	node_ptr->values = (node_vals*)realloc(node_ptr->values, (node_ptr->num_of_values + 1)*(SIZE_OF_NODE_VALS));
	if (!node_ptr->values) {
		printf("relloc failed in add_val_to_current_node function. Exiting\n");
		exit(EXIT_FAILURE);
	}

	vals_array = node_ptr->values;

	/* Edit the arguments for the value that has been changed */
	(vals_array[last_array_index]).row = row_arg;
	(vals_array[last_array_index]).column = column_arg;
	(vals_array[last_array_index]).prev_val = prev_val_arg;
	vals_array[last_array_index].updated_val = updated_val_arg;

	(node_ptr->num_of_values)++;

	return EXIT_SUCCESS;
}

int initialize_list_parameters() {
	Node* head_ptr;

	move_list = (List*)malloc(SIZE_OF_LIST);
	if (move_list == NULL) {
		printf("malloc failed in initialize_list_parameters function. Exiting\n");
		exit(EXIT_FAILURE);
	}
	head_ptr = (Node*)calloc(NODE_NUM_OF_PTRS, sizeof(int));
	if (head_ptr == NULL) {
		printf("calloc failed in initialize_list_parameters function. Exiting\n");
		exit(EXIT_FAILURE);
	}
	move_list->head = head_ptr;
	move_list->current_Node_move = head_ptr;
	move_list->tail = head_ptr;

	return EXIT_SUCCESS;
}

void delete_list_on_exit() {
	delete_list_full();
	free(move_list->head);
	free(move_list);
}

