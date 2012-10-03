#ifndef _entity_h
#define _entity_h

//#include "game_share.h"

#define ENTITY_MAX_COLLIDE		32		//maximum number of entities that can collide per entity

//Entity Implementation for engine

typedef class IEntity *hIENTITY;
typedef list<hIENTITY> EntList;
typedef EntList::iterator EntIt;
typedef EntList::const_iterator const_EntIt;

class IEntity : public Entity {
public:
	IEntity(const tCHAR *name, u32 type, const tCHAR *modelFile, const EntityVFTable &vfTable, void *initDat);
	~IEntity();

	void AddRef();
	s32 Release();

	u32 GetType();
	
	void GetID(ID *pIDOut);

	s32 SetMDL(const tCHAR *filename);

	s32 SetMDLMap(hQBSP qbsp, s32 mdlInd, u8 bSetBound);

	s32 MDLGetAnimID(const tCHAR *name);

	s32 MDLGetNumAnim();

	s32 MDLGetBoneID(const tCHAR *name);

	u32 MDLGetNumMaterial();

	void SetFX(hEFFECT fx);

	void SetAreaPortalState(u8 open);
	void SetAreaPortalState(hQBSP qbsp, u8 open);

	void GetLastTrace(gfxTrace *pTraceOut);

	hENTITY GetLastCollide();

	u8 BoundCollide(class Entity *ent2, hQBSP qbsp, gfxTrace *pTrace, f32 testT);

	u8 BoundSphereCheck(class Entity *ent2);

	u8 BoundBoxCheck(class Entity *ent2);

	void MoveDir(f32 s);

	void MoveVec(const Vec3D & v, f32 s);

	void MoveStrafe(f32 s);

	void SetShadowType(eEntShadow shadowType);

	s32 Render(hQBSP qbsp, const Vec3D & camLoc);

	s32 Update(hQBSP qbsp);

	///////////////////////////////////////
	//These are only used by the engine:

	//just initialize the global box convex hull
	//should only be called once or during world load...
	static void sBoundInit();

	void AddCollider(hIENTITY ent); //add an entity to collide with
	void ClearCollideList();		//clear all entity collider

	//do not modify this list, only use it to traverse
	inline const EntList & GetCollideList() { return m_collideList; }

	///////////////////////////////////////
	//public variables for the engine:

	u8			m_bWrldAttached;	//we are attached to the world (only used in world.cpp)

	gfxEnt		m_entCull;			//used for culling with QBSP
	
	hIENTITY	m_lastCollided;		//last entity to collide to
	u8			m_bcollideWith;		//did we collide with lastCollided? (invalid if world)
	gfxTrace	m_trace;			//used by collision.

	Vec3D		m_pushVel;			//pushvelocity when pushed

	EntityVFTable	m_vftbl;		//the imported virtual function table (straight from the maker!)

private:
	u32			m_type;				//generic type (sorted)

	s32			m_ref;				//number of things refering to this

	hEFFECT		m_fx;				//FX override from model

	EntList     m_collideList;		//list of potential entity collision

	eEntShadow	m_shadType;			//shadow type
	hTXT		m_shadTxt;			//shadow blob texture (not NULL if shadType == ENTITY_SHADOW_BLOB)
};

/////////////////////////////////////
// Name:	EntCreate
// Purpose:	create a new entity with
//			given name, userdata and
//			callback.
//			optional modelFile to load
//			model before calling ENTITYMSG_INIT
//			if this is "*<num>", then we get
//			the modelind from current map
//			(bounds are automatically set)
//			initDat is passed on
//			callback: ENTITYMSG_INIT
//			The type can be anything you
//			want.  This is later used
//			to sort from lowest type value
//			to highest (collision and such)
// Output:	new entity created
// Return:	the new entity
/////////////////////////////////////
hENTITY EntCreate(const tCHAR *name, u32 type, const tCHAR *modelFile, 
				  const EntityVFTable &vfTable, void *initDat);

#endif