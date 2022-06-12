#include "ucode.c"
//using psuedocode from 8.15.2 EOS and 13.3.2 MTX

int in, out, err;
char username[128], password[128];
char tokenized_buffer[64][128];

int main(int argc, char *argv[])
{
    char fbuffer[153];
    int count, fp;

    //step 1: close file descriptors 0,1 inherited from INIT
    close(0);
    close(1);

    //step 2: open argv[1] 3 times as in(0), out(1), err(2).

    //rdonly = 0
    //wronly = 1
    //rdwr = 2
    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_RDWR);

    //step 3: set tty name string in PROC.tty
    settty(argv[1]);

    //step 4: open /etc/passwd file for READ.
    fp = open("/etc/passwd", O_RDONLY);


    while (1)
    {
        /*step 5: 
        printf("password:"); gets(password);
        for each line in /etc/passwd file do{
        tokenize user account line;
        }
        */
        printf("login: "); 
        gets(username);
        printf("password: "); 
        gets(password);

        // tokenize input
        read(fp, fbuffer, 153);
        
        //tokenize buffer
        char buffer[128];
        int i, j = 0, k = 0;

        for (i = 0; i < strlen(fbuffer); i++)
        {
            if (fbuffer[i] != ':' && fbuffer[i] != '\n' && fbuffer[i] != EOF)
            {
                buffer[j] = fbuffer[i];
                j++;
            }
            else
            {
                buffer[j] = '\0';
                strcpy(tokenized_buffer[k], buffer);
                j = 0;
                k++;
            }
        }

        for (int i = 0; i < 128; i++)
        {
            //step 6 if (user has a valid account)...
            
            // using format username:password:gid:uid:fullname:HOMEDIR:program
            if (strcmp(password, tokenized_buffer[i+1]) == 0 && strcmp(username, tokenized_buffer[i]) == 0)
            {
                /*step 7:
                change uid, gid to user's uid, gid; // chuid()
                change cwd to user's home DIR // chdir()
                close opened /etc/passwd file // close()
                */
                printf("LOGIN : Welcome %s\n", tokenized_buffer[i+4]);
                printf("LOGIN : cd to HOME=%s   change uid to %s\n", tokenized_buffer[i+5], tokenized_buffer[i+3]);
                printf("LOGIN : exec to /bin/%s\n", tokenized_buffer[i+6]);

                chdir(tokenized_buffer[i+5]);
                chuid(atoi(tokenized_buffer[i+2]), atoi(tokenized_buffer[i+3]));
                close(fp);
                //step 8: exec to program in user account
                exec(tokenized_buffer[i+6]);

            }
        }

        printf("login failed, please try again\n");
    }
}