#pragma once
#include "disastrOS_pcb.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


// funzione per gestire errori
#define HANDLE_ERROR(cond, err, msg) do {                           \
        if (cond) {                                                 \
            fprintf(stderr, "%s: %d\n", msg, err);        \
            running->syscall_retvalue=err;                          \
            return;                                                 \
        }                                                           \
    } while(0); 


#ifdef _DISASTROS_DEBUG_
#include <stdio.h>

#define disastrOS_debug(...) printf(__VA_ARGS__) 


#else //_DISASTROS_DEBUG_

#define disastrOS_debug(...) ;

#endif //_DISASTROS_DEBUG_

// initializes the structures and spawns a fake init process
void disastrOS_start(void (*f)(void*), void* args, char* logfile);

// generic syscall 
int disastrOS_syscall(int syscall_num, ...);

// classical process control
int disastrOS_getpid(); // this should be a syscall, but we have no memory separation, so we return just the running pid
int disastrOS_fork();
void disastrOS_exit(int exit_value);
int disastrOS_wait(int pid, int* retval);
void disastrOS_preempt();
void disastrOS_spawn(void (*f)(void*), void* args );
void disastrOS_shutdown();

// semopen
int disastrOS_semopen(int resource_id, int type);
// semclose
int disastrOS_semclose(int fd);
// semwait
int disastrOS_semwait(int fd);
// sempost
int disastrOS_sempost(int fd);

// timers
void disastrOS_sleep(int);

// respurces (files)
int disastrOS_openResource(int resource_id, int type, int mode);
int disastrOS_closeResource(int fd) ;
int disastrOS_destroyResource(int resource_id);

// debug function, prints the state of the internal system
void disastrOS_printStatus();

