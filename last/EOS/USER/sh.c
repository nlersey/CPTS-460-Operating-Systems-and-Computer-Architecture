#include "ucode.c"

// using psuedo code from 13.3.3 MTX

#define NULL 0

char *head[128], *tail[128];
int pipeposition, hasPipe, lpd[2], pid;

// finds right pipe
int scan(char *cmd[])
{
    int headcmd = 0, tailcmd = 0;
    pipeposition = 0;

    for (int i = 0; i < strlen(cmd)/4; i++) // /4 because of 4 byte ptrs for argv[]
    {
        if (strcmp(cmd[i], "|") == 0) //parse for pipe symbol
        {
            pipeposition = i;
        }
    }

    if (pipeposition == 0)
    {
        strcpy(head, cmd); //set head to whole
        strcpy(tail, ""); 
        return 0;
    }

    for (int i = 0; i < pipeposition; i++)
    {
        head[i] = cmd[i];
        headcmd++;
    }

    int x = 0;
    for (int j = pipeposition+1; j < strlen(cmd)/4; j++) // /4 because of 4 byte ptrs for argv[]
    {
        tail[x] = cmd[j];
        tailcmd++; x++;
    }

    head[headcmd] = NULL;
    tail[tailcmd] = NULL;

    return 1;
}

int do_command(char *cmd[])
{
    int i = 0;
    char *command[BLKSIZE];

    while (cmd[i])
    {
        if (strcmp(cmd[i], ">") == 0) //output redirect 
        {
            close(1);
            open(cmd[i+1], O_WRONLY|O_CREAT);
            break;
        }
        else if (strcmp(cmd[i], "<") == 0) //input redirect
        {
            close(0);
            open(cmd[i+1], O_RDONLY);
            break;
        }
        else if (strcmp(cmd[i], ">>") == 0) //append output redirect
        {
            close(1);
            open(cmd[i+1], O_WRONLY | O_APPEND);
            break;
        }
        i++;
    }

    cmd[i] = 0;

    i = 1;
    strcpy(command, cmd[0]);
    while (cmd[i])
    {
        strcat(command, " ");
        strcat(command, cmd[i]);
        i++;
    }

    exec(command);
}

int do_pipe(char *cmdline[], int *pd)
{
    if (pd) //if pipe passed in has writer on the pipe pd
    {
        close(pd[0]);//close reader
        dup2(pd[1], 1); //dup writer
    }
    
    hasPipe = scan(cmdline); //pass in whole cmdline to parse & find head/tail

    if (hasPipe)
    {
        pipe(lpd); 

        pid = fork(); //0 is returned in child 

        if (pid) //parent
        {
            close(lpd[1]); //close writer
            dup2(lpd[0], 0); //dup reader
            do_command(tail); //pass tail cmd for exec
        }
        else //child
        {
            do_pipe(head, lpd); //recrusively pass head 
        }
    }
    else
    {
        do_command(cmdline); //no pipe, just run whole cmdline
    }
}

int main(int argc, char *argv[])
{
    int status;
    char userInput[256], temp[256];

    while (1)
    {
        signal(2, 1); // ignore Control-C
        printf("sh %d # ", getpid());

        gets(userInput);

        strcpy(temp, userInput);
        token(temp); //tokenize to check if cmd nonbinary cmd

        //simple cmds
        if (strcmp(temp, "logout") == 0)
        {
            exit(0);
        }
        else if (strcmp(temp, "cd") == 0)
        {
            if (!argv[1]) chdir("/");
            else chdir(argv[1]);
            continue;
        }

        //non sh main cmds
        pid = fork(); 
        if (pid)
        {
            pid = wait(&status);
            continue;
        }
        else
        {
            do_pipe(argv, 0); //pass in argv for cmdline
        }
    }
}