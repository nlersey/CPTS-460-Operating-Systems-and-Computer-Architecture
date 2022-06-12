#include "ucode.c"
void main ()
{
    int sh, pid, status;
    sh = fork();
    if(sh) // P1 run in a while(1) loop
    {
        while(1)
        {
            pid = wait(&status); //wait for ANY child to die
            if (pid == sh) //if sh died, fork another one
            {
                sh = fork();
                if (!sh) ///may not need?
                    exec("sh start");
            }
            uprintf("P1: I just buried an orphan %d\n",pid);
        }
    }
    else
        exec("exec sh"); //child of p1 runs sh
}
