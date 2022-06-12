#include "ucode.c"
//psuedo code from 8.15.1 EOS and 13.3.1 MTX

int console, s0,s1;
//tty0 for qemu window
//ttyS0 for terminal that launches qemu
//ttyS1 additonal serial connection to connect with screen
int parent()
{
    int pid, status;
    while(1)
    {
        printf("INIT: wait for Zombie child\n");
        pid = wait(&status);
        if (pid == console)
        {
            printf("INIT : fork a new console login\n");
            console = fork();
            if (console) 
            {   
                continue;
            }
            else 
            {
                exec("login /dev/tty0");
            }
        }

        if (pid == s0)
        {
            printf("INIT : fork a new console login\n");
            s0 = fork();
            if (s0)
            {   
                continue;
            }
            else 
            {
                exec("login /dev/ttyS0");
            }
        }

        if (pid == s1)
        {
            printf("INIT : fork a new console login\n");
            s1 = fork();
            if (s1)
            {   
                continue;
            }
            else 
            {
                exec("login /dev/ttyS1");
            }
        }

        printf("INIT : I just buried an orphan child proc %d\n", pid);
    }
}

main()
{
    int in, out; //file descriptors for terminal I/O
    in = open("/dev/tty0", O_RDONLY); // file descriptor 0
    out = open("/dev/tty0", O_WRONLY); //for display to console
    printf("INIT: fork a login proc on console\n");
    console = fork();
    if(console) //parent
    {
        s0 = fork();
        if (s0) 
        {
            s1 = fork(); //create child process 
            if (s1) 
            {
                parent();
            }
            else
            {
                exec("login /dev/ttyS1");
            }
        }
        else
        {
            exec("login /dev/ttyS0");
        }
    }
    else //child: exec to login on tty0
    {
        exec("login /dev/tty0");
    }

}
