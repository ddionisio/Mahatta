#ifndef _world_h
#define _world_h

#include "entity.h"

/////////////////////////////////////
// Name:	WorldGetMap
// Purpose:	get the QBSP
// Output:	none
// Return:	the QBSP
/////////////////////////////////////
hQBSP WorldGetMap();

/////////////////////////////////////
// Name:	WorldLoad
// Purpose:	load the world
// Output:	world loaded
// Return:	TRUE if success
/////////////////////////////////////
s32 WorldLoad(const tCHAR *filename);

//post a map to be loaded after update (game.dll)
s32 WorldPostMap(const tCHAR *filename);

//check the path of posted map
const tCHAR * WorldGetPostMapPath();

//load the posted map
s32 WorldLoadPostMap();

/////////////////////////////////////
// Name:	WorldDestroy
// Purpose:	destroy current world
// Output:	world destroyed!
// Return:	none
/////////////////////////////////////
void WorldDestroy();

/////////////////////////////////////
// Name:	WorldSetGravity
// Purpose:	set gravity
// Output:	gravity set
// Return:	none
/////////////////////////////////////
void WorldSetGravity(const Vec3D & v);

/////////////////////////////////////
// Name:	WorldGetGravity
// Purpose:	get gravity
// Output:	none
// Return:	gravity
/////////////////////////////////////
Vec3D WorldGetGravity();

/////////////////////////////////////
// Name:	WorldUpdate
// Purpose:	update the world and the
//			entity.
// Output:	a lot stuff happens
// Return:	TRUE for success
/////////////////////////////////////
s32 WorldUpdate();

/////////////////////////////////////
// Name:	WorldDisplay
// Purpose:	renders the world and the
//			entities
// Output:	stuff displayed
// Return:	TRUE for success
/////////////////////////////////////
s32 WorldDisplay(const Vec3D & eye);

/////////////////////////////////////
// Name:	WorldPreview
// Purpose:	preview collisions on an
//			entity
// Output:	stuff moved around
// Return:	none
/////////////////////////////////////
void WorldPreview(hENTITY ent);

/////////////////////////////////////
// Name:	WorldPreviewEx
// Purpose:	preview collisions on a
//			list of entities
// Output:	stuff moved around
// Return:	none
/////////////////////////////////////
void WorldPreviewEx(hQBSP qbsp, std::list<hENTITY> & entities);

//
// Entity
//

/////////////////////////////////////
// Name:	WorldAddEntity
// Purpose:	add entity to world, this
//			will make the entity
//			update and display
// Output:	entity added to list
// Return:	none
/////////////////////////////////////
void WorldAddEntity(hENTITY ent);

/////////////////////////////////////
// Name:	WorldRemoveEntity
// Purpose:	remove entity from world, this
//			will make the entity
//			not update and display
// Output:	entity removed from list
// Return:	none
/////////////////////////////////////
void WorldRemoveEntity(hENTITY ent);

/////////////////////////////////////
// Name:	WorldEntityExist
// Purpose:	check to see if given
//			entity is in the world
// Output:	none
// Return:	TRUE if found
/////////////////////////////////////
u8 WorldEntityExist(hENTITY ent);

/////////////////////////////////////
// Name:	WorldEnumEntities
// Purpose:	invokes a callback on each
//			entity in the world
// Output:	none
// Return:	none
/////////////////////////////////////
void WorldEnumEntities (void (*Enum)(hENTITY, void*), void * context);

//
// Target
//

/////////////////////////////////////
// Name:	WorldTargetSet
// Purpose:	add/change a target in world
// Output:	add/set target
// Return:	none
/////////////////////////////////////
void WorldTargetSet(const tCHAR *name, const Vec3D & pt);

/////////////////////////////////////
// Name:	WorldTargetGet
// Purpose:	get the target
// Output:	ptOut set
// Return:	TRUE if found
/////////////////////////////////////
u8 WorldTargetGet(const tCHAR *name, Vec3D *ptOut);

//
// Waypoint
//

/////////////////////////////////////
// Name:	WorldWPExist
// Purpose:	check to see if given
//			WP name exists
// Output:	none
// Return:	TRUE if exists
/////////////////////////////////////
u8 WorldWPExist(const tCHAR *name);

/////////////////////////////////////
// Name:	WorldWPSet
// Purpose:	set a waypoint
// Output:	new waypt added if not exist
// Return:	none
/////////////////////////////////////
void WorldWPSet(const tCHAR *name, const wayPt & wp);

/////////////////////////////////////
// Name:	WorldWPGet
// Purpose:	get wp data from given
//			index
// Output:	wpOut set
// Return:	TRUE if wp is found
/////////////////////////////////////
u8 WorldWPGet(const tCHAR *name, wayPt * wpOut);

//Exclusive to engine:

/////////////////////////////////////
// Name:	WorldCmdInit
// Purpose:	initialize world basic
//			commands
// Output:	more basic commands (like map)
// Return:	none
/////////////////////////////////////
void WorldCmdInit();

/////////////////////////////////////
// Name:	WorldCollisionExec
// Purpose:	collide everything!
// Output:	stuff moved around
// Return:	none
/////////////////////////////////////
void WorldCollisionExec(hQBSP qbsp, const EntList & entities);//const list<ID> & entities);

/////////////////////////////////////
// Name:	WorldTargetAddEnt
// Purpose:	add a target based on given
//			entity parse
// Output:	new target added
// Return:	none
/////////////////////////////////////
void WorldTargetAddEnt(const EntityParse & entityDat);

/////////////////////////////////////
// Name:	WorldTargetClear
// Purpose:	clear out all targets
// Output:	no more targets
// Return:	none
/////////////////////////////////////
void WorldTargetClear();

/////////////////////////////////////
// Name:	WorldWPAddEnt
// Purpose:	add a target based on given
//			entity parse
// Output:	new target added
// Return:	none
/////////////////////////////////////
void WorldWPAddEnt(const EntityParse & entityDat);

/////////////////////////////////////
// Name:	WorldWPClear
// Purpose:	clear out all waypoints
// Output:	all wps removed
// Return:	none
/////////////////////////////////////
void WorldWPClear();

#endif