#include "ucode.c"

int main(int argc, char *argv[])
{
    char buffer[BLKSIZE];
    int oldfile, newfile, n;
    if(argc==1) //stdin to stdout
    {
        while(read(0, buffer, BLKSIZE))
        {
            for (int i = 0; i < BLKSIZE; i++)
            {
                if (buffer[i] >= 'a' && buffer[i] <= 'z') 
                {
                    buffer[i] = buffer[i] + ('A' - 'a');
                }
            }
            prints(buffer);
        }
    }

    if (argc == 3) //two filenames 
    {
        close(0);
        close(1);
        oldfile = open(argv[1], O_RDONLY);
        newfile = open(argv[2], O_WRONLY);
        while(n=read(oldfile, buffer, BLKSIZE))
        {
            for (int i = 0; i < BLKSIZE; i++)
            {
                if (buffer[i] >= 'a' && buffer[i] <= 'z') 
                {
                    buffer[i] = buffer[i] + ('A' - 'a');
                }
            }
            //prints(buffer);
            write(newfile,buffer,n);
        }
    }
}
