#include "ucode.c"

int main(int argc, char *argv[])
{
    int n, fd;
    char buffer[BLKSIZE], c[1];
    STAT st0, st1, st; //inode stats used for pipes
    char tty[64]; //terminal
    int pipeflag; //flag used for pipe check

    gettty(tty); //get terminal
    stat(tty, &st); //get file stored in tty to store in st
    fstat(0, &st0);//get file stored in file descriptor 0 and store it in st0

    if (st0.st_ino == st.st_ino)//if both stats have same inode
        pipeflag = 0; //pipe situation
    else
        pipeflag = 1; //NOT pipe situation

    if (!argv[2]) //no filename
    {
        if (!pipeflag) //pipe situation
        {
            while (read(0, c, 1))
            {
                if (c[0]=='\r')
                {
                    printf("\n%s\n", buffer);
                }

                write(1,c,1);
                if (n>=BLKSIZE)
                {
                    printf("\n%s", buffer);
                }
                strcat(buffer,c);
                n++;
            }
        }
        else
        {
            char line[BLKSIZE];
            int x=0;

            while (n = read(fd, buffer, BLKSIZE))
            {
                for (int i = 0; i < n; i++)
                {
                    if (buffer[i] == '\n' || buffer[i] == '\r')
                    {
                        char *val = strstr(line, argv[1]);

                        if (val != 0)
                        {
                            printf("%s", val);
                            putc('\n');
                        }

                        memset(line, 0, BLKSIZE);
                        x=0;
                    }  
                    line[x] = buffer[i]; x++;
                }
            }
        }
    }
    else //filename
    {
        fd = open(argv[2], O_RDONLY);

        char line[BLKSIZE];
        int x=0;

        while (n = read(fd, buffer, BLKSIZE))
        {
            for (int i = 0; i < n; i++)
            {
                if (buffer[i] == '\n' || buffer[i] == '\r')
                {
                    char *val = strstr(line, argv[1]);

                    if (val != 0)
                    {
                        printf("%s", val);
                        putc('\n');
                    }

                    memset(line, 0, BLKSIZE);
                    x=0;
                }  
                line[x] = buffer[i]; x++;
            }
        }
        close(fd);
    }
}