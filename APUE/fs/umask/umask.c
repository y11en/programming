#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define RWRWRW	(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main(void)
{
	umask(0);
	
	if (creat("foo", RWRWRW) < 0)
	{
		perror("creat foo error");
		
		exit(1);
	}
	
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	if (creat("bar", RWRWRW) < 0)
	{
		perror("creat foo error");
		
		exit(1);
	}

	return 0;
}
