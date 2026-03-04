// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS		4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;

/* TODO: Define graph synchronization mechanisms. */
static pthread_mutex_t lock_sum, lock_visited;

/* TODO: Define graph task argument. */
typedef struct {
	unsigned int node_index;
	os_threadpool_t *tp;
} os_argument_t;

// function used to free the argument passed to the task
void destroy_argument(void *arg)
{
	os_argument_t *argument = (os_argument_t *)arg;

	// free the memory
	free(argument);
}

// function that processes adjacent nodes (it is the function passed to the threads)
static void process_adjacent_nodes(void *args)
{
	// get the threadpool and the current node
	os_argument_t *my_args = (os_argument_t *)args;

	os_threadpool_t *threadpool = my_args->tp;
	unsigned int node, i, neighbour_node;
	os_argument_t *new_args;

	node = my_args->node_index;

	// lock the sum variable while performing addition
	pthread_mutex_lock(&lock_sum);

	sum += graph->nodes[node]->info; // add the node to the sum

	pthread_mutex_unlock(&lock_sum); // unlock the mutex for the sum

	// traverse all the adjacent nodes
	for (i = 0; i < graph->nodes[node]->num_neighbours; i++) {
		neighbour_node = graph->nodes[node]->neighbours[i];

		// lock the visited array
		pthread_mutex_lock(&lock_visited);
		if (graph->visited[neighbour_node] == 0) {
			// mark the node as visited
			graph->visited[neighbour_node] = 1;
			pthread_mutex_unlock(&lock_visited); // unlock the visited array

			// create a new task
			os_task_t *new_task;

			// allocate the memory for the task
			new_args = calloc(1, sizeof(os_argument_t));
			new_args->node_index = neighbour_node;
			new_args->tp = threadpool;

			// create a new task and add it into the queue
			new_task = create_task(process_adjacent_nodes, (void *)new_args, destroy_argument);
			enqueue_task(threadpool, new_task);
		} else {
			pthread_mutex_unlock(&lock_visited); // unlock the visited array
		}
	}
}

// function that processed the initial node
static void process_node(unsigned int idx)
{
	os_argument_t *new_args;
	os_task_t *new_task;

	// lock the visited array
	pthread_mutex_lock(&lock_visited);

	// check if the node has been visited
	if (graph->visited[idx] == 0) {
		// set the node as visited
		graph->visited[idx] = 1;

		// unlock the visited array
		pthread_mutex_unlock(&lock_visited);

		new_args = calloc(1, sizeof(os_argument_t));
		new_args->node_index = idx;
		new_args->tp = tp;

		// create a new task and add it into the queue
		new_task = create_task(process_adjacent_nodes, (void *)new_args, destroy_argument);
		enqueue_task(tp, new_task);

	} else {
		pthread_mutex_unlock(&lock_visited);
	}
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	graph = create_graph_from_file(input_file);

	/* TODO: Initialize graph synchronization mechanisms. */
	pthread_mutex_init(&lock_sum, NULL); // mutex for the sum variable
	pthread_mutex_init(&lock_visited, NULL); // mutex for the visited array

	tp = create_threadpool(NUM_THREADS);
	process_node(0);
	wait_for_completion(tp);
	destroy_threadpool(tp);

	printf("%d", sum);

	// destroy the mutexes
	pthread_mutex_destroy(&lock_sum);
	pthread_mutex_destroy(&lock_visited);

	return 0;
}
