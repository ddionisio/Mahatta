#ifndef _pager_h
#define _pager_h

#include "std_types.h"

/*/
// Pager ID types
//

#define     IDINVALIDATE(id) ((id).counter = 0)
#define		IDISVALID(id) ((id).counter > 0)
#define		IDISEQUAL(id1, id2) ((id1).ID == (id2).ID && (id1).counter == (id2).counter)

typedef struct _ID {
	u32 ID;			//unique identifier (key)
	u64 counter;	//unique counter (0 == invalid ID)

#ifdef __cplusplus
	u8 operator == ( const struct _ID & d ) const { return IDISEQUAL(*this, d); }
#endif

} ID;*/

//*****class version for your convenient
// -- inherit this to any class that requires tracking
//whenever you create game ID, game ID will add itself to a global list to
//keep track of itself.  Removes itself when destroyed
class GameID
{
public:
	GameID(void *thisPtr); //this will add an identifier in the ID list for given 'thisPtr'
	virtual ~GameID();	   //this will remove the identifier in the ID list

	ID GetID() { return m_ID; } //grab identifier

	static void *IDQuery(const ID & key);
	static void IDClear();
private:
	ID m_ID;	//the identifier
};

/////////////////////////////////////
// Name:	PageAdd
// Purpose:	adds an item to a page
// Output:	item added to page
// Return:	none
/////////////////////////////////////
void PageAdd(void *item, ID *pID);

/////////////////////////////////////
// Name:	PageRemove
// Purpose:	remove an item from page
// Output:	item removed
// Return:	none
/////////////////////////////////////
void PageRemove(ID *pID);

/////////////////////////////////////
// Name:	PageClear
// Purpose:	destroys all page and
//			items. Call this on
//			program termination
// Output:	all destroyed
// Return:	none
/////////////////////////////////////
void PageClear();

/////////////////////////////////////
// Name:	PageQuery
// Purpose:	grabs item from page with
//			given ID (key)
// Output:	none
// Return:	the data (NULL if non-existant)
/////////////////////////////////////
void * PageQuery(const ID *key);

#endif