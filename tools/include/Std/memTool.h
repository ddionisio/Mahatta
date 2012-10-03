#ifndef _memTool_h
#define _memTool_h

/////////////////////////////////////
// Name:	MemAlloc
// Purpose:	allocates memory
// Output:	memory allocated
// Return:	new memory
/////////////////////////////////////
void * MemAlloc(u64 size);

/////////////////////////////////////
// Name:	MemFree
// Purpose:	frees up given memory ptr.
//			(assumes that this given
//			memory ptr. was allocated
//			using MemAlloc)
// Output:	free up memory
// Return:	none
/////////////////////////////////////
void MemFree(void *mem);

/////////////////////////////////////
// Name:	MemRealloc
// Purpose:	reallocate given memory
//			and returns newly reallocated
//			memory
// Output:	memory reallocated
// Return:	new memory
/////////////////////////////////////
void *MemRealloc(void *mem, u64 size);


#endif