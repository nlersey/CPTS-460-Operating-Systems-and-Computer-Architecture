#include "ucode.c" //user commands and syscall interface
/*
char *name[16];

void *tokenize(char *line, char *name[])
{
  int i, n; char *cp;

  n = 0; 
  for (i=0; i<16; i++)
      name[i]=0;

  cp = line;
  while (*cp != 0){
       while (*cp == ' ')
              *cp++ = 0;       
       if (*cp != 0)
           name[n++] = cp; 
       while (*cp != ' ' && *cp != 0)
	       cp++; 
       if (*cp != 0)       
	   *cp = 0;        
       else
           break; 
       cp++;
  }
}

*/

int main(int argc, char *argv[])
{
    int pid, status;
    //char cmdline[256], buf[256];
    char cmdline[64];
    char cmd[1];
    while(1){
        ///display executable commands in /bin directory
        ///show commands are (u1,u2,exit [parameters])

        ///prompt for a command line cmd line = "cmd a1 a2... an"
        uprintf("commands=(u1|u2|exit [parameters])\n");
        ugetline(cmdline);
        if (!strcmp(cmdline, "exit")) 
            uexit(0);
        //gets(cmdline);
        //strcpy(cmdline, buf);
        //tokenize(cmdline, name);
        //char cmd[16]=name[0]; ////may not work?
        //if(!strcmp(cmd, "exit"))
            //exit(0); ///may want a continue here

        // set cmd to the first two characters of cmdline
        cmd[0] = cmdline[0];
        cmd[1] = cmdline[1];

        if (!strcmp(cmd, "u1") || !strcmp(cmd, "u2")) {
            // uprintf("Hello from %s\n", cmd);
        } 
        else {
            uprintf("unknown command\n");
            continue;
        }
        //fork a child process to execute the cmd line
        pid = ufork();
        if(pid) //parent sh waits for child to die
            pid = wait(&status);
        else //child exec cmdline
            exec(cmdline); //exec("cmd a1 a2 ... an");
    }
}
