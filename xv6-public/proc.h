// Per-CPU state
struct cpu
{
  uchar apicid;              // Local APIC ID
  struct context *scheduler; // swtch() here to enter scheduler
  struct taskstate ts;       // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS]; // x86 global descriptor table
  volatile uint started;     // Has the CPU started?
  int ncli;                  // Depth of pushcli nesting.
  int intena;                // Were interrupts enabled before pushcli?
  struct proc *proc;         // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

// PAGEBREAK: 17
//  Saved registers for kernel context switches.
//  Don't need to save all the segment registers (%cs, etc),
//  because they are constant across kernel contexts.
//  Don't need to save %eax, %ecx, %edx, because the
//  x86 convention is that the caller has saved them.
//  Contexts are stored at the bottom of the stack they
//  describe; the stack pointer is the address of the context.
//  The layout of the context matches the layout of the stack in swtch.S
//  at the "Switch stacks" comment. Switch doesn't save eip explicitly,
//  but it is on the stack and allocproc() manipulates it.
struct context
{
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate
{
  UNUSED,
  EMBRYO,
  SLEEPING,
  RUNNABLE,
  RUNNING,
  ZOMBIE
};

extern struct proc *rdyq_head;
extern struct proc *rdyq_tail;

// Per-process state
struct proc
{
  uint sz;                    // Size of process memory (bytes)
  pde_t *pgdir;               // Page table
  char *kstack;               // Bottom of kernel stack for this process
  enum procstate state;       // Process state
  int pid;                    // Process ID
  struct proc *parent;        // Parent process
  struct trapframe *tf;       // Trap frame for current syscall
  struct context *context;    // swtch() here to run process
  void *chan;                 // If non-zero, sleeping on chan
  int killed;                 // If non-zero, have been killed
  struct file *ofile[NOFILE]; // Open files
  struct inode *cwd;          // Current directory
  char name[16];              // Process name (debugging)

  int tq;            // Time Quantum (in clock ticks)
  int ticks;         // Number of clock ticks process has ran for
  struct proc *next; // Next process in the queue
  int burst_time;    // Burst time of the process
  int priority;      // Priority of the process
  int wait_time;     // Total time spent waiting
};

#define MAX_PRIORITY 40 // Maximum priority level
#define MIN_PRIORITY 0  // Minimum priority level
#define NPRIORITIES 3   // Number of priority levels

#define SCHED_RR 0
#define SCHED_PRIORITY 1
#define SCHED_MLFQ 2

extern int scheduling_policy;
void setsched(int policy);
struct priority_queue
{
  struct proc *bins[MAX_PRIORITY + 1];
};

#define DEFAULT_QUANTUM 1

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
