////////////////////////////////////////////////////////
// A Round-Robin Scheduler with 50% share to console
// 
// Process queue is maintained as a doubly linked list
// TODO: processes should be on different queues based 
//       on their state

#include "kernel_only.h"

extern GDT_DESCRIPTOR gdt[6];	// from startup.S
extern TSS_STRUCTURE TSS;	// from systemcalls.c

PCB console;	// PCB of the console (==kernel)
PCB *current_process; // the currently running process
PCB *processq_next = NULL; // the next user program to run

void init_scheduler() {
	current_process = &console; // the first process is the console
}

/*** Add process to process queue ***/
// Returns pointer to added process
PCB *add_to_processq(PCB *p) {
	disable_interrupts();

	// TODO: add process p to the queue of processes, always
	// maintained as a circular doubly linked list;

	// processq_next always points to the next user process
	// that will get the time quanta;
	// if the process queue is non-empty, p should be added immediately
	if (processq_next == NULL) {
		p -> prev_PCB = p;
		p -> next_PCB = p;
		processq_next = p;
	}else {
		p ->prev_PCB = processq_next;
		p -> next_PCB = processq_next ->next_PCB;
		processq_next -> next_PCB ->prev_PCB = p; 
		processq_next -> next_PCB = p;

	}
	// before processq_next
	// For details, read assignment background material

	enable_interrupts();

	return p;		
}

/*** Remove a TERMINATED process from process queue ***/
// Returns pointer to the next process in process queue
PCB *remove_from_processq(PCB *p) {
	PCB *nextRun = p -> next_PCB;
	if (p == NULL && p ->next_PCB == NULL){
	//Queue is empty so prints nothing
	printf("NOthing\n");
	
	}else if(p ->next_PCB == NULL) {
	//Only on process that's in the queue, so just deallocate the current	
		printf("only one process\n")
	}else{
		p -> prev_PCB -> next_PCB = p ->next_PCB;
		p -> next_PCB -> prev_PCB = p -> prev_pcb;
	}
	// TODO: free the memory used by process p's image
	dealloc_memory(p ->memory_base);
	// TODO: free the memory used by the PCB
	dealloc_memory(p);
	// TODO: return pointer to next process in list
	
	enable_interrupts();
	return nextRun;
}

/*** Schedule a process ***/
// This function is called whenever a scheduling decision is needed,
// such as when the timer interrupts, or the current process is done
void schedule_something() { // no interruption when here
	disable_interrupts();
	// TODO: see assignment background material on what this function should do 

	PCB * p = processq_next;

	// TODO: comment the following when you start working on this function
	PCB next_process = processq_next;
	int epochs = get_epochs();
	//tracker
	bool consoleRun = FALSE;
	//scheduling policy
	int count = 0;
	do {
		if (p->state == NEW) {
			p->state = READY;
			add_to_processq(p);
		}
		else if (p-> state == RUNNING){
			p -> state = WAITING;
			ready = FALSE;
		}else if(p->state == WAITING) {
			int currEpoch = get_epochs();
			if (currEpoch >= p->sleep_end) {
				p->state = READY;
				ready = TRUE;
			}
		}else if (p -> state == TERMINATED) {
			p = remove_from_processq(p);
		}
		p = p->next_PCB	
		if (count >1000) {
			consoleRun = FALSE;
			break;
		}
	} while (p ->state != READY);
	consoleRun = !(consoleRun);
	//timer_interrupt_handler

}
	if (urrent_process = &console) {
		switch_to_kernel_process(&console);
	}else {
		current_process = p;
		p->state = RUNNING;
		ready = TRUE;
		enable_interrupts();
		switch_to_user_process(p);
	}
/*** Switch to kernel process described by the PCB ***/
// We will use the "fastcall" keyword to force GCC to pass 
// the pointer in register ECX;
// process switched to is a kernel process; so no ring change
__attribute__((fastcall)) void switch_to_kernel_process(PCB *p)  {

	// load CPU state from process PCB
	asm volatile ("movl %0, %%edi\n": :"m"(p->cpu.edi));
	asm volatile ("movl %0, %%esi\n": :"m"(p->cpu.esi));
	asm volatile ("movl %0, %%eax\n": :"m"(p->cpu.eax));
	asm volatile ("movl %0, %%ebx\n": :"m"(p->cpu.ebx));
	asm volatile ("movl %0, %%edx\n": :"m"(p->cpu.edx));
	asm volatile ("movl %0, %%ebp\n": :"m"(p->cpu.ebp));
	asm volatile ("movl %0, %%esp\n": :"m"(p->cpu.esp));

	// switching within the same ring; IRET requires the following in stack (see IRET details)
	asm volatile ("pushl %0\n"::"m"(p->cpu.eflags));
	asm volatile ("pushl %0\n": :"m"(p->cpu.cs));
	asm volatile ("pushl %0\n": :"m"(p->cpu.eip));

	// this should be the last one to be copied
	asm volatile ("movl %0, %%ecx\n": :"m"(p->cpu.ecx));

	// issue IRET; see IRET details
	asm volatile("sti\n"); // interrupts cleared in timer/syscall handler
	asm volatile("iretl\n"); // this completes the timer/syscall interrupt
}

/*** Switch to user process described by the PCB ***/
/*** DO NOT UNCOMMENT ***/
/*
__attribute__((fastcall)) void switch_to_user_process(PCB *p) {

	// You implemented this in the previous assignment
	...

}*/


