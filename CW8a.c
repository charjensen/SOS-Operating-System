#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t h;
	
sem_t w;

void *print_hello(void *arg) {
	int i;
	for (i=0; i<10; i++) {
		sem_wait(&h);
		printf("Hello ");
		fflush(stdout);
		sleep(rand()%3);
		sem_post(&w);
	}
}

void *print_world(void *arg) {
	int i;
	
	for (i=0; i<10; i++) {
		sem_wait(&w);
		sleep(rand()%3);
		printf("World\n");
		fflush(stdout);
		sem_post(&h);
	}
}

int main(int argc, char *argv[]) {
	pthread_t thread1, thread2;
	int ret;
	
	sem_init(&h, 0, 1);
	sem_init(&w, 0, 0);

	ret = pthread_create(&thread1, NULL, print_hello, NULL);
	if (ret != 0) {
		printf("Error creating hello thread.\n");
		exit(1);
	}
	
	ret = pthread_create(&thread2, NULL, print_world, NULL);
	if (ret != 0) {
		printf("Error creating world thread.\n");
		exit(1);
	}
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	printf("\n");
	
	return(0);
}

