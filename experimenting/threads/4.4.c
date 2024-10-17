#include "header.h"

int x = 0;

void *mythread(void * arg) {
	x = 1;
	printf("Child process set x=1\n");
	return NULL;
}

int main(int argc, char *argv[])
{
	int	  en;
	pthread_t tid;
	void	 *vp;
	if ((en = pthread_create(&tid, NULL, mythread, NULL)) != 0) {
		fprintf(stderr, "pthread_create: %s\n", strerror(en));
		exit(EXIT_FAILURE);
	}
	/* parent */
	if ((en = pthread_join(tid, &vp)) != 0) {
		fprintf(stderr, "pthread_join: %s\n", strerror(en));
		exit(EXIT_FAILURE);
	}
	printf("Parent process sees x=%d\n", x);
	return EXIT_SUCCESS;
}
