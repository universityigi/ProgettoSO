#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  // fd della syscall
  int fd=running->syscall_args[0];

  // mi prendo descrittore del semaforo su cui devo fare wait
  SemDescriptor* sem_desc= SemDescriptorList_byFd(&running->sem_descriptors,fd);

  // puntatore SemDescriptorPtr
  SemDescriptorPtr* sem_desc_ptr=sem_desc->ptr;

  // prendo semaforo dal descrittore
  Semaphore* sem=sem_desc->semaphore;
  //verifico se count <=0
  sem->count=sem->count-1;
  printf("COUNT value=%d id = %d\n", sem->count,sem->id);
  if(sem->count<0){
    printf("semaphore waiting id=%d e count=%d\n", sem->id, sem->count);
    // rimuovo descrittori
    List_detach(&sem->descriptors, sem_desc);

    // inserisco processo chiamante nella waiting semaphore list
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last,sem_desc_ptr);

    // inserisco processo nella waiting list
    List_insert(&waiting_list, waiting_list.last,running);

    // cambio stato da running a waiting
    running->status=Waiting;

    // prendo prossimo processo da schedulare
    PCB* p_next= List_detach(&ready_list,ready_list.first);

    // setto running
    running=p_next;
  }
  running->syscall_retvalue=0;
  
  return;

  
}
