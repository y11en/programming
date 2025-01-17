#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define 	FTW_F		1
#define 	FTW_D		2
#define 	FTW_DNR		3
#define 	FTW_NS		4
static char *fullpath;

char *path_alloc(int *sizep);
typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc  myfunc;
static int myftw(char *, Myfunc*);
static int dopath(Myfunc *);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[])
{
	int ret;

	if (argc != 2)
	{
		fprintf(stderr, "usage: ftw starting-pathname\n");
		exit(1);
	}

	ret = myftw(argv[1], myfunc);
	
	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	// avoid divided by 0
	if (ntot == 0)
		ntot = 1;	

	// DON"T use tab, or it'll be messy
	fprintf(stdout, "regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	fprintf(stdout, "dirtories     = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
	fprintf(stdout, "block  special= %7ld, %5.2f %%\n", nblk, nblk *100.0/ntot);
	fprintf(stdout, "char   special= %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
	fprintf(stdout, "fifo    files = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
	fprintf(stdout, "symbolic links= %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
	fprintf(stdout, "sockets       = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);
	fprintf(stdout, "regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	
	return 0;
}

static int myftw(char *pathname, Myfunc *func)
{
	int len;
	
	fullpath = path_alloc(&len);

	strncpy(fullpath, pathname, len);
	fullpath[len-1] = 0;
	
	return (dopath(func));
}                  

static int dopath(Myfunc* func)
{
	struct stat 	statbuf;
	struct dirent 	*dirp;
	DIR 			*dp;
	int 			ret;
	char 			*ptr;

	if (lstat(fullpath, &statbuf) < 0)
		return (func(fullpath, &statbuf, FTW_NS));

	// not dir
	if (S_ISDIR(statbuf.st_mode) == 0)
		return (func(fullpath, &statbuf, FTW_F));

	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return ret;

	ptr = fullpath + strlen(fullpath);
	*ptr++ = '/';
	*ptr = 0;

	// can't read a dir
	if ((dp = opendir(fullpath)) == NULL)
		return (func(fullpath, &statbuf, FTW_DNR));

	while ((dirp = readdir(dp)) != NULL)
	{
		if ((strcmp(dirp->d_name, ".") == 0) || (strcmp(dirp->d_name, "..") == 0)) 
			continue;

		strcpy(ptr, dirp->d_name);
	
		if ((ret = dopath(func)) != 0)
			break;
	}
	
	ptr[-1] = 0;

	if (closedir(dp) < 0)
	{
		fprintf(stderr, "can't close dir %s\n", fullpath);
		exit(2);
	}

	return ret;
}

static int myfunc(const char *pathname, const struct stat *statbuf, int type)
{
	switch (type)
	{
		case FTW_F:
				switch (statbuf->st_mode & S_IFMT)		// simplicity
				{
					case S_IFREG:	nreg++;		break;
					case S_IFBLK:	nblk++;		break;
					case S_IFCHR:	nchr++;		break;
					case S_IFIFO:	nfifo++;	break;
					case S_IFLNK:	nslink++;	break;
					case S_IFSOCK:	nsock++;	break;
					case S_IFDIR:	
									fprintf(stderr, "%s: should have type FTW_D\n", pathname);
									exit(3);
				}
		case FTW_D:		ndir++;		break;
		case FTW_DNR:	fprintf(stderr, "can't read dir %s\n", pathname);	break;
		case FTW_NS:	fprintf(stderr, "stat error for %s\n", pathname);	break;
		default:		fprintf(stderr, "unknown file type %d for %s\n", type, pathname);
	}

	return 0;
}

#ifdef PATH_MAX
	static int pathmax = PATH_MAX;
#else
	static int pathmax = 0;
#endif

#define SUSV3	200112L

static long posix_version = 0;

#define PATH_MAX_GUESS	1024	

char *path_alloc(int *sizep)
{
	char *ptr;
	int size;

	if (posix_version == 0)
		posix_version = sysconf(_SC_VERSION);

	if (pathmax == 0)
	{
		errno = 0; 
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0)
		{
			if (errno == 0)
				pathmax = PATH_MAX_GUESS;
			else
			{
				fprintf(stderr, "pathconf error for _PC_PATH_MAX\n");
				exit(4);
			}
		}
		else
			pathmax++;
	}

	if (posix_version < SUSV3)
		size = pathmax + 1;
	else
		size = pathmax;

	if ((ptr = malloc(size)) == NULL)
	{
		fprintf(stderr, "malloc error\n");
		exit(5);
	}

	if (sizep != NULL)
		*sizep = size;

	return ptr;
}

