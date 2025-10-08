# Threads

<!-- TOC -->

- [Threads](#threads)
  - [Defining a thread stack](#defining-a-thread-stack)
  - [Starting a thread](#starting-a-thread)
  - [Thread priorities](#thread-priorities)
  - [Thread options](#thread-options)
    - [Thread custom data](#thread-custom-data)
  - [Mutexes](#mutexes)
    - [Defining a mutex](#defining-a-mutex)
    - [Locking a mutex](#locking-a-mutex)
    - [Releasing a mutex](#releasing-a-mutex)

<!-- /TOC -->

A thread is a kernel object that is used for application processing that is too lengthy or too complex to be performed by an ISR.

A thread has the following key properties:

- A **stack area**, which is a region of memory used for the thread’s stack. The size of the stack area can be tailored to conform to the actual needs of the thread’s processing. Special macros exist to create and work with stack memory regions.

- A **thread control block** for private kernel bookkeeping of the thread’s metadata. This is an instance of type k_thread.

- An **entry point function**, which is invoked when the thread is started. Up to 3 argument values can be passed to this function.

- A **scheduling priority**, which instructs the kernel’s scheduler how to allocate CPU time to the thread. (See Scheduling.)

- A set of **thread options**, which allow the thread to receive special treatment by the kernel under specific circumstances. (See Thread Options.)

- A **start delay**, which specifies how long the kernel should wait before starting the thread.

- An **execution mode**, which can either be supervisor or user mode. By default, threads run in supervisor mode and allow access to privileged CPU instructions, the entire memory address space, and peripherals. User mode threads have a reduced set of privileges. This depends on the CONFIG_USERSPACE option. See User Mode.

## Defining a thread stack

In Zephyr, each thread requires its own stack for storing local variables, function call data, and more. The `K_THREAD_STACK_DEFINE` macro is used to allocate memory for the stack of the blink_thread:

```c
‎#define BLINK_THREAD_STACK_SIZE 256‎
K_THREAD_STACK_DEFINE(blink_stack, BLINK_THREAD_STACK_SIZE);‎

```

Here, `BLINK_THREAD_STACK_SIZE` specifies the size of the stack (in bytes). The stack size should be chosen carefully based on the memory requirements of the thread.
Zephyr incourages defining stack size so that the memory for the stack is allocated in a deterministic way. This means we can calculate the exact execution time for the thread. This also prevent stack from overflowing in adjacent memory areas.
The stack used by the main thread is already defined by Zephyr settings, the setting can be changed in the configuration.

Zephyr stores thread information in a specific struct:

```c
static struct k_thread blink_thread;
```

The thread entrypoint must have 3 void pointers parameters in it's definition:

```c
void blink_thread_start(void *arg_1, void *arg_2, void *arg_3)
```

_It is usually not reccomended to print to the console from multiple threads, a possible soultion is to push the messages on a queue and then print from the queue._

Thread must be stopped at the end of the loop to let other threads run:

```c
k_msleep(blink_sleep_ms);
```

To identify a thread, Zephyr offers a custom variable:

```c
 k_tid_t blink_tid;
```

Keeping track of a thread id allows you to control the thread, stop it, kill it and so on.

## Starting a thread

```c
 // Start the blink thread
    blink_tid = k_thread_create(&blink_thread,          // Thread struct
                                blink_stack,            // Stack
                                K_THREAD_STACK_SIZEOF(blink_stack),
                                blink_thread_start,     // Entry point
                                NULL,                   // arg_1
                                NULL,                   // arg_2
                                NULL,                   // arg_3
                                7,                      // Priority
                                0,                      // Options
                                K_NO_WAIT);             // Delay

```

## Thread priorities

A thread’s priority is an integer value, and can be either negative or non-negative. Numerically lower priorities takes precedence over numerically higher values. For example, the scheduler gives thread A of priority 4 higher priority over thread B of priority 7; likewise thread C of priority -2 has higher priority than both thread A and thread B. The scheduler distinguishes between two classes of threads, based on each thread’s priority.

- A **cooperative thread** has a negative priority value. Once it becomes the current thread, a cooperative thread remains the current thread until it performs an action that makes it unready.

- A **preemptible thread** has a non-negative priority value. Once it becomes the current thread, a preemptible thread may be supplanted at any time if a cooperative thread, or a preemptible thread of higher or equal priority, becomes ready.

A thread’s initial priority value can be altered up or down after the thread has been started. Thus it is possible for a preemptible thread to become a cooperative thread, and vice versa, by changing its priority.

## Thread options

The kernel supports a small set of [thread options](https://docs.zephyrproject.org/latest/kernel/services/threads/index.html#thread-options)that allow a thread to receive special treatment under specific circumstances. The set of options associated with a thread are specified when the thread is spawned.
A thread that does not require any thread option has an option value of zero. A thread that requires a thread option specifies it by name, using the | character as a separator if multiple options are needed (i.e. combine options using the bitwise OR operator).

### Thread custom data

Every thread has a 32-bit custom data area, accessible only by the thread itself, and may be used by the application for any purpose it chooses. The default custom data value for a thread is zero.
By default, thread custom data support is disabled. The configuration option `CONFIG_THREAD_CUSTOM_DATA` can be used to enable support.

The `k_thread_custom_data_set()` and `k_thread_custom_data_get()` functions are used to write and read a thread’s custom data, respectively. A thread can only access its own custom data, and not that of another thread.

```c
int call_tracking_routine(void)
{
    uint32_t call_count;

    if (k_is_in_isr()) {
        /* ignore any call made by an ISR */
    } else {
        call_count = (uint32_t)k_thread_custom_data_get();
        call_count++;
        k_thread_custom_data_set((void *)call_count);
    }

    /* do rest of routine's processing */
    ...
}

```

The following code uses the custom data feature to record the number of times each thread calls a specific routine.

## Mutexes

### Defining a mutex

```c
K_MUTEX_DEFINE(my_mutex);
```

### Locking a mutex

```c
k_mutex_lock(&my_mutex, K_FOREVER)
```

### Releasing a mutex

```c
k_mutex_lock(&my_mutex)
```
