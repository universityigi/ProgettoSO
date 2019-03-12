#include <assert.h>
#include <stdio.h>
#include "disastrOS_semdescriptor.h"
#include "pool_allocator.h"
#include "disastrOS_constants.h"

#define SEMDESCRIPTOR_SIZE sizeof(SemDescriptor)
#define SEMDESCRIPTOR_MEMSIZE (sizeof(SemDescriptor)+sizeof(int))
#define MAX_NUM_SEMDESCRIPTORS (MAX_NUM_SEMDESCRIPTORS_PER_PROCESS*MAX_NUM_PROCESSES)
#define SEMDESCRIPTOR_BUFFER_SIZE MAX_NUM_SEMDESCRIPTORS*SEMDESCRIPTOR_MEMSIZE

#define SEMDESCRIPTORPTR_SIZE sizeof(SemDescriptorPtr)
#define SEMDESCRIPTORPTR_MEMSIZE (sizeof(SemDescriptorPtr)+sizeof(int))
#define SEMDESCRIPTORPTR_BUFFER_SIZE MAX_NUM_SEMDESCRIPTORS*SEMDESCRIPTORPTR_MEMSIZE

static char _sem_descriptor_buffer[SEMDESCRIPTOR_BUFFER_SIZE];
static PoolAllocator _sem_descriptor_allocator;

static char _sem_descriptor_ptr_buffer[SEMDESCRIPTORPTR_BUFFER_SIZE];
static PoolAllocator _sem_descriptor_ptr_allocator;

void SemDescriptor_init(){
  int result=PoolAllocator_init(& _sem_descriptor_allocator,
				SEMDESCRIPTOR_SIZE,
				MAX_NUM_PROCESSES,
				_sem_descriptor_buffer,
				SEMDESCRIPTOR_BUFFER_SIZE);
  assert(! result);

  result=PoolAllocator_init(& _sem_descriptor_ptr_allocator,
			    SEMDESCRIPTORPTR_SIZE,
			    MAX_NUM_PROCESSES,
			    _sem_descriptor_ptr_buffer,
			    SEMDESCRIPTORPTR_BUFFER_SIZE);
  assert(! result);
}

SemDescriptor* SemDescriptor_alloc(int fd, Semaphore* res, PCB* pcb) {
  SemDescriptor* d=(SemDescriptor*)PoolAllocator_getBlock(&_sem_descriptor_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->fd=fd;
  d->semaphore=res;
  d->pcb=pcb;
  return d;
}

int SemDescriptor_free(SemDescriptor* d) {
  return PoolAllocator_releaseBlock(&_sem_descriptor_allocator, d);
}

SemDescriptor*  SemDescriptorList_byFd(ListHead* l, int fd){
  ListItem* aux=l->first;
  while(aux){
    SemDescriptor* d=(SemDescriptor*)aux;
    if (d->fd==fd)
      return d;
    aux=aux->next;
  }
  return 0;
}

SemDescriptorPtr* SemDescriptorPtr_alloc(SemDescriptor* descriptor) {
  SemDescriptorPtr* d=PoolAllocator_getBlock(&_sem_descriptor_ptr_allocator);
  if (!d)
    return 0;
  d->list.prev=d->list.next=0;
  d->descriptor=descriptor;
  return d;
}

int SemDescriptorPtr_free(SemDescriptorPtr* d){
  return PoolAllocator_releaseBlock(&_sem_descriptor_ptr_allocator, d);
}

void SemDescriptorList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    SemDescriptor* d=(SemDescriptor*)aux;
    printf("(fd: %d, rid:%d)",
	   d->fd,
	   d->semaphore->id);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}


void SemDescriptorPtrList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("[");
  while(aux){
    SemDescriptorPtr* d=(SemDescriptorPtr*)aux;
    printf("(pid: %d, fd: %d, rid:%d)",
	   d->descriptor->fd,
	   d->descriptor->pcb->pid,
	   d->descriptor->semaphore->id);
    if(aux->next)
      printf(", ");
    aux=aux->next;
  }
  printf("]");
}
