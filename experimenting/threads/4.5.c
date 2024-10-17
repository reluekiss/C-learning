#include "header.h"

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

int x = 0;

void *lock_reader(void *arg)
{
	int		 en;
	int		 i;
	int		 newx = -1;
	int		 oldx = -1;
	pthread_mutex_t *pmlp = (pthread_mutex_t *)arg;
	
    if ((en = pthread_mutex_lock(pmlp)) != 0) {
		fprintf(stderr, "lock_reader:pthread_mutex_lock: %s\n",
			strerror(en));
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < 100; i++) {
		newx = READ_ONCE(x);
		if (newx != oldx) {
			printf("lock_reader(): x = %d\n", newx);
		}
		oldx = newx;
		poll(NULL, 0, 1);
	}
	if ((en = pthread_mutex_unlock(pmlp)) != 0) {
		fprintf(stderr, "lock_reader:pthread_mutex_unlock: %s\n",
			strerror(en));
		exit(EXIT_FAILURE);
	}
	return NULL;
}

void *lock_writer(void *arg)
{
	int		 en;
	int		 i;
	pthread_mutex_t *pmlp = (pthread_mutex_t *)arg;

	if ((en = pthread_mutex_lock(pmlp)) != 0) {
		fprintf(stderr, "lock_writer:pthread_mutex_lock: %s\n",
			strerror(en));
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < 3; i++) {
		WRITE_ONCE(x, READ_ONCE(x) + 1);
		poll(NULL, 0, 5);
	}
	if ((en = pthread_mutex_unlock(pmlp)) != 0) {
		fprintf(stderr, "lock_writer:pthread_mutex_unlock: %s\n",
			strerror(en));
		exit(EXIT_FAILURE);
	}
	return NULL;
}
