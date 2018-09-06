#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdatomic.h>

int n;
// shared objects
sem_t *chopStick;
atomic_int *times_ate;

void *simulatedp(void *s){
	// philosopher ids
	int philosopher_id = *(int *)s;
	int rcnt, count_eat=0;

	while(1){
		printf("%d is now thinking.\n",philosopher_id);
		usleep(5);
		printf("%d is now hungry.\n",philosopher_id);

		// get the number of times the right neightbour has eaten
		rcnt = atomic_load(&times_ate[(philosopher_id + 1) % 5]);

		// get chopsticks if right neighbour has eaten more number of times
		if((rcnt >= count_eat)){
			// philosopher tries to take first chop stick. if not available, he starts thinking
			if( sem_trywait(&chopStick[philosopher_id+1]) != EAGAIN){
				// philosopher tries to take second chop stick
                // if available, he starts eating or else drops the first chop stick
				if( sem_trywait(&chopStick[philosopher_id]) != EAGAIN){
					// increment eating count
					count_eat++;
					atomic_store(&times_ate[philosopher_id],count_eat);
					usleep(5);
					printf("%d is eating %d times.\n", philosopher_id, count_eat);
					// philosopher drops the chop sticks
					sem_post(&chopStick[philosopher_id+1]);
					sem_post(&chopStick[philosopher_id]);
				}else{
					sem_post(&chopStick[philosopher_id+1]);
				}
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}

int main(void) {
	int output, i = 0, n;
	int *philosopher_id;
	pthread_t *thread;

	printf("Enter number of philosophers:\n");
	scanf("%d", &n);
	chopStick = (sem_t *)malloc(n*sizeof(sem_t));
	times_ate = (atomic_int *)malloc(n*sizeof(atomic_int));
	philosopher_id = (int *)malloc(n*sizeof(int));
	thread = (pthread_t *)malloc(n*sizeof(pthread_t));

	// spawning one thread for each philosopher and assigning philosopher_id to each thread created

	i=0;
	while(i < n){
		philosopher_id[i] = i;
		output = pthread_create(&thread[i], NULL, simulatedp, (void *)&philosopher_id[i]);
		if(output != 0){
			perror("Thread creation failed, cannot create thread. \n");
			exit(EXIT_FAILURE);
		}
		i++;
	}

	// get n to stop program execution
	system("stty cbreak -echo");
	while (getchar() != 'n') {
		usleep(10000);
	}
	system("stty cooked echo");

	free(thread);
	free(philosopher_id);
	free(times_ate);
	free(chopStick);
	return(EXIT_SUCCESS);
}
