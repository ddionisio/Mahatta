#include "common.h"
#include "STD\memTool.h"

#include <malloc.h>

/////////////////////////////////////
// Name:	MemAlloc
// Purpose:	allocates memory
// Output:	memory allocated
// Return:	new memory
/////////////////////////////////////
void * MemAlloc(u64 size)
{
	return calloc(1, (size_t)size);
}

/////////////////////////////////////
// Name:	MemFree
// Purpose:	frees up given memory ptr.
//			(assumes that this given
//			memory ptr. was allocated
//			using MemAlloc)
// Output:	free up memory
// Return:	none
/////////////////////////////////////
void MemFree(void *mem)
{
	free(mem);
}

/////////////////////////////////////
// Name:	MemRealloc
// Purpose:	reallocate given memory
//			and returns newly reallocated
//			memory
// Output:	memory reallocated
// Return:	new memory
/////////////////////////////////////
void *MemRealloc(void *mem, u64 size)
{
	return realloc(mem, (size_t)size);
}