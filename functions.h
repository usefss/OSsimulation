#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include "processStrct.h"
int sortAreadinp(struct Process processArr[]);
void showqueue(struct Process* queue, int limit);
void showProcessInf(struct Process p);
void FIFOadd(struct Process object, struct Process* queue, int* limit);
void* FIFOextract(struct Process* queue, int* limit);

#endif