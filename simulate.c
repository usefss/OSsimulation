#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "functions.h"
#define MAXPROCESS 100
#define MAXMEMSIZE 8589934592 /* 8GB */

long int allocateMemory(struct Process* p, struct Partition* Memory, int* Mlim)
{
	int bestfit = -1;
	long int bestsize = MAXMEMSIZE;
	int i;
	for (i = 0; i < *Mlim; i ++){ // for all free partitions in partition memory table
		if (Memory[i].state == 'F' && p->memNeed <= Memory[i].size)
			if (Memory[i].size <= bestsize){ // here is allocation best fit policy
				bestfit = i;
				bestsize = Memory[i].size;
			}
	}
	if (bestfit == -1) return -1;
	// other wise founded a best fit then must allocate processs to the best fit partition
	struct partition busy;
	busy.size = p->size;
	busy.address = Memory[bestfit].address;
	busy.access = p;
	p.allocation = busy;
	status = 'B';
	if (Memory[bestfit].size - p->size == 0){
		Memory[bestfit] = busy;
		return bestfit;
	}
	// other size there will be a over head partition
	struct partition free;
	free.size = Memory[bestfit].size - p->size;
	free.address = Memory[bestfit].address + p->size;
	status = 'F';
	Memory[bestfit] = busy;
	Memory[(*Mlim)++] = free;
	for (i = Mlim - 1; i > bestfit + 1; i --)
		swap((char *)Memory[i], (char *)Memory[i - 1]);
	return bestfit;
}

void uniteBellowPartitions(int index, struct Partition* Memory, int* Mlim)
{

	if (Memory[index + 1].status == 'F'){
		int i;
		Memory[index].size += Memory[index + 1].size;
		for (i = index + 1; i < *Mlim - 1; i ++){
			 	swap((char*) Memory[i], (char*) Memory[i + 1]);
			 	if (Memory[i].status == 'B')
			 			Memory[i].access->allocation = &Memory[i];
	 	}
 		(*Mlim)--
		uniteBelowPartitions(index, Memory, Mlim);
	}
}

void deallocateMemory(struct Process* p, struct Partition* Memory, int* Mlim)
{
	struct Partition* part = p->allocation;
	p->allocation = NULL;
	part->access = NULL;
	part->status = 'F';
	// now that the partition is free memory manager must unite every partition that are neighbours
	int index = ((void*) part - (void*) Memory) / sizeof(struct Partition);
	while(index > 0 && Memory[index - 1] == 'F') index --;
	uniteBelowPartitions(index, Memory, Mlim);
}

checkBlockedes(struct Process* Block, int* Blim, struct Process* Ready, int* Rlim)
{
	// to do : before check just sort block to order by memory need
	int i;
	long int = result;
	for (i = 0; i < *Blim; i ++){
		result = allocateMemory(&Block[i], Memory, Mlim);
		if (result != -1){
			printf(" ** blocked process %s has been allocated to %ld", Block[i].name, result);
			FIFOadd((struct Process*)FIFOextract(i, Block, Blim), Ready, Rlim);
		}
	}
}
void checkNewCommers(int globtimer, struct Process* New, int *Nlim, struct Process* Ready, int* Rlim,
	 									struct Partition* Memory, int* Mlim, struct Process* Block, int* Blim);
{
	int i = 0;
	long int result;
	while (i < *Nlim){
		result = -1;
		if (New[i].startT < globtimer + 1){
			printf("%dms: prcess %s arrived ...\n", globtimer, New[i].name);
			result = allocateMemory(&New[i], Memory, Mlim);
			if (result == -1){
				printf("				... and it has blocked\n");
				FIFOadd((struct Process*)FIFOextract(0, New, Nlim), Block, Blim);
			} else{
				printf("				... and allocated to %ld\n", Memory[result].address);
				FIFOadd((struct Process*)FIFOextract(0, New, Nlim), Ready, Rlim);
			}
		}
		else
			break;
	}
}

int initMemory(struct Partition* memory)
{
	int limit = 1;
	memory->address = 0;
	memory->size = MAXMEMSIZE; // maximom size of memory
	memory->access = NULL;
	memory->status = 'F';
	return limit;
}

int main()
{
	int quantum = 50;
	struct Process New[MAXPROCESS]; // new queue process
	int Nlim = sortAreadinp(New); // returns sorted processes by start time
	struct Process Ready[MAXPROCESS];
	int Rlim = 0;
	struct Process Terminate[MAXPROCESS];
	int Tlim = 0;
	struct partition Memory[MAXPROCESS];
	int Mlim = initMemory(Memory);
	struct Process Block[MAXPROCESS];
	int Blim = 0;

	int globtimer = New[0].startT;
	checkNewCommers(globtimer, New, &Nlim, Ready, &Rlim,
									Memory, &Mlim, Block, &Blim);
	struct Process* running;

	printf("NEW ");showqueue(New, Nlim);
	printf("READY ");showqueue(Ready, Rlim);

	printf("[***] scheduler started\n\n");
	if (Rlim > 0) while(7999) // round rabin scheduler
	{
		printf("ready queue: ");showqueueByname(Ready, Rlim);
		printf("block queue: ");showqueueByname(Block, Blim);
		running = (struct Process *) FIFOextract(0, Ready, &Rlim);
		// process consumes cpu
		if (running == NULL)
			globtimer = New[0].startT;
		else if (running->burstT > quantum + 1){
			globtimer += quantum;
			running->burstT -= quantum;
			FIFOadd(running, Ready, &Rlim);
			printf("%dms: %s take %dms and remained: %d ...\n", globtimer, running->name, quantum, running->burstT);
		} else{
			globtimer += running->burstT;
			// terminate process
			printf("%dms: %s take %dms and terminated...\n", globtimer, running->name, running->burstT);
			deallocateMemory(running, Memory, &Mlim); ///
			FIFOadd(running, Terminate, &Tlim);
			/// after dealocating memory there is a chance for blocked processes
			checkBlockedes(Block, &Blim, Ready, &Rlim);
		}
		usleep(1000);
		checkNewCommers(globtimer, New, &Nlim, Ready, &Rlim,
										Memory, &Mlim, Block, &Blim);
		if (Nlim == 0 && Rlim == 0) break; // means no process will ever come
	}
	printf("\n[**] scheduling ended...\n");

	printf("NEW ");showqueue(New, Nlim);
	printf("READY ");showqueue(Ready, Rlim);
	printf("TERMINATE ");showqueue(Terminate, Tlim);
	return 0;
}
