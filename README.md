# Parallel Graph

## Description

This project implements a parallel graph traversal system in C. It utilizes a custom-built,
generic thread pool to compute the sum of all node values in a graph. The implementation focuses on efficient task
distribution and thread synchronization using POSIX threads (pthreads), effectively transitioning from a serial
recursive traversal to a scalable parallel architecture.

## Objectives

* Design and implement a generic, reusable **Thread Pool** in C.
* Utilize **Synchronization Primitives** (mutexes, condition variables, semaphores) to manage shared resources.
* Master the **POSIX Threads (pthreads)** API for concurrent execution.
* Analyze and bridge the performance gap between serial and parallel graph processing.

## Project Structure

* `src/`: Core implementation files, including the thread pool logic and parallel traversal algorithms.
* `utils/`: Logging and debugging utilities.
* `tests/`: Automated test suite, input graph files, and reference checkers.

## Implementation Details

### 1. Generic Thread Pool
The thread pool is designed to avoid the overhead of repeated thread creation. A fixed number of worker threads (**N**)
are spawned at initialization and persist throughout the program's lifecycle.



**Key Functions:**
* `create_threadpool()`: Initializes the pool and spawns worker threads.
* `enqueue_task()` / `dequeue_task()`: Thread-safe operations on a shared task queue.
* `wait_for_completion()`: Blocks until all tasks are finished and workers are idle.
* `destroy_threadpool()`: Gracefully shuts down the pool and joins threads.

### 2. Parallel Graph Traversal
The traversal uses a "divide and conquer" approach where processing a single node becomes a discrete task for the thread pool.



**Task Workflow:**
1. Atomic addition of the current node's value to the global sum.
2. Discovery of adjacent nodes.
3. Enqueuing new tasks for unvisited neighbors.

### 3. Synchronization
To ensure thread safety and avoid race conditions (especially on the global sum and task queue), the implementation employs:
* **Mutexes**: Protecting the shared task queue and graph node metadata.
* **Condition Variables**: Signaling workers when new tasks are available or when the graph processing is complete.

## Building and Running

### Compilation
Navigate to the `src/` directory and use the provided `Makefile`:
```
cd src/
make
```
This generates two binaries: serial (for baseline comparison) and parallel (the thread pool implementation).


## Testing

The testing infrastructure verifies both the final sum and the efficiency of the parallel 
execution. Navigate to the `tests/` directory:

```
cd tests/
make check
```

## Requirements
 - **Language:** C
 - **Libraries:** pthreads (POSIX Threads)
 - **Environment:** Linux-based systems




