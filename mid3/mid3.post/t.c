#include "type.h"
#include "string.c"

int kprintf(char *fmt, ...);
#define printf kprintf

#include "queue.c"
#include "kbd.c"
#include "vid.c"

#include "exceptions.c"
#include "wait.c"
#include "kernel.c"
#include "pipe.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
      
    if (vicstatus & (1<<31)){ // SIC interrupts=bit_31=>KBD at bit 3 
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

PIPE *kpipe;

int pipe_writer()
{
  char line[128];

  while(1){
    kprintf("Enter a line for task%d to get : ", running->pid);
    kgets(line);

    if (strcmp(line, "exit")==0){
      kexit(2);
    }

    printf("task%d writing line=[%s] to pipe\n", running->pid, line);
    write_pipe(kpipe, line, strlen(line));
    printf("task%d wrote [%s] to pipe\n", running->pid,line);
  }
}

int pipe_reader()
{
  char line[128];
  int i, n;
  while(1){
    printf("proc%d : enter number of chars to read : ", running->pid);
    n = geti();
    if (n==0){
       kexit(1);
    }
    printf("proc%d reading %d chars from pipe\n", running->pid, n);
    n = read_pipe(kpipe, line, n);
    printf("proc%d read n=%d bytes from pipe : [", running->pid, n);
    for (i=0; i<n; i++)
        kputc(line[i]);
    printf("]\n");
    
  }
}

int sendChild(PROC *parent, PROC *p)  // this function inserts a process at the end of the given child list
{
  PROC *q  = parent->child;
  if(!q)    // if the parent has no children, insert at the first child position
  {
     printf("Apparently p%d has no children so we're gonna give him p%d\n", parent->pid, p->pid);
     parent->child = p;
     p->sibling = 0;
     return 0;
  }

  while(q->sibling)  // find the last child of parent
  {
     q = q->sibling;
  }
  q->sibling = p; // insert p there
  printf("p%d is now the sibling of p%d\n", p->pid, q->pid);
  p->sibling = 0;
  return 0;
}

int main()
{ 
   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kprintf("Welcome to Wanix in ARM\n");
   kbd_init();
   
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
   SIC_ENSET |= (1<<3);      // KBD int=3 on SIC
 
   pipe_init();
   kpipe = create_pipe();   

   init();

   kprintf("P0 kfork tasks: ");
 
   kfork((int)pipe_writer, 1);
   kfork((int)pipe_reader, 1);

   printQ(readyQueue);

   unlock();
   while(1){
     if (readyQueue)
        tswitch();
   }
}
