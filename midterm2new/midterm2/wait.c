int tswitch();

int ksleep(int event)
{
  int SR = int_off(); //disable IRQ and return CPSR
  running->event = event;
  running->status = SLEEP;
  printf("PROC %d sleeping on EVENT %x\n", running->pid, event);
  enqueue(&sleepList, running);
  printf("sleepList: ", sleepList);
  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{
  int SR = int_off(); //disable IRQ and return CPSR
  
  PROC *temp = 0;
  PROC *p;
  
  while (p = dequeue(&sleepList))
  {
    if (p->status==SLEEP && p->event==event)
    {
      p->status = READY; 
      enqueue(&readyQueue, p);
      printf("Wake up PROC %d Event %d\n", p->pid, event);
    }
    else
    {
      enqueue(&temp,p);
    }
  }
  sleepList=temp;
  int_on(SR); //restore orginal CPSR
}

int kexit(int exitValue)
{
  PROC *P1temp = proc + 1;
  PROC *child = running->child;
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  
  printf("PROC %d exit(%d)\n", running->pid, exitValue);
  printf("Proc %d ZOMBIE child of %d\n", running->pid, running->ppid);

  while (child) {
    child->ppid = 1;
    child->parent = P1temp;

    running->child = child->sibling;
    child->sibling = P1temp->child;
    P1temp->child = child;
    child = running->child;
  }
  kwakeup(running->parent);
  tswitch();
  
  /*
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  kwakeup(running->parent);
  tswitch();
  */

}

int kwait (int *status)
{
  if (!running->child)
  {
    printf("kwait error: No Children.\n");
    return -1;
  }
  
  PROC *parenttemp = running;
  while(1){
    PROC *ptemp = parenttemp->child;
    PROC *previoustemp = parenttemp;

    while(ptemp)
    {
      if (ptemp->status ==ZOMBIE) 
      {
      *status = ptemp->exitCode;
      enqueue(&freeList, ptemp);

      //free zombie
      if(ptemp->parent->child==ptemp)
      {
        ptemp->parent->child=ptemp->sibling;
      }
      else
      {
        PROC *freetemp= ptemp->parent->child;
        while(freetemp->sibling != ptemp)
        {
          freetemp = freetemp->sibling;
        }
        freetemp->sibling = ptemp->sibling;
      }
        ptemp->parent = 0;
        ptemp->ppid = 0;     
        ptemp->child = 0;
        ptemp->sibling = 0;
        ptemp->status = FREE;

        return ptemp->pid;
      }
      previoustemp = ptemp;
      ptemp = ptemp->sibling;
    }
    ksleep(running);
  }
}