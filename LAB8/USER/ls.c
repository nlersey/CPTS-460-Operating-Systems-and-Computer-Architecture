#include "ucode.c"
#define BLKSIZE 1024

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int ls_file(char *name)
{
  STAT fstat, *sp = &fstat;
  stat(name, sp);
  // if (sp->st_mode & (1 << 7))
  //   mputc('d');

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
  //if (S_ISLNK(mip->INODE.i_mode)) got rid of this cause hardcoding the file properties in ucode.c was easier
  if (sp->st_mode == S_ISLNK)
  {
    char linkname[1024];
    if (readlink(name, linkname) > 0)
    {
      printf(" -> %s", linkname);
    }
  }

  putc('\n');
}

/*** Summary ***/
/*
    In this function, all we did is have a loop that runs a block of data
    Then in each section, call ls_file() on that section
    Then move the pointer accordingly
*/
int ls_dir(char *name)
{
  char buf[BLKSIZE], fname[BLKSIZE];
  DIR *dp;
  char *cp;

  int fd = open(name, O_RDONLY);
  read(fd, buf, BLKSIZE);
  dp = (DIR *)buf;
  cp = buf;

  while (cp < buf + BLKSIZE)
  {
    strncpy(fname, dp->name, dp->name_len);
    ls_file(fname);

    cp += dp->rec_len;
    dp = (DIR *)cp;
  }
}

/*** Summary ***/
/*
    In this function, we made it check for 2 conditions
    One where use ls without an argument, then we just ls the cwd
    The other is where the user specifies the directory they want to ls, then we ls that directory
*/
int main(int argc, char *argv[])
{
  STAT mstat, *sp;
  int r;
  char *s;
  char cwd[1024];

  if (!argv[1])
  {
    getcwd(cwd);
    ls_dir(cwd);
  }
  else
  {
    ls_dir(argv[1]);
  }