#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 starting_va;
  int check_num;
  uint64 buffer_addr;
  uint64 abits = 0;

  if(argaddr(0, &starting_va) < 0) {
    return -1;
  }
  if(argint(1, &check_num) < 0 || check_num > 32) {
    return -1;
  }
  if(argaddr(2, &buffer_addr) < 0) {
    return -1;
  }

  pagetable_t cur_pt = myproc()->pagetable;
  vmprint(cur_pt);
  for(int i = 0; i < check_num; i++) {
    pte_t *pte = walk(cur_pt, starting_va + i * PGSIZE, 0);
    if((*pte & PTE_A) != 0) {
      printf("hit: %d\n", i);
      abits |= (1 << i);
      *pte &= ~PTE_A;
    }
  }

  printf("abits:%ld\n", abits);
  return copyout(cur_pt, buffer_addr, (char *)&abits, sizeof(uint64));
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
