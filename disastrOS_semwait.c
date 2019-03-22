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

  printf("[PROCESSO %d] CHIAMATO SEMWAIT, fd=%d\n",running->pid,fd);

  // mi prendo descrittore del semaforo su cui devo fare wait
  SemDescriptor* sem_desc= SemDescriptorList_byFd(&running->sem_descriptors,fd);

  // controllo che descrittore esista
  HANDLE_ERROR(!sem_desc,DSOS_SEM_DESC_NOT_FOUND,"Errore! SemDescriptor non valido");

  // puntatore SemDescriptorPtr
  SemDescriptorPtr* sem_desc_ptr=sem_desc->ptr;

  // controllo che SemDescriptor esista
  HANDLE_ERROR(!sem_desc_ptr,DSOS_SEM_DESC_PTR_NOT_FOUND,"Errore! SemDescriptorPtr non valido");

  // prendo semaforo dal descrittore
  Semaphore* sem=sem_desc->semaphore;
  
  // controllo che sem esista
  HANDLE_ERROR(!sem, DSOS_SEM_NOT_FOUND,"Errore! Semaforo non valido");

  //verifico se count <=0
  sem->count=sem->count-1;

  if(sem->count<0){
      // rimuovo descrittori
    List_detach(&sem->descriptors, (ListItem*)sem_desc_ptr);

    // inserisco processo chiamante nella waiting semaphore list
    List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last,(ListItem*)sem_desc_ptr);

    // inserisco processo nella waiting list
    List_insert(&waiting_list, waiting_list.last,(ListItem*)running);

    // cambio stato da running a waiting
    running->status=Waiting;

    // prendo prossimo processo da schedulare
    PCB* p_next= (PCB*)List_detach(&ready_list,ready_list.first);

    // setto running
    running=p_next;
    printf("[PROCESSO %d] RUNNING\n",running->pid);

  }
  running->syscall_retvalue=0;
  
  return;

  
}
