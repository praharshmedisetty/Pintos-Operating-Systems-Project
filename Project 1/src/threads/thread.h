#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>


// Phase 3 Addition: MLFQS
// Including the fixedpoint header file to use fixed_point_t data type for 'recent_cpu'
#include "threads/fixedpoint.h"
// End Phase 3 Addition: MLFQS

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */


    // Phase 3 Addition: Priority Donation
    // defining actual_priority which stores the priority which will be used for priority donation purposes
    int actual_priority;

    // list of locks: contains the list of all locks which a threads holds 
    struct list locks;

    // pointer to identify the lock for which the thread is waiting
    struct lock *wait_lock;
    // End Phase 3 Addition: Priority Donation

    // Phase 3 Addition: MLFQS
    // declare an integer 'nice' that holds the niceness value of a thread. Value ranges between 20 and -20
    int nice;

    // recent cpu to measure how much CPU time each process has received recently
    // fixed_point_t declared in "threads/fixedpoint.h" header file
    fixed_point_t recent_cpu;
    // End Phase 3 Addition: MLFQS

    // Phase 2 Addition
    // Declaring a variable in the struct thread
    // int64_t : typedef signed long int64_t from the stdint.h library
    // thread_sleep_ticks: variable to hold ticks from OS boot time, until a thread unblocks from sleep
    int64_t thread_sleep_ticks;

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

// Phase 2 Addition
// Function to compare the thread sleep time of two threads
// declared this function in thread header (thread.h) instead of thread.c
// because timer.c uses the function using struct thread
bool compare_thread_sleep(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);

// Phase 3 Addition: Priority Donation
// function definition to implement priority donation for a thread 
void priority_donation(struct thread *, int);

// Function to return actual priority after priority donation
int get_actual_priority(struct thread *);

// Function declaration to compare priorities of two threads
bool compare_thread_priority(const struct list_elem *, const struct list_elem *, void *aux UNUSED);

// Phase 3 Addition: MLFQS
// Function Declaration to calculate 'recent_cpu'
void get_recent_cpu(struct thread *ready_list_thread, void *aux UNUSED);

// Function Declaration to calculate 'load_avg'
void get_load_avg(struct thread *ready_list_thread);

// Function Declaration to calculate new priority using the given formulas
void mlfqs_thread_priority(struct thread *ready_list_thread, void *aux UNUSED);
// End Phase 3 Addition: MLFQS

#endif /* threads/thread.h */