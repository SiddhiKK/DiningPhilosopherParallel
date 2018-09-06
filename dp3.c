#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdatomic.h>

int n;

// shared objects
sem_t chopStick[5];
atomic_int ate_arr[5];

void *simulatedp(void *s){
	// philosopher ids
	int philosopher_id = *(int *)s;
	int myCount, myRightCount = 0;

	while(1){
		printf("%d is now thinking.\n", philosopher_id);
		usleep(5);
		printf("%d is now hungry.\n", philosopher_id);

		// noting the number of times my neighbor on right side ate
		myRightCount = atomic_load(&ate_arr[(philosopher_id + 1) % 5]);

        // if my right neighbor ate more than me, I will try to take the chop sticks and eat
		if((myCount <= myRightCount)){
			// philosopher tries to take first chop stick. if not available, he starts thinking
			if( sem_trywait(&chopStick[philosopher_id+1]) != EAGAIN){
				// philosopher tries to take second chop stick
                // if available, he starts eating or else drops the first chop stick
				if( sem_trywait(&chopStick[philosopher_id]) != EAGAIN){
					// increment my eating count
					myCount++;
					atomic_store(&ate_arr[philosopher_id], myCount);
					usleep(5);
					printf("%d is eating %d times.\n", philosopher_id, myCount);
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
	int output, i=0, philosopher_id[5], n;
	pthread_t thread[5];

	while(i < 5){
		sem_init(&chopStick[i++], 0, 1);
		atomic_store(&ate_arr[i++], 0);
	}

	// spawning one thread for each philosopher and assigning philosopher_id to each thread created

	i=0;
	while(i < 5){
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
	return EXIT_SUCCESS;
}
