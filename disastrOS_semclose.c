#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){

  // prendo il filedescriptor della syscall
  int sem_fd=running->syscall_args[0];

  // prendo il descrittore dalla lista dei descritttori
  SemDescriptor* sem_desc=SemDescriptorList_byFd(&running->sem_descriptors,sem_fd);

  if(!sem_desc){
    running->syscall_retvalue=-1;
    return;
  }
  // rimuovo descrittore
  List_detach(&running->sem_descriptors,(ListItem*)sem_desc);

  // prendo il semaforo dal descrittore sem_desc
  Semaphore* sem=sem_desc->semaphore;
  // prendo puntatore alla entry nella resource list di quel descrittore sem_desc
  SemDescriptorPtr* sem_desc_ptr =(SemDescriptorPtr*) List_detach(&sem->descriptors, (ListItem*)sem_desc->ptr);

  // controllo che se non ci sono descrittore nel semaforo sem, sia nella lista
  // dei descrittori sia nella lista , rimuovo il semaphore dalla lista dei semafori
  // dealloco
  if(sem->descriptors.size==0 && sem->waiting_descriptors.size==0){
    List_detach(&semaphores_list,(ListItem*)sem);
    Semaphore_free(sem);
  }

  //dealloco risorse e setto valore di ritorno della syscall
  SemDescriptor_free(sem_desc);
  SemDescriptorPtr_free(sem_desc_ptr);
  running->syscall_retvalue=0;
  return;

}
