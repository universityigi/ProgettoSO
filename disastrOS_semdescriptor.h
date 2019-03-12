#pragma once
#include "linked_list.h"
#include "disastrOS_pcb.h"
#include "disastrOS_semaphore.h"


struct SemDescriptorPtr;

typedef struct SemDescriptor{
  ListItem list;
  PCB* pcb;
  Semaphore* semaphore;
  int fd;
  struct SemDescriptorPtr* ptr; // pointer to the entry in the resource list
} SemDescriptor;

typedef struct SemDescriptorPtr{
  ListItem list;
  ListItem DescriptorPtr;
  SemDescriptor* descriptor;
} SemDescriptorPtr;

void SemDescriptor_init();
SemDescriptor* SemDescriptor_alloc(int fd, Semaphore* res, PCB* pcb);
int SemDescriptor_free(SemDescriptor* d);
SemDescriptor*  SemDescriptorList_byFd(ListHead* l, int fd);
void SemDescriptorList_print(ListHead* l);

SemDescriptorPtr* SemDescriptorPtr_alloc(SemDescriptor* descriptor);
int SemDescriptorPtr_free(SemDescriptorPtr* d);
void SemDescriptorPtrList_print(ListHead* l);
