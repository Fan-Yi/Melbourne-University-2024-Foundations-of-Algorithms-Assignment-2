/* A simple inverted index implementation:
 *
 * Skeleton code written by Jianzhong Qi, May 2024
 * Edited by: [Add your name and student ID here for automatic recognition]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STAGE_NUM_ONE 1							/* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n"	/* stage header format string */

#define DATA_SET_SIZE 50						/* number of input POIs */
#define DATA_DIM 2								/* POI point dimensionality */
#define QUERY_BOUNDS 4							/* query range bounds */

typedef struct {
	/* add your poi_t struct definition */
} poi_t;

typedef struct {
	/* add your query_t struct definition */
} query_t;

typedef struct {
	/* add your index_t struct definition */
} index_t;

typedef int data_t;								/* to be modified for Stage 2 */

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
list_t *insert_at_foot(list_t *list, data_t value);
void process_queries();

void print_stage_header(int stage_num);

void stage_one();
void stage_two();
void stage_three();
void stage_four();

/* add your own function prototypes here */

/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
	/* add variables to hold the input data */

	/* stage 1: read POI records */
	stage_one();
	
	/* stage 2: read and process queries */
	stage_two();
	
	/* stage 3: compute unique POI categories */
	stage_three();
	
	/* stage 4: construct an inverted index */
	stage_four();
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* stage 1: read POI records */
void 
stage_one() {
	/* add code for stage 1 */
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	printf("\n");
}

/* stage 2: read and process queries */
void 
stage_two() {
	/* add code for stage 2 */
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);
	
	process_queries();

	printf("\n");
}

/* stage 3: compute unique POI categories */
void 
stage_three() {
	/* add code for stage 3 */
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);

	printf("\n");
}

/* stage 4: construct an inverted index */
void stage_four() {
	/* add code for stage 4 */
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t
*make_empty_list(void) {
	list_t *list;

	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;

	return list;
}

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
	node_t *curr, *prev;

	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(list);
}

/* insert a new data element into the end of a linked list
*/
list_t
*insert_at_foot(list_t *list, data_t value) {
	node_t *new;

	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->data = value;
	new->next = NULL;

	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}

	return list;
}

/* process queries in a list (or in an array if using array-based implementation) */
void
process_queries() {
	/* add code to process queries */

}

/****************************************************************/
/*
	Using N to denote the number of POIs, 
	C to denote the maximum number of categories per POI, 
	L to denote the maximum length of a category, and
	U to denote the number of all unique POI categories:

	(1) What is the worst-case time complexity to process a single query 
	using the linear search algorithm in Stage 2, and why?
	
	(2) What is the worst-case time complexity to process a single query 
	using the query algorithm described in Stage 4, and why? 

*/
