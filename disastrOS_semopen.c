#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){

  // prendo id e il count della syscall 
  int sem_id=running->syscall_args[0];
  int sem_count=running->syscall_args[1];

  // mi prendo il Semaphore corrispondente tramite il suo id nella lista dei semafori,
  // creata in disastrOS_globals.h
  Semaphore* sem = SemaphoreList_byId(&semaphores_list, sem_id);

  // se semaphore sem non è stato allocato/aperto lo alloco
  if(!sem){
    sem = Semaphore_alloc(sem_id,sem_count);
    List_insert(&semaphores_list,semaphores_list.last,(ListItem*)sem);
  }

  // alloco il SemDescriptor riferito a quel semaphore
  SemDescriptor* sem_desc = SemDescriptor_alloc(running->last_sem_fd,sem,running);

  if(!sem_desc){
    running->syscall_retvalue=-1;
    return;
  }
  // inserisco nella lista dei descrittori del semaphore del processo
  List_insert(&running->sem_descriptors, running->sem_descriptors.last,(ListItem*)sem_desc);

  // incremento il valore di fd
  running->last_sem_fd++;

  // alloco puntatore SemDescriptorPtr che si riferisce all descrittore sopra
  SemDescriptorPtr* sem_desc_ptr= SemDescriptorPtr_alloc(sem_desc);

  // setto puntatore alla entry della resource list del descrittore sem_desc
  sem_desc->ptr=sem_desc_ptr;

  // inserisco nella lista descrittori del semaforo sem
  List_insert(&sem->descriptors,sem->descriptors.last,(ListItem*)sem_desc_ptr);

  // setto valore di ritorno della syscall al fd del descrittore e poi esco
  running->syscall_retvalue= sem_desc->fd;
  return;
}

