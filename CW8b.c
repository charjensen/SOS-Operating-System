#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#define BUFFER_SIZE 5
char buffer[BUFFER_SIZE];
int next_write_position = 0;
int next_read_position = 0;
int count = 0;

pthread_mutex_t readlock;

sem_t w;

void *reader(void *arg) {
	int i=0;
	
	for (i=0; i<26; i++) {
		while(count == BUFFER_SIZE) { sleep(1); }
		buffer[next_write_position] = letters[i];
		next_write_position = (next_write_position + 1) % BUFFER_SIZE;
		sem_post(&w);
		count++;

	}
}

void *writer(void *arg) {
	char c;
	
	while(1) {

	sem_wait(&w);
		while(count <= 0) {
			if (next_read_position == -1) return(NULL);
			else sleep(1);
		}
		
		if (next_read_position == -1) {
			return(NULL);
		}
		
		pthread_mutex_lock(&readlock);
		c = buffer[next_read_position];
		printf("%c",c);
		fflush(stdout);
		next_read_position = (next_read_position + 1) % BUFFER_SIZE;
		
		if (c == 'Z') {
			next_read_position = -1;
			sem_post(&w);
			return(NULL);
		}
		
		count--;
		pthread_mutex_unlock(&readlock);
		sleep(rand()%3);
	}
}

int main(int argc, char *argv[]) {
	pthread_t thread1, thread2, thread3;
	int ret;
	pthread_mutex_init(&readlock, NULL);
	sem_init(&w, 0, 0);
	
	ret = pthread_create(&thread1, NULL, reader, NULL);
	if (ret != 0) {
		printf("Error creating reader thread.\n");
		exit(1);
	}
	
	ret = pthread_create(&thread2, NULL, writer, NULL);
	if (ret != 0) {
		printf("Error creating writer1 thread.\n");
		exit(1);
	}
	
	ret = pthread_create(&thread3, NULL, writer, NULL);
	if (ret != 0) {
		printf("Error creating writer2 thread.\n");
		exit(1);
	}
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	
	printf("\n");
	
	return(0);
}

