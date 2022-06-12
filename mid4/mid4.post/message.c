/******** message.c file ************/

// READ Chapter 5.13.4.3; Use its code

#define NMBUF 10
struct semaphore nmbuf, mlock;
extern MBUF mbuf[NMBUF], *mbufList;         // mbufs buffers and mbufList

int menqueue(MBUF **queue, MBUF *p)
{
    MBUF *q = *queue;
    if (q == 0 || p->pri > q->pri)
    {
        *queue = p;
        p->next = q;
        return;
    }
    while (q->next && p->pri <= q->next->pri)
    {
        q = q->next;
    }
    p->next = q->next;
    q->next = p;
}

MBUF *mdequeue(MBUF **queue)
{
        MBUF *p = *queue;
    if (p)
    {
        *queue = p->next;
    }
    return p;
}
    
MBUF mbuf[NMBUF], *mbufList;
		    
int msg_init()
{
    int i;
    MBUF *mp;
    printf("mesg_init()\n");
    mbufList = 0;
    for (i = 0; i < NMBUF; i++)        // initialize mbufList
        menqueue(&mbufList, &mbuf[i]); // all priority=0, so use menqueue()
    nmbuf.value = NMBUF;
    nmbuf.queue = 0; // counting semaphore
    mlock.value = 1;
    mlock.queue = 0; // lock semaphore
}

MBUF *get_mbuf()
{
    P(&nmbuf);
    P(&mlock);
    MBUF *mp = mdequeue(&mbufList);
    V(&mlock);
    return mp;    
}

int put_mbuf(MBUF *mp)
{
    P(&mlock);
    menqueue(&mbufList, mp);
    V(&mlock);
    V(&nmbuf);    
}

int send(char *msg, int pid)
{
    if(pid < 0)
    {
        return -1;
    }
    
    PROC *p = &proc[pid];
    MBUF *mp = get_mbuf();

    //printf("In send(): pid=%d\n", pid);

    mp->pid = running->pid;
    mp->pri = 1;

    strcpy(mp->contents, msg);
    printf("task%d  in send, msg = [%s]\n",running->pid, mp->contents);
    
    P(&p->mlock);      

    menqueue(&p->mqueue, mp);
    
    V(&p->mlock);
    V(&p->nmsg);

    return 0;
}

int recv(char *msg)
{
  //  printf("\n\n**Inside recv\n");
    P(&running->nmsg);
    
   // printf("\n\n**Inside recv2\n");
    P(&running->mlock);
    
    MBUF *mp = mdequeue(&running->mqueue);
    
    //printf("**mp->contents: %s\n", mp->contents);
    
    V(&running->mlock);
    strcpy(msg, mp->contents);
    int sender = mp->pid;
    
    put_mbuf(mp);

    return sender;
}

  int semaphore_init()
{
    int i, j;
    MBUF *p;
    kprintf("semaphore_init()\n");
    for (i = 0; i < NMBUF; i++)
    {
        p = &mbuf[i];
        p->pid = i;
        p->pri = READY;
        //p->contents = "\0";
        p->next = p + 1;
    }
    mbuf[NMBUF - 1].next = 0; // circular proc list

    //freeList = &proc[0];
    //mreadyQueue = 0;
    //sleepList = 0;

    //p = dequeue(&freeList);
    //p->priority = 0;
    //p->ppid = 0;
    //running = p;

    //kprintf("running = %d\n", running->pid);
    //printList("freeList", freeList);
}

  
