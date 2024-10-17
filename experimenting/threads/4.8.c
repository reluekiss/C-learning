#define THREAD_HEADER
#include "header.h"


pthread_rwlock_t rwl	         = PTHREAD_RWLOCK_INITIALIZER;
unsigned long	 holdtime        = 0;
unsigned long	 thinktime       = 0;
long long	    *readcounts;
int		         nreadersrunning = 0;

#define GOFLAG_INIT 0
#define GOFLAG_RUN

#define GOFLAG_STOP 2
char goflag = GOFLAG_INIT;

void *reader(void *arg)
{
	int en;

	int i;

	long long loopcnt = 0;

	long me = (long)arg;
	__sync_fetch_and_add(&nreadersrunning, 1);
	while (READ_ONCE(goflag) == GOFLAG_INIT) {
		continue;
	}
	while (READ_ONCE(goflag) == GOFLAG_RUN) {
		if ((en = pthread_rwlock_rdlock(&rwl)) != 0) {
			fprintf(stderr, "pthread_rwlock_rdlock: %s\n",
				strerror(en));
			exit(EXIT_FAILURE);
		}
		for (i = 1; i < holdtime; i++) {
			msleep(1);
		}
		if ((en = pthread_rwlock_unlock(&rwl)) != 0) {
			fprintf(stderr, "pthread_rwlock_unlock: %s\n",
				strerror(en));
			exit(EXIT_FAILURE);
		}
		for (i = 1; i < thinktime; i++) {
		    msleep(1);
		}
		loopcnt++;
	}
	readcounts[me] = loopcnt;
	return NULL;
}
