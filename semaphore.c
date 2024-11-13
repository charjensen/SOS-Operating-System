///////////////////////////////////////////////////////
// Semaphore implementation
// This implementation provides SEM_MAXNUMBER semaphores
// for use by user processes; the semaphore number is specified 
// using an 8-bit number (called key), which restricts us to 
// have up to 256 semaphore variables
// DO NOT use a semaphore object that has not been created; the functions
// always return in such cases
// TODO: Allow user process to create own semaphore object

//Author: Charles Jensen

#include "kernel_only.h"

SEMAPHORE sem[SEM_MAXNUMBER];	// the semaphore locks; maximum 256 of them 

/*** Initialize all semaphorees ***/
void init_semaphores() {
	int i;
	for (i=0; i<SEM_MAXNUMBER; i++) {
		init_queue(&(sem[i].waitq));
		sem[i].value = 0;
		sem[i].available = TRUE;
	}
	sem[0].available = FALSE;
}

/*** Create a semaphore object ***/
// At least one of the cooperating processes (typically
// the main process) should create the semaphore before use.
// The function returns 0 if no semaphore object is
// available; otherwise the semaphore object number is returned 
// init_value is the start value of the semaphore
sem_t semaphore_create(uint8_t init_value, PCB *p) {
	// TODO: see background material on what this function should do

	//find an available semaphore from 2nd on
	unsigned char i = 1;
	while (sem[i].available != TRUE) {
		
		i++;

		if (i > 256) return 0;

	}

	//update semaphore objects vars
	sem[i].available = FALSE;
	sem[i].creator = p->pid;
	sem[i].value = init_value;
	sem[i].waitq.head = 0;
	sem[i].waitq.count = 0;

	// TODO: comment the following line before you start working

	//return semaphore objects array index
	return i;

}

/*** Destroy a semaphore with a given key ***/
// This should be called by the process who created the semaphore
// using semaphore_create; the function makes the semaphore key available
// for use by other creators
// Semaphore is automatically destroyed if creator process dies; creator
// process should always destroy a semaphore when no other process is
// using it; otherwise the behavior is undefined
void semaphore_destroy(sem_t key, PCB *p) {
	// TODO: see background material on what this function should do

	//if the semaphore was never created then exit
	if (sem[key].available == TRUE) return;

	//if the process created the semaphore let it destroy it
	if (sem[key].creator == p->pid) sem[key].available = TRUE;

}

/*** DOWN operation on a semaphore ***/
// Returns TRUE if process p is able to obtain semaphore
// number <key>; otherwise the process is queued and FALSE is
// returned.
bool semaphore_down(sem_t key, PCB *p) {
	// TODO: see background material on what this function should do

	if (sem[key].available == TRUE) return FALSE;

	//if the semaphore is > 0 then decrement it
	if (sem[key].value > 0) {

		sem[key].value--;
		return TRUE;

	}

	//otherwise block the program and put it in the queue
	int ret = enqueue(&(sem[key].waitq), p);

	p->semaphore.wait_on = key;
	p->semaphore.queue_index = ret;

	// TODO: comment the following line before you start working
	return FALSE;
}

/*** UP operation on a sempahore ***/
void semaphore_up(sem_t key, PCB *p) {
	// TODO: see background material on what this function should do

	if (sem[key].available == TRUE) return;

	//up the value of the semaphore by one
	sem[key].value++;

	//if the queue is not empty then find a process that was blocked to give the resource to
	if (sem[key].waitq.count > 0) {
	
		PCB* process = dequeue(&(sem[key].waitq));
		sem[key].value--;
		process->semaphore.wait_on = -1;
		process->state = READY;
			
	}

}

/*** Cleanup semaphorees for a process ***/
void free_semaphores(PCB *p) {
	int i;

	for (i=1; i<SEM_MAXNUMBER; i++) {
		// see if process is creator of the semaphore
		if (p->pid == sem[i].creator) semaphore_destroy((sem_t)i,p);
	}

	// remove from wait queue, if any
	if (p->semaphore.wait_on != -1) 
		remove_queue_item(&sem[p->semaphore.wait_on].waitq, p->semaphore.queue_index);
	
}



