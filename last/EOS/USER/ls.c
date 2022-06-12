#include "ucode.c"

//psuedo code from 8.12.2 EOS

#define S_ISDIR    0040000   // directory
#define S_ISREG    0100000   // regular file
#define S_ISLNK    0120000   // symbolic link
#define S_IFMT    00170000


char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

char buffer[BLKSIZE];


int ls_file(char *name)
{
  STAT fstat, *sp = &fstat;
  stat(name, sp);

  if ((sp->st_mode & S_IFMT) == S_ISREG)
  {
    putc('-');
  }
  if ((sp->st_mode & S_IFMT) == S_ISDIR)
  {
    putc('d');
  }
  if ((sp->st_mode & S_IFMT) == S_ISLNK)
  {
    putc('l');
  }

  for (int i = 8; i >= 0; i--)
  {
    if (sp->st_mode & (1 << i))
    {
      putc(t1[i]);
    }
    else
    {
      putc(t2[i]);
    }
  }

  printf(" %d  %d  %d  %d", sp->st_nlink, sp->st_uid, sp->st_gid, sp->st_size);
  printf(" %s", name);
  if (sp->st_mode == S_ISLNK)
  {
    char slinkname[1024];
    if (readlink(name, slinkname) > 0)
    {
      printf(" -> %s", slinkname);
    }
  }
  putc('\n');
}

int ls_dir(char *dname)
{
    char name[256]; // EXT2 filename: 1-255 chars
    DIR *dp;
    char *cp;
    int dfd = open(dname, O_RDONLY);
    read(dfd,buffer,BLKSIZE);
    cp=buffer;
    dp = (DIR *) buffer;

    while (cp < buffer + BLKSIZE)
    {
        strncpy(name, dp->name, dp->name_len); 
        ls_file(name);

        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
}

int main(int argc, char *argv[])
{
    STAT mstat, *sp;
    int r;
    char *s;
    char cwd[1024];

    if (!argv[1]) //ls in cwd
    {
        getcwd(cwd);
        ls_dir(cwd);
    }
    else
    {
        ls_dir(argv[1]);
    }
}