#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  
  // prendo fd dalla syscall
  int fd= running->syscall_args[0];

  printf("[PROCESSO %d] CHIAMATO SEMPOST, fd=%d\n",running->pid,fd);

  // descrittore del semaforo su cui fare sempost
  SemDescriptor* sem_desc=SemDescriptorList_byFd(&running->sem_descriptors,fd);

  // controllo
  if(!sem_desc){
    running->syscall_retvalue=-1;
    return;
  }
  SemDescriptorPtr* sem_desc_ptr;
  

  // semaforo su cui fare sempost
  Semaphore* sem=sem_desc->semaphore;
  printf("id post = %d\n",sem->id);

  // incremento il count
  sem->count++;

  printf("count  post = %d\n",sem->count);

  // se count <=0 allora devo mettere il processo corrente nella ready list
  if(sem->count<=0){
    // prendo il SemDescriptorPtr dalla semaphore waiting list
    sem_desc_ptr=(SemDescriptorPtr*)List_detach(&sem->waiting_descriptors,sem->waiting_descriptors.first);

    // vado a prendere il PCB primo processo che si trova nella waiting list
    PCB* pcb_next=sem_desc_ptr->descriptor->pcb;

    //setto stato a ready
    pcb_next->status=Ready;

    // mi prendo il prossimo processo da schedulare dalla waiting list
    List_detach(&waiting_list,(ListItem*)pcb_next);

    // metto processo nella ready list
    List_insert(&ready_list,ready_list.last,(ListItem*)pcb_next);

    // inserisco SemDescriptor nella sem_descriptor list
    List_insert(&sem->descriptors,sem->descriptors.last,(ListItem*)sem_desc_ptr);

  }
  running->syscall_retvalue=0;
  return;

}
