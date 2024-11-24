#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_getburst(void)
{
  return myproc()->burst_time;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// System call to set the time quantum for process
int sys_settq(void)
{
  int time_quantum;
  if (argint(0, &time_quantum) < 0 || time_quantum <= 0)
    return -1;

  settq(time_quantum);

  return 0;
}

int sys_setpriority(void)
{
  int pid, priority;

  if (argint(0, &pid) < 0 || argint(1, &priority) < 0)
    return -1;

  setpriority(pid, priority);
  return -1; // PID not found
}

int sys_getpriority(void)
{
  int pid;
  if (argint(0, &pid) < 0)
    return -1;
  struct proc *p = getproc(pid);
  if (p)
  {
    return p->priority;
  }
  return -1;
}

int sys_setsched(void)
{
  int policy;
  if (argint(0, &policy) < 0)
    return -1;

  setsched(policy);
  return 0;
}
