////////////////////////////////////////////////////////
// The NAIVE Logical Memory Manager
// 
// Divides memory into 4KB pages and allocates from them

#include "kernel_only.h"

// kernel page directory will be placed at frame 257
PDE *k_page_directory = (PDE *)(0xC0101000); 
// page table entries for the 768th page directory entry will be placed
// at frame 258; 768th entry corresponds to virtual address range
// 3GB to 3GB+4MB-1 (0xC0000000 to 0xC03FFFFF)
PTE *pages_768 = (PTE *)(0xC0102000); 

/*** Initialize logical memory for a process ***/
// Allocates physical memory and sets up page tables;
// we need to allocate memory to hold the program code and
// data, the stack, the page directory, and the required
// page tables
// called by runprogram.c; this function does not load the 
// program from disk to memory (done in scheduler.c)

bool init_logical_memory(PCB *p, uint32_t code_size) {

	// TODO: see background material on what this function should
	// do. High-level objectives are:
	// 1) calculate the number of frames necessary for program code,
	//    user-mode stack and kernel-mode stack
	// 2) allocate frames for above
	// 3) determine beginning physical address of program code,
	//    user-mode stack and kernel-mode stack
	// 4) calculate the number of frames necessary for page directory
	//    and page tables
	// 5) allocate frames for above
	// 6) set up page directory and page tables
	// 7) set mem struct in PCB: start_code, end_code, start_stack,
	//    start_brk, brk, and page_directory
	// Return value: TRUE if everything goes well; FALSE if allocation
	//     of frames failed (you should dealloc any frames that may
	//     have already been allocated before returning)

	// TODO: uncomment following line when you start working in 
	//        this function

	return FALSE;
}

/*** Initialize kernel's page directory and table ***/
void init_kernel_pages(void) {
	uint32_t i;

	// set up kernel page directory (users cannot touch this)
	for (i=0; i<1024; i++) k_page_directory[i] = 0;
	k_page_directory[768] = ((uint32_t)pages_768-KERNEL_BASE) | PDE_PRESENT | PDE_READ_WRITE;

	// map virtual (0xC0000000--0xC03FFFFF) to physical (0--0x3FFFFF)
	for (i=0; i<1024; i++) 
		pages_768[i] = (i*4096) | PTE_PRESENT | PTE_READ_WRITE | PTE_GLOBAL;

	// load page directory
	load_CR3((uint32_t)k_page_directory-KERNEL_BASE);
}

/*** Load CR3 with page directory ***/
void load_CR3(uint32_t pd) {
	asm volatile ("movl %0, %%eax\n": :"m"(pd));
	asm volatile ("movl %eax, %cr3\n");
}

/*** Allocate logical memory for kernel***/
// Allocates pages for kernel and returns logical address of allocated memory
// Note: Kernel uses 0xC0000000 to 0xC0400000 for now
void *alloc_kernel_pages(uint32_t n_pages) { 
	uint32_t p_alloc_base; // physical address of allocated memory
	uint32_t l_alloc_base; // logical address of allocated memory
	int i;

	p_alloc_base = (uint32_t)alloc_frames(n_pages, KERNEL_ALLOC); 
	if (p_alloc_base==NULL) return NULL;

	// Note: page table update is not necessary since first 4MB is already
	// mapped and kernel allocation is always from first 4MB

	// adding KERNEL_BASE converts address to logical when allocation 
	// of kernel is from the first 4MB (see alloc_frames)
	l_alloc_base = p_alloc_base + KERNEL_BASE;

	// fill-zero the memory area
	zero_out_pages((void *)l_alloc_base, n_pages);

	return (void *)l_alloc_base; 
}



/*** Deallocate one page ***/
// Deallocates the page corresponding to virtual address
// <loc>; p is the virtual address of page directory
void dealloc_page(void *loc, PDE *p) {
	uint32_t pd_entry = (uint32_t)loc >> 22; // top 10 bits
	uint32_t pt_entry = ((uint32_t)loc >> 12) & 0x000003FF; // next top 10 bits 
	int i;

	// obtain page table corresponding to page directory entry
	PTE *pt = (PTE *)(p[pd_entry] & 0xFFFFF000);
	pt = (PTE *)((uint32_t)pt + KERNEL_BASE); // converting to virtual address

	// deallocate the frame
	dealloc_frames((void *)(pt[pt_entry] & 0xFFFFF000), 1);

	// if user space address, then mark page table entry as not present
	if ((uint32_t)loc < KERNEL_BASE) 
		pt[pt_entry] = 0; 	
}

/*** Deallocate all pages ***/
// Traverses the page directory and deallocs all allocated
// pages; p is the virtual address of page directory
void dealloc_all_pages(PDE *p) {
	uint32_t pd_entry;
	uint32_t pt_entry;
	uint32_t i;
	uint32_t loc = 0;
	PTE *pt;

	while (loc < 0xC0000000) { // only freeing user area of virtual memory
		pd_entry = loc >> 22; // top 10 bits

		if (p[pd_entry] != 0) { // page directory entry exists
			pt = (PTE *)((p[pd_entry] & 0xFFFFF000) + KERNEL_BASE);
			for (i=0; i<1024; i++) { // walk through page table
				if (pt[i] == 0) continue;
				dealloc_page((void *)(loc + i*4096), p);
			}

			// dealloc page table space and mark page directory entry not present
			dealloc_frames((void *)(p[pd_entry] & 0xFFFFF000), 1);
			p[pd_entry] = 0;
		}
		
		loc += 0x400000; // move ahead 4MB; the next page table
	}
}

/*** Zero out pages ***/
// Ensure that page mappings exist before calling this function
void zero_out_pages(void *base, uint32_t n_pages) {
	int i=0;
	for (i=0; i<1024*n_pages; i++)
		*((uint32_t *)((uint32_t)base + i)) = 0;
}


