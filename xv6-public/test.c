#include "fcntl.h"
#include "types.h"
#include "user.h"
#include "stat.h"

#define MAX_PROCESSES 10

// Simulate workload
void workload(int n, int io_bound)
{
    int i;
    int x = 0;
    for (i = 0; i < n * 1000000; i++)
    {
        x = x + i;
        if (io_bound && i % 1000000 == 0)
        {
            sleep(1);
        }
    }
}

// Test Round Robin with Variable Time Quantum
void test_round_robin()
{
    int pid, i;
    int num_processes = 7;
    int time_quantums[] = {5, 10, 15, 20, 25, 30, 35};
    int start_time, end_time;

    printf(1, "Testing Round Robin with Variable Time Quantum\n");

    for (i = 0; i < num_processes; i++)
    {
        start_time = uptime();
        pid = fork();
        if (pid == 0)
        {
            // Child process
            settq(time_quantums[i]);

            workload(100, 0);
            end_time = uptime();

            int tt = end_time - start_time;

            // Print results
            printf(1, "Process ID: %d | Time Quantum: %d | Turnaround Time: %d ticks\n",
                   getpid(),
                   time_quantums[i],
                   tt);
            exit();
        }
    }

    for (i = 0; i < num_processes; i++)
    {
        wait();
    }
}

// Test Priority Scheduling
void test_priority_scheduling()
{
    int pid, i;
    int num_processes = 7;
    int priorities[] = {5, 10, 15, 20, 25, 30, 35};
    int start_time, end_time, child_start_time;

    printf(1, "Testing Priority Scheduling\n");

    for (i = 0; i < num_processes; i++)
    {
        start_time = uptime();
        pid = fork();
        if (pid == 0)
        {
            // Child process
            setpriority(getpid(), priorities[i]);
            child_start_time = uptime();

            int is_io_bound = i % 2;

            workload(50, is_io_bound);
            end_time = uptime();

            int turnaround_time = end_time - start_time;
            int response_time = child_start_time - start_time;

            printf(1, "Process ID: %d | Priority: %d | Type: %s-bound | Turnaround Time: %d ticks | Response Time: %d ticks\n",
                   getpid(),
                   priorities[i],
                   is_io_bound ? "I/O" : "CPU",
                   turnaround_time,
                   response_time);
            exit();
        }
    }

    for (i = 0; i < num_processes; i++)
    {
        wait();
    }
}

// Test Multilevel Feedback Queue
void test_mlfq()
{
    int pid, i;
    int num_processes = MAX_PROCESSES;
    int start_time[MAX_PROCESSES], end_time[MAX_PROCESSES];
    int turnaround_time, response_time;
    int fd[MAX_PROCESSES][2];

    printf(1, "Testing Multilevel Feedback Queue Scheduler with Response Time Calculation\n");
    for (i = 0; i < num_processes; i++)
    {
        if (pipe(fd[i]) < 0)
        {
            printf(1, "Pipe creation failed\n");
            exit();
        }

        start_time[i] = uptime();
        pid = fork();
        if (pid == 0)
        {
            // Child process
            close(fd[i][0]);

            int is_io_bound = i % 2;

            int child_start_time = uptime();
            write(fd[i][1], &child_start_time, sizeof(child_start_time));
            close(fd[i][1]);

            workload(50, is_io_bound);

            end_time[i] = uptime();
            turnaround_time = end_time[i] - start_time[i];
            response_time = child_start_time - start_time[i];

            printf(1, "Process ID: %d | Type: %s-bound | Turnaround Time: %d ticks | Response Time: %d ticks\n",
                   getpid(),
                   is_io_bound ? "I/O" : "CPU",
                   turnaround_time,
                   response_time);
            exit();
        }
        else if (pid < 0)
        {
            printf(1, "Fork failed\n");
            exit();
        }
        else
        {
            int child_start_time;
            if (read(fd[i][0], &child_start_time, sizeof(child_start_time)) != sizeof(child_start_time))
            {
                printf(1, "Failed to read from pipe for process %d\n", i);
            }
            close(fd[i][0]);
        }
    }

    for (i = 0; i < num_processes; i++)
    {
        close(fd[i][1]);
    }

    for (i = 0; i < num_processes; i++)
    {
        wait();
    }

    printf(1, "MLFQ Test Completed.\n");
}

int main()
{
    printf(1, "Switching to Round Robin Scheduling...\n");
    setsched(SCHED_RR);
    test_round_robin();

    printf(1, "Switching to Priority Scheduling...\n");
    setsched(SCHED_PRI);
    test_priority_scheduling();

    printf(1, "Switching to Multilevel Feedback Queue Scheduling...\n");
    setsched(SCHED_MLFQ);
    test_mlfq();

    exit();
}
