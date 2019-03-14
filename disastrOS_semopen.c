#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  // prendo la syscall
  int syscall_num=running->syscall_num;

  // controlli per verificare che quella num della syscall esista effettivamente in disastrOS
  if(syscall_num<0 || syscall_num> DSOS_MAX_SYSCALLS)
    return DSOS_ESYSCALL_OUT_OF_RANGE;

  // prendo id e il type della syscall 
  int sem_id=running->syscall_args[0];
  int sem_type=running->syscall_args[1];

  

  // mi prendo il Semaphore corrispondente tramite il suo id nella lista dei semafori,
  // creata in disastrOS_globals.h
  Semaphore* sem = SemaphoreList_byId(&semaphores_list, sem_id);

  // se semaphore sem non è stato allocato/aperto lo alloco
  if(!sem){
    sem = Semaphore_alloc(sem_id,sem_type);
    List_insert(&semaphores_list,semaphores_list.last,&sem);
    printf("Semaphore sem_id=%d e sem_type=%d, allocato!\n",sem_id, sem_type);
  }

  // alloco il SemDescriptor riferito a quel semaphore
  SemDescriptor* sem_desc = SemDescriptor_alloc(running->last_sem_fd,sem,running);

  // inserisco nella lista dei descrittori del semaphore del processo
  List_insert(&running->sem_descriptors, running->sem_descriptors.last,sem_desc);

  // incremento il valore di fd
  running->last_sem_fd++;

  // alloco puntatore SemDescriptorPtr che si riferisce all descrittore sopra
  SemDescriptorPtr* sem_desc_ptr= SemDescriptorPtr_alloc(sem_desc);

  // setto puntatore alla entry della resource list del descrittore sem_desc
  sem_desc->ptr=sem_desc_ptr;

  // inserisco nella lista descrittori del semaforo sem
  List_insert(&sem->descriptors,sem->descriptors.last,sem_desc_ptr);

  // setto valore di ritorno della syscall al fd del descrittore e poi esco
  running->syscall_retvalue= sem_desc->fd;
  return;
}

