//
//  mymalloc.c
//  Name: Cole Groen
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
uint64_t* prev(int n, uint64_t* a);
void insert(int n, uint64_t a);
uint64_t deleteFirst(int n);


uint64_t *free_list_start[MAX_ORDER+1]; // the free list pointers for different orders
void *start_brk; // the heap start address (set in main())

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

// helper function to find previous value in the linked list
uint64_t* prev(int n, uint64_t* a) {
    uint64_t* temp = free_list_start[n];

    // traverse the list until the next element is the passed in pointer
    for (temp; temp != NULL; temp = (uint64_t *)(*temp)) {
        if (*temp == (uint64_t)a) {
            return temp;
        }
    }

    return NULL; // if the function hasn't returned at this point then it means it was the first element
}

// inserts a block starting at address a into the free list of order n
void insert(int n, uint64_t a) {
    if (free_list_start[n] == NULL) { // inserting into an empty list
        free_list_start[n] = (uint64_t*)a;
        *free_list_start[n] = 0;
        return;
    } else if ((uint64_t*)a < free_list_start[n]) { // inserting first in the list
        *((uint64_t*)a) = (uint64_t)free_list_start[n];
        free_list_start[n] = (uint64_t*)a;
	    return;
    }

    uint64_t* temp = free_list_start[n];

    while(temp != NULL) {
        if ((uint64_t*)a > temp && a < *temp) { // inserting between two elements
            *((uint64_t*)a) = *temp;
            *temp = a;
	        return;
        } else if ((uint64_t*)(*temp) == NULL) { // inserting at the end of the list
            *((uint64_t*)a) = 0;
            *temp = a;
            return;
        }

        temp = (uint64_t*)(*temp); // move to next element
    }
}

// deletes the first element from the free list of order n
uint64_t deleteFirst(int n) {
    uint64_t deletedVal = (uint64_t)free_list_start[n];
    free_list_start[n] = (uint64_t*)(*free_list_start[n]);

    return deletedVal;
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

    int i;
    int j;

    // TODO: implement as per requirements
    for (i = n; i <= MAX_ORDER; i++) {
        if (i == n && free_list_start[i] != NULL) { // if i == n then no splitting is necessary
            uint64_t* returnVal = (uint64_t*)deleteFirst(n);
            *((uint32_t *)returnVal) = n; // setting first 4 bytes
            uint64_t* testVal = (uint64_t*)((uint8_t*)returnVal + 4);

            return (uint64_t*)((uint8_t*)returnVal + 4); // return after the first 4 bytes
        } else if (free_list_start[i] != NULL) { // otherwise you need to split
            for (j = 0; j < i - n; j++) { // delete first element and split it and add it to lower order list
                uint64_t deleteVal = deleteFirst(i - j);
                insert(i - j - 1, deleteVal);
                insert(i - j - 1, deleteVal + pow(2, i - j - 1));
            }

            uint64_t* returnVal = (uint64_t*)deleteFirst(n);
            *((uint32_t *)returnVal) = n; // setting first 4 bytes
            return (uint64_t*)((uint8_t*)returnVal + 4); // return after the first 4 bytes
        } else if (free_list_start[i] == NULL && i == MAX_ORDER) { // if there are no blocks then grow the heap
            uint64_t* newBlock = grow_heap();
            free_list_start[MAX_ORDER] = newBlock;

            for (j = 0; j < i - n; j++) { // split the block at max order all the way down to the order n list
                uint64_t deleteVal = deleteFirst(i - j);
                insert(i - j - 1, deleteVal);
                insert(i - j - 1, deleteVal + pow(2, i - j - 1));
            }

            uint64_t* returnVal = (uint64_t*)deleteFirst(n);
            *((uint32_t *)returnVal) = n; // setting first 4 bytes
            
            return (uint64_t*)((uint8_t*)returnVal + 4); // return after the first 4 bytes
        }
    }
}


///////////////////////////////////////////////////////////
/* Deallocate memory using Buddy algorithm 
   Input: pointer received from my_malloc 
*/
void my_free(void *ptr) {
    // TODO: implement as per requirements
    uint32_t order = *((uint32_t *)((uint8_t*)ptr - 4)); // calulate order by getting 4 bytes before the malloc address
    uint64_t* f = (uint64_t*)((uint8_t*)ptr - 4); // set f equal to the pointer - 4
    insert(order, (uint64_t)f); // add the block back to the list
    int n = order;
    while (n <= MAX_ORDER) {
        // if the inserted block and the next block are contiguous then merge them and add to the next list
        if (*f == (uint64_t)f + pow(2, n) && ((uint64_t)f % (uint64_t)pow(2, n + 1)) == 0) {
 	        uint64_t* temp = (uint64_t*)(*f);
            insert(n + 1, (uint64_t)f); // insert combined block to list of order n + 1

            // delete the two blocks that were merged
            if (prev(n, f) == NULL) {
                free_list_start[n] = (uint64_t*)(*temp);
            } else {
                *prev(n, f) = *temp;
            }
        // if the inserted block and the previous block are contiguous then merge them and add to the next list
        } else if ((uint64_t)prev(n, f) == (uint64_t)f - pow(2, n) && ((uint64_t)prev(n, f) % (uint64_t)pow(2, n + 1)) == 0) {
	        uint64_t* temp = prev(n, f);
            insert(n + 1, (uint64_t)prev(n, f)); // insert combined block to list of order n + 1

            // delete the two blocks that were merged
            if (prev(n, temp) == NULL) {
                free_list_start[n] = (uint64_t*)(*f);
            } else {
                *prev(n, temp) = *f;
            }

	        f = temp; // set f to be the previous block since it has a lower address
        } else { // otherwise don't merge
            return;
        }
        n++; // increment n to move to the next list to see if blocks need to be combined
    }
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
