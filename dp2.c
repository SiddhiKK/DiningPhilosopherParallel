#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

// shared objects
sem_t chopStick[5];

void *simulatedp(void *s){
	// philosopher ids
	int philosopher_id = *(int *)s;

	while(1){
		printf("%d is now thinking.\n", philosopher_id);
		usleep(5);
		printf("%d is now hungry.\n", philosopher_id);

		// philosopher tries to take first chop stick. if not available, he starts thinking

		if(sem_trywait(&chopStick[philosopher_id + 1]) != EAGAIN){
                // philosopher tries to take second chop stick
                // if available, he starts eating or else drops the first chop stick
			if(sem_trywait(&chopStick[philosopher_id]) != EAGAIN){
				printf("%d is now eating.\n", philosopher_id);
				usleep(5);
				// philosopher drops the chop sticks
				sem_post(&chopStick[philosopher_id + 1]);
				sem_post(&chopStick[philosopher_id]);
			} else {
				sem_post(&chopStick[philosopher_id + 1]);
			}
		}
	}
	pthread_exit(EXIT_SUCCESS);
}
int main(void) {
	int output, i=0, philosopher_id[5];
	pthread_t thread[5];

	while(i < 5)
		sem_init(&chopStick[i++], 0, 1);

	// spawning one thread for each philosopher and assigning philosopher_id to each thread created

	i=0;
	while(i < 5){
		philosopher_id[i] = i;
		output = pthread_create(&thread[i], NULL, simulatedp, (void *)&philosopher_id[i]);
		if(output != 0){
			perror("Thread creation failed, cannot create thread. \n");
			exit(EXIT_FAILURE);
		}
		i  = i+1 ;
	}

	// get n to stop program execution
	system("stty cbreak -echo");
	while (getchar() != 'n') {
		usleep(10000);
	}
	system("stty cooked echo");

	return EXIT_SUCCESS;
}
