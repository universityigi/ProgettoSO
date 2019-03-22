#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_constants.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){

  // prendo il filedescriptor della syscall
  int fd=running->syscall_args[0];

  // prendo il descrittore dalla lista dei descritttori
  SemDescriptor* sem_desc=SemDescriptorList_byFd(&running->sem_descriptors,fd);
  
  // controllo che il descrittore esista
  HANDLE_ERROR(!sem_desc,DSOS_SEM_DESC_NOT_FOUND,"Errore! SemDescriptor non valido");

  // rimuovo descrittore dal PCB
  List_detach(&running->sem_descriptors,(ListItem*)sem_desc);

  // prendo il semaforo dal descrittore sem_desc
  Semaphore* sem=sem_desc->semaphore;

  // controllo che semaforo esista
  HANDLE_ERROR(!sem,DSOS_SEM_NOT_FOUND,"Errore! Semaforo non valido");

  // prendo puntatore alla entry nella resource list di quel descrittore sem_desc
  SemDescriptorPtr* sem_desc_ptr =(SemDescriptorPtr*) List_detach(&sem->descriptors, (ListItem*)sem_desc->ptr);

  // controllo che SemDescriptorPtr esista
  HANDLE_ERROR(!sem_desc_ptr,DSOS_SEM_DESC_PTR_NOT_FOUND,"Errore! SemDescriptorPtr non valido");

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
