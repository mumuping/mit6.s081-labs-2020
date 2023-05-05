#include "types.h"
#include "date.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64 sys_exit(void) {
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return fork(); }

uint64 sys_wait(void) {
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64 sys_sbrk(void) {
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64 sys_sleep(void) {
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (myproc()->killed) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

/**
 * In this assignment you will add a system call tracing feature that may help
 * you when debugging later labs. You'll create a new trace system call that
 * will control tracing. It should take one argument, an integer "mask", whose
 * bits specify which system calls to trace. For example, to trace the fork
 * system call, a program calls trace(1 << SYS_fork), where SYS_fork is a
 * syscall number from kernel/syscall.h. You have to modify the xv6 kernel to
 * print out a line when each system call is about to return, if the system
 * call's number is set in the mask. The line should contain the process id, the
 * name of the system call and the return value; you don't need to print the
 * system call arguments. The trace system call should enable tracing for the
 * process that calls it and any children that it subsequently forks, but should
 * not affect other processes.
 */
uint64 sys_trace(void) {
  int mask;
  argint(0, &mask);
  /* Check the mask */
  if (mask < 0) {
    return -1;
  }
  /* Save the mask */
  myproc()->trace_mask = mask;
  return 0;
}

/**
 * In this assignment you will add a system call, sysinfo, that collects
 * information about the running system. The system call takes one argument: a
 * pointer to a struct sysinfo (see kernel/sysinfo.h). The kernel should fill
 * out the fields of this struct: the freemem field should be set to the number
 * of bytes of free memory, and the nproc field should be set to the number of
 * processes whose state is not UNUSED. We provide a test program sysinfotest;
 * you pass this assignment if it prints "sysinfotest: OK".
 */
uint64 sys_sysinfo(void) {
  struct sysinfo si;
  si.freemem = getfreememorysize();
  si.nproc = getusingprocs();

  uint64 address_of_user_sysinfo;
  if (argaddr(0, &address_of_user_sysinfo) < 0) {
    return -1;
  }
  if (copyout(myproc()->pagetable, address_of_user_sysinfo, (char *)&si,
              sizeof(struct sysinfo)) < 0) {
    return -1;
  }
  return 0;
}