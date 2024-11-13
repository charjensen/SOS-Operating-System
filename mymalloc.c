//
//  mymalloc.c
//  Name: Charles Jensen
//

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<unistd.h>

#define MAX_ORDER 12
#define PAGE_SIZE 0x1000

typedef unsigned long long uint64_t;
typedef unsigned uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

// function prototypes
void print_ra(void *);
void print_ran(void *);
void print(uint64_t *);
void printAll();
void *grow_heap();
void *my_malloc(uint32_t);
void my_free(void *);
void insert(int n, uint64_t a);
uint64_t deleteFirst(int n);

uint64_t *free_list_start[MAX_ORDER+1]; // the free list pointers for different orders
void *start_brk;			// the heap start address (set in main())

//////////////////////////////
/* Print address relative to heap start (start_brk)*/
void print_ra(void *a) {
	a = (void *)((uint64_t)a-(uint64_t)start_brk);
	printf("0x%llx",(uint64_t)a);
}

//////////////////////////////
/* Print address relative to heap start (start_brk) with newline*/
void print_ran(void *a) {
	print_ra(a);
	printf("\n");
} 

//////////////////////////////
/* Print a free list*/
void print(uint64_t *pointer) {
    if (pointer == NULL) 
        printf("Empty list.\n");
    else {
        print_ra(pointer);
        for (pointer=(uint64_t *)(*pointer); pointer!=NULL; pointer=(uint64_t *)(*pointer)) {
            printf("\t");
            print_ra(pointer);			
        }
        printf("\n");
    }
}    

//////////////////////////////
/* Print all free lists*/
void printAll() {
    int i;
    
    for (i=0; i<MAX_ORDER+1; i++) {
        printf("%d:\t",i);
        print(free_list_start[i]);
    }
}

//////////////////////////////
/* Increment heap top (brk) by one page 
   Output: NULL if failure; otherwise the old heap top 
   Meaning: if the function returns a non-NULL value, then 
         all logical addresses from <return value> to 
         (<return value> + PAGE_SIZE - 1) are now usable
*/
void *grow_heap() {
    return sbrk(PAGE_SIZE); // tell Linux to increment heap top
}

///////////////////////////////////////////////////////////
/* Allocate memory using Buddy algorithm 
   Input: amount of requested memory 
   Output: the beginning address of requested memory, or
         NULL if not able to allocate (heap cannot grow anymore)
*/

void *my_malloc(uint32_t size) {
    if (size == 0) return NULL;
    
    int n = (int)ceil(log2(size+4)); // order; 4 more bytes to store order

    if (n > MAX_ORDER) return NULL;

    // TODO: implement as per requirements
	
	uint64_t ret = NULL;
	
	if (free_list_start[n] != NULL) {
		ret = deleteFirst(n);
		*((uint32_t*) ret) = n;
		return ret + 4;
	}
	
	temp_n = n;
	while ( (free_list_start[temp_n] == NULL) && (temp_n < MAX_ORDER) ) {
		
		temp_n++;
		
		if (temp_n == MAX_ORDER) {
			free_list_start[12] = grow_heap();
			continue;
		}
	}
	
	
	
}


///////////////////////////////////////////////////////////
/* Deallocate memory using Buddy algorithm 
   Input: pointer received from my_malloc 
*/

void my_free(void *ptr) {
    // TODO: implement as per requirements
}

void insert(int n, uint64_t a) {
	
	
	
}

uint64_t deleteFirst(int n) {
	
	if (free_list_start[n] == NULL) return NULL;
		
	uint32_t first_ele_value = *((uint32_t*)free_list_start[n]);
		
	if (first_ele_value == NULL) {
		free_list_start[n] = NULL;
		return first_ele_value;
	}
	else {
		uint32_t t_first = free_list_start[n];
		free_list_start[n] = (uint64_t) first_ele_value;
		return t_first;
	}
	
	return NULL;
	
}

///////////////////////////////////////////////////////////
/* Change this function only for testing. No functionality 
   should reside in this function; the GTA will use his own
   main function. 
*/

int main(int argc, char *argv[])
{   
    int i;
	
    start_brk = sbrk(0); // get heap start address

    // initialize all free lists to NULL
    for (i=0; i<MAX_ORDER+1; i++) free_list_start[i]=NULL;

    void *ptr1 = my_malloc(8); print_ran(ptr1); 
    void *ptr2 = my_malloc(8); print_ran(ptr2); 
    void *ptr3 = my_malloc(8); print_ran(ptr3); 
    void *ptr4 = my_malloc(8); print_ran(ptr4); 
    void *ptr5 = my_malloc(8); print_ran(ptr5); 
    void *ptr6 = my_malloc(8); print_ran(ptr6); 
    void *ptr7 = my_malloc(8); print_ran(ptr7); 
    void *ptr8 = my_malloc(8); print_ran(ptr8); 	

    my_free(ptr3); 
    my_free(ptr5); 
    my_free(ptr2); 
    my_free(ptr1); 
    my_free(ptr4); 

    void *ptr9 = my_malloc(48); print_ran(ptr9); 
    void *ptr10 = my_malloc(8); print_ran(ptr10); 
	
    return 0;
}
