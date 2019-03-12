#include <assert.h>
#include <stdio.h>
#include "disastrOS_semaphore.h"
#include "disastrOS_descriptor.h"
#include "pool_allocator.h"
#include "linked_list.h"

#define SEMAPHORE_SIZE sizeof(Semaphore)
#define SEMAPHORE_MEMSIZE (sizeof(Semaphore)+sizeof(int))
#define SEMAPHORE_BUFFER_SIZE MAX_NUM_SEMAPHORES*SEMAPHORE_MEMSIZE

static char _semaphores_buffer[SEMAPHORE_BUFFER_SIZE];
static PoolAllocator _semaphores_allocator;

void Semaphore_init(){
    int result=PoolAllocator_init(& _semaphores_allocator,
				  SEMAPHORE_SIZE,
				  MAX_NUM_SEMAPHORES,
				  _semaphores_buffer,
				  SEMAPHORE_BUFFER_SIZE);
    assert(! result);
}

Semaphore* Semaphore_alloc(int id, int count){
  Semaphore* r=(Semaphore*) PoolAllocator_getBlock(&_semaphores_allocator);
  if (!r)
    return 0;
  r->list.prev=r->list.next=0;
  r->id=id;
  r->count=count;
  List_init(&r->descriptors);
  return r;
}

int Semaphore_free(Semaphore* r) {
  assert(r->descriptors.first==0);
  assert(r->descriptors.last==0);
  return PoolAllocator_releaseBlock(&_semaphores_allocator, r);
}

Semaphore* SemaphoreList_byId(SemaphoreList* l, int id) {
  ListItem* aux=l->first;
  while(aux){
    Semaphore* r=(Semaphore*)aux;
    if (r->id==id)
      return r;
    aux=aux->next;
  }
  return 0;
}

void Semaphore_print(Semaphore* r) {
  printf("id: %d, count:%d, pids:", r->id, r->count);
  DescriptorPtrList_print(&r->descriptors);
  printf("waiting: ");
  DescriptorPtrList_print(&r->waiting_descriptors);
}

void SemaphoreList_print(ListHead* l){
  ListItem* aux=l->first;
  printf("{\n");
  while(aux){
    Semaphore* r=(Semaphore*)aux;
    printf("\t");
    Semaphore_print(r);
    if(aux->next)
      printf(",");
    printf("\n");
    aux=aux->next;
  }
  printf("}\n");
}
