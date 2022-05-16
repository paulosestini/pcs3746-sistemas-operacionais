#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1

int main()
{
	char *argv[] = {"/child"};
	char *newargv[] = {NULL, "2", NULL};
	char *newenviron[] = {NULL};

	if (fork() != 0)
	{ /* PARENT */
		while (TRUE)
		{
			printf("1");
		}
	}
	else
	{ /* CHILD */
		execve(argv[0], newargv, newenviron);
		perror("execve"); /* execve() returns only on error */
		exit(EXIT_FAILURE);
	}
	return 0;
}
