// Save this file as user/test_ready_queue.c

#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_CHILDREN 5

void cpu_bound_task()
{
    int i, j;
    volatile int dummy = 0;
    for (i = 0; i < 100000; i++)
    {
        for (j = 0; j < 1000; j++)
        {
            dummy += i + j;
        }
    }
    printf(1, "CPU-bound task %d completed\n", getpid());
    exit();
}

void io_bound_task()
{
    int i;
    for (i = 0; i < 10; i++)
    {
        printf(1, "IO-bound task %d: iteration %d\n", getpid(), i);
        sleep(50); // Sleep for 50 ticks
    }
    printf(1, "IO-bound task %d completed\n", getpid());
    exit();
}

int main(int argc, char *argv[])
{
    int pid, i;

    printf(1, "Starting ready queue test\n");

    // Create CPU-bound child processes
    for (i = 0; i < NUM_CHILDREN; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            cpu_bound_task();
        }
        else if (pid < 0)
        {
            printf(1, "Fork failed\n");
        }
    }

    // Create IO-bound child processes
    for (i = 0; i < NUM_CHILDREN; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            io_bound_task();
        }
        else if (pid < 0)
        {
            printf(1, "Fork failed\n");
        }
    }

    // Parent process waits for all children to exit
    for (i = 0; i < 3 * NUM_CHILDREN; i++)
    {
        wait();
    }

    printf(1, "Ready queue test completed\n");
    exit();
}
