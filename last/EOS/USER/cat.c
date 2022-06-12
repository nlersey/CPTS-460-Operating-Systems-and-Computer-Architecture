#include "ucode.c"

int main(int argc, char* argv[])
{
  int n, fd;
  char buffer[BLKSIZE];
  STAT st0, st1, st; //inode stats used for pipes
  char tty[64]; //terminals
  int pipeflag; //flag used for pipe checks

  gettty(tty); //get terminal
  stat(tty, &st); //get file stored in tty to store in st
  fstat(0, &st0); //get file stored in file descriptor 0 and store it in st0
  //used for piping, contents of command to the right of pipe will be stored in 0 which represents stdin

  if (st0.st_ino == st.st_ino)//if both stats have the same inode
  { 
    pipeflag = 0; //pipe situation
  }
  else
  {
    pipeflag = 1; //NOT pipe situation
  }
  if (!argv[1]) //no 2nd argumenet from stdin
  {
    strcpy(buffer, ""); 
    char c[1];
    while (read(0, c, 1)) //while stdin input is open
    {
      if (c[0]=='\n'||c[0]=='\r')
      {
        if (!pipeflag)
        {
          prints("\n\r");
        }
        printf("%s", buffer);
        putc('\n');
        strcpy(buffer, "");
        continue;
      }

      if (!pipeflag) //pipe situation
      {
        write(1,c,1); //write out what's read from pipe
      }
      if (n>=BLKSIZE)
      {
        printf("%s", buffer);
        strcpy(buffer, "");
        n=0;
        continue;
      }
      strcat(buffer,c);
      n++;
    }
  }
  else//2nd arguement read
  {
    fd = open(argv[1], O_RDONLY); //open file for read

    while ((n = read(fd, buffer, BLKSIZE)) > 0)
    {
      for (int i = 0; i < n; i++)
      {
        if (buffer[i] == '\n')
        {
          if (pipeflag)
          {
            putc('\n');
          }
          else
          {
            printf("\n\r");
          }
        }
        else
        {
          putc(buffer[i]);
        }
        if (i>=n)
        {
          break;
        }
      }
    }
    close(fd);
  }
}