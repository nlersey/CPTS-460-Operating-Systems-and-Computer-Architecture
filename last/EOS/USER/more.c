#include "ucode.c"

int main(int argc, char *argv[])
{
    int i;
    char c[1];
    if (!argv[1])//no file passed 
    {
        while(i = read(0, c, 1))
        {
            if (c[0] == '\r')
            {
                putc('\n');
                continue;
            } 
            write(1, c, 1);  
        }
    }
    else //file passed in
    {
        int fd = open(argv[1], O_RDONLY);
        char buffer[BLKSIZE];
        int n = read(fd, buffer, BLKSIZE);
        printf("%s", buffer);
    
         while (n = read(fd, buffer, BLKSIZE))
        {
            int j = 0;
            while(read(0, c, 1)) 
            {
                if (c[0] == ' ') //user entered space
                {
                    for (int j = 0; j < n; j++)
                    {
                        putc(buffer[j]);
                        if (buffer[j] == '\r')
                        {
                            break;
                        }
                    }
                    break; 
                }
                if (c[0] == '\n' || c[0] == '\r') //user entered enter
                {
                    while (buffer[j] != '\n' && buffer[j] != '\r')
                    {
                        putc(buffer[j]);
                        if(j >= n)
                        {
                            break;
                        }
                        j++;
                    }
                    putc('\n');
                }
                else //user entered other key
                {
                    putc(buffer[j]);
                }
                if (j >= n)
                    break; 
                j++;               
            }            
        }
        close(fd);
    }
}