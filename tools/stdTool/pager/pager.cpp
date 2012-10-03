#include "common.h"

#include "STD\memTool.h"

#include "STD\pager.h"

//page stuff
#define	NUMPP			32	//number of obj per page
#define PAGE_SPOT_ONE	1	//(NUMPP/32)	// Flag indicating spot one on page is used

#define PAGE_PREV_SPOT	1	//(NUMPP/32)	// Amount to decrement mask to move to last spot
#define PAGE_NEXT_SPOT	1	//(NUMPP/32)	// Amount to increment mask to move to next spot

#define PAGE_EMPTY		 0	// Flag indicating empty page
#define PAGE_FULL		~0	// Flag indicating full page

typedef struct _PageItem {
	void *data;
	u64  counter;
} PageItem;

typedef struct _Page {
	FLAGS status;		//stuff it
	PageItem stuff[NUMPP];
} Page;

typedef struct _PageList {
	u32 nStuff;				// number of stuff
	s32 nPages, nMaxPages;	// Count of object pages; maximum possible pages
	Page **Pages;		// Vector of pages of registered objs.
} PageList;

u64 g_uCounter=1;	//unique counter

PageList	 g_pageList={0};

PRIVATE void _PageAllocVector()
{
	Page **newVec;	// Vector of pages to reallocate

	s32 index;	// Loop variable

	newVec = (Page **)MemAlloc(++g_pageList.nMaxPages * sizeof(Page*));

	assert(newVec);

	for (index = 0; index < g_pageList.nPages; ++index)	// Loop through current vector
		newVec[index] = g_pageList.Pages[index];	// Copy pointer data

	if (g_pageList.nPages != 0)	// Check whether pages exist
		MemFree(g_pageList.Pages);

	g_pageList.Pages = newVec;	// Install new page vector
}

PRIVATE Page * _PageAdd()
{
	Page *page = (Page *)MemAlloc(sizeof(Page));	// Client page to allocate

	assert(page);

	if (g_pageList.nPages == g_pageList.nMaxPages)	// Check whether page vector is full
		_PageAllocVector();	// Reallocate page vector

	g_pageList.Pages[g_pageList.nPages++] = page;	// Install page in page vector

	page->status = PAGE_EMPTY;	// Indicate that all page blocks are available

	return page;// Return newly allocated page
}

PRIVATE void _PageClean(Page *page)
{
	PageItem *obj = page->stuff, *nextObj;	// Reference to objs on page

	if (page->status == PAGE_EMPTY) return;	// Trivial success if page is empty

	while(page->status != PAGE_EMPTY)	// Scan through all objects
	{
		nextObj = obj; nextObj++;
		if (GETFLAGS(page->status,PAGE_SPOT_ONE))	// Check whether block is occupied
		{
			// destroy obj?
			obj->counter=0;
			obj->data=0;
		}

		obj=nextObj;

		page->status >>= PAGE_PREV_SPOT;	// Update mask for next block
	}
}

PRIVATE void _PageRemoveLast()
{
	Page *page = g_pageList.Pages[--g_pageList.nPages];	// Obtain final page from manager

	// Perform cleanup on page
	_PageClean(page);

	// Release page memory
	MemFree(page);
}

/////////////////////////////////////
// Name:	PageAdd
// Purpose:	adds an item to a page
// Output:	item added to page
// Return:	none
/////////////////////////////////////
void PageAdd(void *item, ID *pID)
{
	assert(item);	// Verify that Data points to valid memory

	s32 pIndex, bIndex;	// Index of client page; index of client block

	Page *page;

	FLAGS Mask = PAGE_SPOT_ONE;	// Mask used to test page availability

	if (g_pageList.nPages * NUMPP == g_pageList.nStuff)	// Check whether all pages are full
	{
		pIndex = g_pageList.nPages;	// Get index of new page

		page = _PageAdd();	// Append a new page to the manager
	}

	else	// Pages are available
	{
		for (pIndex = 0; pIndex < g_pageList.nPages; ++pIndex)	// Loop through all pages
			if (g_pageList.Pages [pIndex]->status != PAGE_FULL) break;	// Break if page has openings

		page = g_pageList.Pages [pIndex];	// Obtain refence to page
	}

	for (bIndex = 0; bIndex < NUMPP; ++bIndex)	// Scan block occupancy flags
	{
		if (!GETFLAGS(page->status,Mask)) break;	// Break if block is open

		Mask <<= PAGE_NEXT_SPOT;	// Update mask for next block
	}

	SETFLAG(page->status,Mask);	// Indicate block is in use

	page->stuff[bIndex].data = item;	// set obj ptr

	pID->ID = pIndex * NUMPP + bIndex;	// Obtain obj ID
	page->stuff[bIndex].counter = pID->counter = ++g_uCounter;

	++g_pageList.nStuff;	// Increment item count
}

/////////////////////////////////////
// Name:	PageRemove
// Purpose:	remove an item from page
// Output:	item removed
// Return:	none
/////////////////////////////////////
void PageRemove(ID *pID)
{
	Page *page;	//page where obj resides

	s32 pIndex = pID->ID / NUMPP;	// Page index

	// Ascertain that ID is within the messager's pages
	ASSERT_MSG(pIndex < g_pageList.nPages,L"Invalid ID",L"PageRemove");

	page = g_pageList.Pages [pIndex];	// Obtain client page

	pID->ID %= NUMPP;	// Convert client ID to index

	CLEARFLAG(page->status,PAGE_SPOT_ONE << pID->ID);	// Mark block as available

	//if(page->status == PAGE_EMPTY && pIndex + 1 == g_pageList.nPages)	// Check whether last page is empty
	//	_PageRemoveLast();

	--g_pageList.nStuff;	// Decrement obj count
}

/////////////////////////////////////
// Name:	PageClear
// Purpose:	destroys all page and
//			items. Call this on
//			program termination
// Output:	all destroyed
// Return:	none
/////////////////////////////////////
void PageClear()
{
	while(g_pageList.nPages != 0)		// Iterate until all pages are gone
			_PageRemoveLast();	// Remove final page

	if(g_pageList.nMaxPages != 0)	// Check whether client pages exist
		MemFree(g_pageList.Pages);	// Release page vector

	memset(&g_pageList, 0, sizeof(PageList));
}

PRIVATE void * _PageGet(u32 ID, u64 *pCounter)
{
	Page *page;	//page where obj resides

	s32 pIndex = ID / NUMPP;	// Page index
	s32 bIndex = ID % NUMPP;	// Block index

	if (pIndex >= g_pageList.nPages) return NULL;	// Trivially fail if client cannot exist

	page = g_pageList.Pages [pIndex];	// Refer to client page

	if (GETFLAGS(page->status,PAGE_SPOT_ONE << bIndex))	// Check whether block is in use
	{
		if(pCounter)
			*pCounter=page->stuff [bIndex].counter;
		return page->stuff [bIndex].data;	// If so, return client data
	}

	return 0;// Client is unavailable
}

/////////////////////////////////////
// Name:	PageQuery
// Purpose:	grabs item from page with
//			given ID (key)
// Output:	none
// Return:	the data (NULL if non-existant)
/////////////////////////////////////
void * PageQuery(const ID *key)
{
	void *data;
	u64 counter;

	assert(key);// Verify that Key points to valid memory

	data = _PageGet(key->ID, &counter);	// Attempt to obtain client

	if (data != 0 && counter == key->counter)	// Check whether key matches
		return data;	// If so, return client

	return 0;// Client is unavailable
}

//////////////////////////////////////////////////////////////////////
//class GameID
GameID::GameID(void *thisPtr)
{
	PageAdd(thisPtr, &m_ID);
}

GameID::~GameID()
{
	PageRemove(&m_ID);
}

void * GameID::IDQuery(const ID & key)
{
	return PageQuery(&key);
}

void GameID::IDClear()
{
	PageClear();
}