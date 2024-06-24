/* A simple inverted index implementation:
 *
 * Skeleton code written by Jianzhong Qi, May 2024
 * Edited by: [Add your name and student ID here for automatic recognition]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

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
	int id;
	double x_cood, y_cood;
	char categories[5][21];
	int category_cnt;
} poi_t;

typedef struct {
	/* add your query_t struct definition */
	double x_lb, y_lb, x_ub, y_ub;
	char query_category[21];
} query_t;

typedef struct {
	/* add your index_t struct definition */
	char category[21];
	int pois[51];
	int num_matched_pois;
} index_t;

typedef query_t data_t;								/* to be modified for Stage 2 */

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
void process_queries(list_t *query_list, poi_t* poi_array, int poi_cnt);

void print_stage_header(int stage_num);

int stage_one(poi_t* poi_arr);
void stage_two(poi_t* poi_arr, int poi_cnt);
int stage_three(char (*possible_categories)[21], poi_t *poi_arr, int poi_cnt);
void stage_four(char (*possible_categories)[21], int possible_category_cnt, poi_t* poi_arr, int poi_cnt);

/* add your own function prototypes here */

// determine whether a point lies in a query range
// (x, y) is the coordinate of that point
// (x_lb, y_lb, x_ub, y_ub) are coordinates of bottom-left and above-right endpoints
inline static int lie_in(double x, double y, double x_lb, double y_lb, double x_ub, double y_ub);


inline static int poi_lie_in_query_range(poi_t poi, query_t qry);


// determine whether query_category occurs in the array of categories which contains exactly category_cnt elements
int linear_query_matched(char *query_category, char (*categories)[21], int category_cnt);

// (used for qsort and bsearch) compare two strings which serve as members of a structure of type index_t
int cmp_asc_category_name(const void *v1, const void *v2);

/****************************************************************/

/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
	/* add variables to hold the input data */

	poi_t poi_array[51];
	int poi_cnt;
	/* stage 1: read POI records */
	poi_cnt = stage_one(poi_array);
	
	/* stage 2: read and process queries */
	stage_two(poi_array, poi_cnt);
	
	char possible_categories[51][21];
	int possible_category_cnt;
	/* stage 3: compute unique POI categories */
	possible_category_cnt = stage_three(possible_categories, poi_array, poi_cnt);
	
	/* stage 4: construct an inverted index */
	stage_four(possible_categories, possible_category_cnt, poi_array, poi_cnt);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* add your code below; you can also modify the function return type 
   and parameter list 
*/

/* stage 1: read POI records */



int 
stage_one(poi_t* poi_arr) {
	/* add code for stage 1 */
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	char temp_start_str[1024];
	scanf("%s", temp_start_str);
	char category_keyword[21];
	int record_cnt = 0;
	while(temp_start_str[0] >= '0' && temp_start_str[0] <= '9')
	{
		int poi_id = atoi(temp_start_str);
		// printf("%d ", poi_id);
		double x_cood, y_cood;
		scanf("%lf %lf", &x_cood, &y_cood);
		// printf("%.1lf %.1lf ", x_cood, y_cood);
		poi_arr[poi_id].id = poi_id;
		poi_arr[poi_id].x_cood = x_cood;
		poi_arr[poi_id].y_cood = y_cood;
		int category_count = 0;
		do
		{
			scanf("%s", category_keyword);
			// printf("%s ", category_keyword);
			if(category_keyword[0] == '#')
			{
				break;
			}
			strcpy(poi_arr[poi_id].categories[category_count++], category_keyword); 
		} while (1);
		poi_arr[poi_id].category_cnt = category_count;
		record_cnt++;
		// printf("\n");
		scanf("%s", temp_start_str);
	}
	

	int max_category_cnt = INT_MIN;
	int index_of_max_cat_cnt_record;
	for(int i = 0; i < record_cnt; i++)
	{
		if(max_category_cnt < poi_arr[i].category_cnt)
		{
			max_category_cnt = poi_arr[i].category_cnt;
			index_of_max_cat_cnt_record = i;
			continue;
		}
		if(max_category_cnt == poi_arr[i].category_cnt && i < index_of_max_cat_cnt_record)
		{
			index_of_max_cat_cnt_record = i;
		}
	}

	printf("Number of POIs: %d\n", record_cnt);
	printf("POI #%d has the largest number of categories:\n", index_of_max_cat_cnt_record);
	for(int i = 0; i < poi_arr[index_of_max_cat_cnt_record].category_cnt; i++)
	{
		if(i > 0)
		{
			printf(" ");
		}
		printf("%s", poi_arr[index_of_max_cat_cnt_record].categories[i]);
	}
	printf("\n");
	printf("\n");
	return record_cnt;
}

/* stage 2: read and process queries */

// determine whether a point lies in a query range in coordinate formats
// (x, y) is the coordinate of that point
// (x_lb, y_lb, x_ub, y_ub) are coordinates of bottom-left and above-right endpoints
inline static int lie_in(double x, double y, double x_lb, double y_lb, double x_ub, double y_ub)
{
	// printf("%.1lf %.1lf %.1lf %.1lf %.1lf %.1lf\n", x, y, x_lb, y_lb, x_ub, y_ub);
	if(x < x_lb || x > x_ub) return 0;
	if(y < y_lb || y > y_ub) return 0;
	return 1;
}

// determine whether a point lies in a query in structure formats
inline static int poi_lie_in_query_range(poi_t poi, query_t qry)
{
	// printf("cood: %.1lf %.1lf \n", poi.x_cood, poi.y_cood);
	return lie_in(poi.x_cood, poi.y_cood, qry.x_lb, qry.y_lb, qry.x_ub, qry.y_ub);
}

// determine whether a string occurs in a list of strings
int linear_query_matched(char *query_category, char (*categories)[21], int category_cnt)
{
	for(int i = 0; i < category_cnt; i++)
	{
		if(!strcmp(query_category, categories[i]))
		{
			return 1;
		}
	}
	return 0;
}

void 
stage_two(poi_t* poi_arr, int poi_cnt) {
	/* add code for stage 2 */
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	list_t* query_list = make_empty_list();

	double x_lb, y_lb, x_ub, y_ub;
	char query_category[21];
	while (scanf("%lf %lf %lf %lf %s", &x_lb, &y_lb, &x_ub, &y_ub, query_category) != EOF)
	{
		// printf("%.1lf %.1lf %.1lf %.1lf %s\n", x_lb, y_lb, x_ub, y_ub, query_category);
		query_t my_query;
		my_query.x_lb = x_lb;
		my_query.y_lb = y_lb;
		my_query.x_ub = x_ub;
		my_query.y_ub = y_ub;
		strcpy(my_query.query_category, query_category);
		insert_at_foot(query_list, my_query);
	}

	process_queries(query_list, poi_arr, poi_cnt);

	free_list(query_list);

	printf("\n");
}

/* stage 3: compute unique POI categories */
int 
stage_three(char (*possible_categories)[21], poi_t *poi_arr, int poi_cnt) {
	/* add code for stage 3 */
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);
	int possible_category_cnt = 0;
	for(int i = 0; i < poi_cnt; i++)
	{
		// char (*str)[21] = poi_arr[i].categories;
		for(int j = 0; j < poi_arr[i].category_cnt; j++)
		{
			// printf("%s ", poi_arr[i].categories[j]);
			char * str = poi_arr[i].categories[j];
			int found = 0;
			for(int k = 0; k < possible_category_cnt; k++)
			{
				found = !strcmp(str, possible_categories[k]);
				if(found)
				{
					break;
				}
			}
			if(!found)
			{
				strcpy(possible_categories[possible_category_cnt++], str);
			}
		}
	}

	printf("%d unique POI categories:\n", possible_category_cnt);
	for(int i = 0; i < possible_category_cnt; i++)
	{
		printf("%s", possible_categories[i]);
		if((i + 1) % 5 != 0 && i != possible_category_cnt - 1)
		{
			printf(", ");
		}
		else
		{
			printf("\n");
		}
	}
	printf("\n");
	return possible_category_cnt;
}

/* stage 4: construct an inverted index */

int
cmp_asc_category_name(const void *v1, const void *v2) {
	return strcmp(((index_t*)(v1))->category, ((index_t*)(v2))->category);
}

index_t* binary_query_matched_between_poi_and_query_index(char *query_category, index_t *index, int possible_category_cnt)
{
	index_t *p = bsearch(query_category, index, possible_category_cnt, sizeof(*index), cmp_asc_category_name);
	if(p != NULL)
	{
		// printf("\n%s is found at location %ld\n", query_category, p - index);
	}
	return p;
}

void stage_four(char (*possible_categories)[21], int possible_category_cnt, poi_t* poi_arr, int poi_cnt) {
	/* add code for stage 4 */
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

	index_t *index = malloc(sizeof(index_t) * possible_category_cnt);
	assert(index);

	// to do
	for(size_t i = 0; i < possible_category_cnt; i++)
	{
		strcpy(index[i].category, possible_categories[i]);
		index[i].num_matched_pois = 0;
	}

	qsort(index, possible_category_cnt, sizeof(*index), cmp_asc_category_name);


#if 1
	for(size_t i = 0; i < poi_cnt; i++) // go through all POIs
	{
		// printf("\npoi id: %d\n", poi_arr[i].id);
		for(size_t j = 0; j < poi_arr[i].category_cnt; j++) // go through all its categories of poi_arr[i]
		{
			// printf("\treception category: %s\n", index[j].category);
			index_t *p = binary_query_matched_between_poi_and_query_index(poi_arr[i].categories[j], index, possible_category_cnt);
			if(p)
			{
				p->pois[p->num_matched_pois++] = poi_arr[i].id;
			}
		}
	}
#endif

	for(size_t j = 0; j < possible_category_cnt; j++)
	{
		printf("%s:", index[j].category);
		for(size_t k = 0; k < index[j].num_matched_pois; k++)
		{
			printf(" %d", index[j].pois[k]);
		}
		printf("\n");
	}
	printf("\n");
	free(index);
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
process_queries(list_t *query_list, poi_t* poi_array, int poi_cnt) {
	/* add code to process queries */
	node_t *query_p = query_list->head;
	int j = 0;
	do
	{
		/* code */
		// printf("%.1lf %.1lf %.1lf %.1lf %s\n", data.x_lb, data.y_lb, data.x_ub, data.y_ub, data.query_category);
		printf("POIs in Q%d:", j++);
		int qualified_poi_cnt = 0;
		for(int i = 0; i < poi_cnt; i++)
		{
			poi_t poi = poi_array[i];
			if(!poi_lie_in_query_range(poi, query_p->data))
			{
				continue;
			}
			if(!linear_query_matched(query_p->data.query_category, poi.categories, poi.category_cnt))
			{
				continue;
			}
			qualified_poi_cnt++;
			printf(" %d", poi.id);
		}
		if(!qualified_poi_cnt)
		{
			printf(" none");
		}
		printf("\n");
		query_p = query_p->next;
	} while (query_p != query_list->foot->next);
	
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
