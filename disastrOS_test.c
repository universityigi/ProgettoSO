#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"

int sem1,sem2,sem3,sem4;
// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("\n");
  printf("-----------------------------------------------\n");
  printf("---------------OPEN SEMAPHORES-----------------\n");
  printf("-----------------------------------------------\n");
  printf("\n");
  sem1=disastrOS_semopen(1,4);
  sem2=disastrOS_semopen(2,0);
  sem3=disastrOS_semopen(3,-1);
  sem4=disastrOS_semopen(4,1);
 
  disastrOS_printStatus();
  
  for (int i=0; i<4; ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
    printf("%d\n", disastrOS_getpid());
    
    if(disastrOS_getpid()%2==0){
      
          printf("[PROCESSO %d È UN PRODUTTORE]\n",disastrOS_getpid());
          disastrOS_semwait(sem1);
          disastrOS_semwait(sem4);
          printf("[PROCESSO %d] è in sezione critica\n",disastrOS_getpid() );
          disastrOS_sempost(sem4);
          disastrOS_sempost(sem2);
          
    }
    else{
      
          printf("[PROCESSO %d È UN CONSUMATORE]\n",disastrOS_getpid());
          disastrOS_semwait(sem2);
          disastrOS_semwait(sem4);
          printf("[PROCESSO %d] è in sezione critica\n",disastrOS_getpid() );
          disastrOS_sempost(sem4);
          disastrOS_sempost(sem1);
        
    }
  }
  printf("PID=%d IS TERMINATING\n", disastrOS_getpid());
  
  printf("\n");
  printf("-----------------------------------------------\n");
  printf("---------------CLOSE SEMAPHORES----------------\n");
  printf("-----------------------------------------------\n");
  printf("\n");

  disastrOS_semclose(sem1);
  disastrOS_semclose(sem2);
  disastrOS_semclose(sem3);
  disastrOS_semclose(sem4);

  disastrOS_exit(disastrOS_getpid()+1);
   disastrOS_printStatus();
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  

  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;  
  }
  

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  printf("shutdown!");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
