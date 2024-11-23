# Simple Operating System (SOS) 
## Overview SOS is a Simple Operating System designed for the 32-bit x86 architecture. Its purpose is to help understand basic concepts of operating system design. 
## Files 
### `cw1.txt` This file contains assembly instructions and commands for setting up the operating system environment, including: - CPU and memory settings for QEMU. - Assembly instructions for handling CPU registers and flags. - Commands for compiling and running C programs. ### `Cw8a.c` This file contains a multithreading example using POSIX threads and semaphores: - `print_hello`: Prints "Hello" with synchronization using semaphores. - `print_world`: Prints "World" with synchronization using semaphores. 
### `cw8b.c` This file contains another multithreading example using POSIX threads and semaphores: - `reader`: Simulates a producer that fills a buffer with letters. - `writer`: Simulates a consumer that reads from the buffer and prints the contents.
### And many more files, including interrupt handling, memory allocation, running user programs, semaphors and mutexes for interprocess communication, etc.
