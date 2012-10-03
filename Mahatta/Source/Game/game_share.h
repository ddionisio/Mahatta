#ifndef _game_share_h
#define _game_share_h

//All the types and definitions shared by engine/server and modules
//This is the first thing modules will include (game.dll/ui.dll)

#include "common.h"

#include "STD\std_types.h"

#include "GFX\gfx_types.h" //includes all graphic and math types
#include "GFX\gfx_bkgrndFX.h"
#include "GFX\gfx_qbsp_flags.h"

#include "INP\input_types.h"

#include "AUDIO\audio_types.h"

extern "C"
{
	#include "Lua\lua.h"
	#include "Lua\lualib.h"
	#include "Lua\lauxlib.h"
}

////////////////////////////////////////////////
// Defines

////////////////////////////////////////////////
// Flags

//engine flags
#define	ENGINE_FLAG_SHOWBBOX			0x00000001		//show bounding boxes of all entites

//value flags
#define VALUE_FLAG_ARCHIVE				0x00000001		//save value to autoexec.cfg

//Entity flags
#define ENTITY_FLAG_NOWORLDCOLLIDE		0x00000001		//don't collide with world

#define ENTITY_FLAG_NONSOLID			0x00000002		//if this is set, then this
														//entity will be ignored by
														//solid entities on collision.
														//But this entity can collide
														//with other entities.
														//Non-Solid entites cannot
														//collide with each other...

#define ENTITY_FLAG_POLLDEATH			0x00000004		//We are going to die on the next
														//update.

#define ENTITY_FLAG_NONSOLID_COLLIDE	0x00000008		//if this is set, non-solid
														//can collide with another 
														//non-solid
#define ENTITY_FLAG_SHADOW				0x00000010		//entity casts shadow

#define ENTITY_FLAG_IGNORE_COLLIDE		0x00000020		//completely ignore collision

#define ENTITY_FLAG_NOGRAVITY			0x00000040		//entity is not affected by gravity

#define ENTITY_FLAG_PUSHABLE			0x00000080		//entity can be pushed by another entity

////////////////////////////////////////////////
// Enumerates

//Input Device types
typedef enum {
	INP_TYPE_KEYBOARD,
	INP_TYPE_MOUSE,
	INP_TYPE_JOYSTICK,

	INP_TYPE_MAX
} eInputType;

//entity shadow types
typedef enum {
	ENTITY_SHADOW_NONE,
	ENTITY_SHADOW_BLOB,
	ENTITY_SHADOW_VOLUME,

	ENTITY_SHADOW_MAX
} eEntShadow;

typedef enum {
	TXTQUALITY_LOWEST,
	TXTQUALITY_LOW,
	TXTQUALITY_MEDIUM,
	TXTQUALITY_HIGH,
	TXTQUALITY_HIGHEST
} eTextureQuality;

////////////////////////////////////////////////
// Structures

typedef struct _wayPt {
	wstring		target;			//the next waypoint path (NULL string if done)
	f32			angle;			//y-axis angle in radians
	f32			delay;			//time it takes to get to next waypoint (-1 if not used)
	f32			wait;			//pause delay before going to next waypoint
	
	Vec3D		pt;
} wayPt;

//Class Flag for you to inherit
class Flag {
public:
	Flag() : m_flags(0) {}

	FLAGS		GetFlags() { return m_flags; }

	void SetFlag(u32 mask, u8 bSet)
	{
		if(bSet)
			SETFLAG(m_flags, mask);
		else
			CLEARFLAG(m_flags, mask);
	}

	bool		CheckFlag(u32 mask) { return TESTFLAGS(m_flags, mask); }

protected:
	FLAGS m_flags;		//the flags
};

////////////////////////////////////////////////
// Handles

//
// Entity Interface
//

typedef class Entity {
public:
	Entity() : m_obj(0), m_mapMdlID(-1), m_flag(0), m_loc(0,0,0), m_dir(0,0,-1), m_vel(0,0,0),
	           m_scale(1,1,1), m_spd(0), m_boundRadius(0), m_boundMin(0,0,0), m_boundMax(0,0,0),
			   m_fxCB(0), m_usrDat(0) { }

	virtual void AddRef() = 0;
	virtual s32 Release() = 0;

	/////////////////////////////////////
	// Purpose:	get the type you set this
	//			entity when created.
	// Output:	none
	// Return:	the type
	/////////////////////////////////////
	virtual u32 GetType() = 0;

	/////////////////////////////////////
	// Purpose:	set the entity model
	//			this will re-create the
	//			object with a new model
	//			give a NULL filename to
	//			just remove the entity's
	//			object and model
	// Output:	model set, obj recreated
	// Return:	TRUE if success
	/////////////////////////////////////
	virtual s32 SetMDL(const tCHAR *filename) = 0;

	//Model and Object Interface Wrapper
	//note: all MDL* only works on entities with objects (hOBJ) (not map models)

	virtual s32 MDLGetAnimID(const tCHAR *name) = 0;

	virtual s32 MDLGetNumAnim() = 0;

	virtual s32 MDLGetBoneID(const tCHAR *name) = 0;

	virtual u32 MDLGetNumMaterial() = 0;

	/////////////////////////////////////
	// Purpose:	set the entity to a model
	//			inside the given map
	//			bSetBound if you want to
	//			set the bound to map model
	// Output:	model index set
	// Return:	TRUE if success
	/////////////////////////////////////
	virtual s32 SetMDLMap(hQBSP qbsp, s32 mdlInd, u8 bSetBound) = 0;

	/////////////////////////////////////
	// Purpose:	set entity Effect, this will
	//			override the model's FXs
	//			Set to NULL to restore model's
	//			FXs
	// Output:	object FX override
	// Return:	none
	/////////////////////////////////////
	virtual void SetFX(hEFFECT fx) = 0;

	/////////////////////////////////////
	// Purpose:	open the area portals
	//			around this entity
	//			Used for doors and such
	//			There are two versions:
	//			one for use with world
	//			other for your own provided map.
	// Output:	area open/close in map
	// Return:	none
	/////////////////////////////////////
	virtual void SetAreaPortalState(u8 open) = 0;
	virtual void SetAreaPortalState(hQBSP qbsp, u8 open) = 0;

	/////////////////////////////////////
	// Purpose:	get the last trace from
	//			collision
	// Output:	none
	// Return:	the trace
	/////////////////////////////////////
	virtual void GetLastTrace(gfxTrace *pTraceOut) = 0;

	/////////////////////////////////////
	// Purpose:	get the last entity that
	//			collided to us from prev.
	//			update.
	// Output:	none
	// Return:	the entity ID
	/////////////////////////////////////
	virtual class Entity *GetLastCollide() = 0;

	///////////////////////////////////////
	//These are entity bound interfaces:

	//collide ent2 to us
	//testT is the 't' comparison for testing if we already
	//hit a minimum value.
	//NOTE: you can set qbsp to 0 to force box collision
	virtual u8 BoundCollide(class Entity *ent2, hQBSP qbsp, gfxTrace *pTrace, f32 testT) = 0;

	//this will only check if sphere intersects
	//both entities
	virtual u8 BoundSphereCheck(class Entity *ent2) = 0;

	//AABB to AABB intersect check
	virtual u8 BoundBoxCheck(class Entity *ent2) = 0;

	///////////////////////////////////////
	//Entity Movement Functions:

	virtual void MoveDir(f32 s) = 0;

	virtual void MoveVec(const Vec3D & v, f32 s) = 0;

	virtual void MoveStrafe(f32 s) = 0;

	//set shadow type
	virtual void SetShadowType(eEntShadow shadowType) = 0;

	//usu. entities are rendered when World is rendered (if they are attached to world)
	virtual s32 Render(hQBSP qbsp, const Vec3D & camLoc) = 0;

	//usu. entities are updated when World is rendered (if they are attached to world)
	virtual s32 Update(hQBSP qbsp) = 0;

	//PUBLIC VARIABLES:

	tCHAR		m_name[MAXCHARBUFF];	//entity's name

	hOBJ		m_obj;					//entity render object (do not set to anything, I'll kick your ass)

	s32			m_mapMdlID;				//the model index to QBSP (-1 if using hOBJ obj, use EntSetMDLMap to set this)

	FLAGS		m_flag;					//entity flags

	Vec3D		m_loc;					//entity location
	Vec3D		m_dir;					//entity direction
	Vec3D		m_vel;					//entity velocity (for movement)
	Vec3D		m_scale;				//entity scale (only works on objects...)

	f32			m_spd;					//entity speed

	f32			m_boundRadius;			//the radius
	Vec3D		m_boundMin,m_boundMax;	//bounding box min/max

	EFFECTFUNC  m_fxCB;					//FX function callback (void * == hENTITY)

	void		*m_usrDat;				//the user data
} *hENTITY;

//The virtual function table for you to export
typedef struct _EntityVFTable {
	s32 (* Init) (hENTITY ent, void *initDat);
	s32 (* Update) (hENTITY ent);
	s32 (* Render) (hENTITY ent, const Vec3D & eyePt);
	s32 (* Destroy) (hENTITY ent);

	//we are colliding to given map
	s32 (* WorldCollide) (hENTITY ent, hQBSP qbsp, gfxTrace *pTrace);

	//an entity is colliding to us
	s32 (* EntityCollide) (hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace);

	//allow ourself to collide with given entity?
	//return TRUE if you want, FALSE o.w. (default: TRUE)
	s32 (* AllowCollide) (hENTITY ent, hENTITY otherEnt);

	s32 (* EntityNoCollide) (hENTITY ent, hENTITY otherEnt);
} EntityVFTable;

//
// Waypoint Node Interface
//

typedef class WayPtNode	{
public:
	virtual void AddRef() = 0;
	virtual s32 Release() = 0;

	/////////////////////////////////////
	// Purpose:	get the current waypoint
	//			location
	// Output:	pLoc set
	// Return:	FALSE if ptOut is null,
	//			wpNode ind invalid or
	//			wp for wpNode doesn't exist
	/////////////////////////////////////
	virtual u8 GetLoc(Vec3D *ptOut) = 0;

	/////////////////////////////////////
	// Purpose:	get the current angle
	//			(radians)
	// Output:	vOut set
	// Return:	the angle
	/////////////////////////////////////
	virtual f32 GetAngle() = 0;

	/////////////////////////////////////
	// Purpose:	get the current wait
	//			delay
	// Output:	none
	// Return:	the wait delay (millisec)
	/////////////////////////////////////
	virtual f32 GetWait() = 0;

	/////////////////////////////////////
	// Purpose:	get the current direction
	// Output:	vOut set
	// Return:	FALSE if vOut is null,
	//			wpNode ind invalid or
	//			wp for wpNode doesn't exist
	//			and if angle doesn't exist
	//			for current wp
	/////////////////////////////////////
	virtual u8 GetDir(Vec3D *vOut) = 0;

	/////////////////////////////////////
	// Purpose:	get the current linear
	//			location of node
	// Output:	ptOut set
	// Return:	FALSE if ptOut is null,
	//			wpNode ind invalid or
	//			wp for wpNode doesn't exist
	/////////////////////////////////////
	virtual u8 GetCurLinearLoc(Vec3D *ptOut) = 0;

	/////////////////////////////////////
	// Purpose:	get the current linear 
	//			interpolated angle
	//			(radians)
	// Output:	vOut set
	// Return:	the angle
	/////////////////////////////////////
	virtual f32 GetCurLinearAngle() = 0;

	/////////////////////////////////////
	// Purpose:	get the current linear
	//			interpolated dir of
	//			the node (based on t)
	// Output:	vOut
	// Return:	FALSE if vOut is null,
	//			wpNode ind invalid or
	//			wp for wpNode doesn't exist
	//			and if angle doesn't exist
	//			for current wp
	/////////////////////////////////////
	virtual u8 GetCurLinearDir(Vec3D *vOut) = 0;

	/////////////////////////////////////
	// Purpose:	get the current curve
	//			location of node
	// Output:	ptOut set, targetOut is
	//			optional
	// Return:	FALSE if ptOut is null,
	//			wpNode ind invalid or
	//			wp for wpNode doesn't exist
	/////////////////////////////////////
	virtual u8 GetCurCurvedLoc(Vec3D *ptOut) = 0;

	/////////////////////////////////////
	// Purpose:	set the next wp immediately
	// Output:	wp next set
	// Return:	none
	/////////////////////////////////////
	virtual void SetNext() = 0;

	/////////////////////////////////////
	// Purpose:	jump to next node (similar
	//			to WPNodeMoveToNext, except
	//			we don't update t)
	// Output:	wpNode set to next
	// Return:	TRUE if we go to next
	/////////////////////////////////////
	virtual u8 GotoNext() = 0;

	/////////////////////////////////////
	// Purpose:	move current pt between
	//			wp and wp next.
	// Output:	current pt set
	// Return:	TRUE if we reached end
	/////////////////////////////////////
	virtual u8 MoveToNext(f32 delay) = 0;

	/////////////////////////////////////
	// Purpose:	check to see if we are
	//			done with moving
	//			(only applies to
	//			WAYPT_STOPATEND)
	// Output:	none
	// Return:	TRUE if we are done
	/////////////////////////////////////
	virtual u8 IsDone() = 0;

} *hWPNODE;

//
// View Interface
//

typedef class View {
public:
	virtual void AddRef() = 0;
	virtual s32 Release() = 0;

	/////////////////////////////////////
	// Purpose:	update the view (for
	//			interpolation)
	// Output:	view updated
	// Return:	none
	/////////////////////////////////////
	virtual void Update() = 0;

	/////////////////////////////////////
	// Purpose:	set the view delay for
	//			interpolation
	// Output:	view delay set
	// Return:	none
	/////////////////////////////////////
	virtual void SetDelay(f32 delay) = 0;

	/////////////////////////////////////
	// Purpose:	check to see if view has
	//			done interpolating
	// Output:	none
	// Return:	TRUE if so
	/////////////////////////////////////
	virtual u8 IsDone() = 0;

	/////////////////////////////////////
	// Purpose:	set point, no interpolation
	// Output:	view point set
	// Return:	none
	/////////////////////////////////////
	virtual void SetNewPt(const Vec3D & pt) = 0;

	/////////////////////////////////////
	// Purpose:	set target, no interpolation
	// Output:	view target set
	// Return:	none
	/////////////////////////////////////
	virtual void SetNewTgt(const Vec3D & tgt) = 0;

	/////////////////////////////////////
	// Purpose:	get the view point
	// Output:	none
	// Return:	the view point
	/////////////////////////////////////
	virtual const Vec3D & GetPt() = 0;

	/////////////////////////////////////
	// Purpose:	get the view target
	// Output:	none
	// Return:	the view target
	/////////////////////////////////////
	virtual const Vec3D & GetTgt() = 0;

	/////////////////////////////////////
	// Purpose:	set point, interpolate
	// Output:	destination pt set
	// Return:	none
	/////////////////////////////////////
	virtual void SetPt(const Vec3D & pt) = 0;

	/////////////////////////////////////
	// Purpose:	set target, interpolate
	// Output:	destination target set
	// Return:	none
	/////////////////////////////////////
	virtual void SetTgt(const Vec3D & tgt) = 0;

	/////////////////////////////////////
	// Purpose:	set the graphic's view
	//			CALL THIS BEFORE RENDERING
	//			ANY 3D OBJECTS!
	// Output:	graphics view set
	// Return:	none
	/////////////////////////////////////
	virtual void SetScene() = 0;

	virtual void CollideMap(hQBSP qbsp) = 0;

	u8	m_bStereo;	//use stereo-scope view?

	//stereo-scope parameters (measurements in inches)

	f32 m_ipd;		//inter pupil distance (distance between our eyes)
	f32 m_wp;		//the physical screen width
	f32 m_dp;		//the physical distance of the user to screen

	f32 m_dv;		//the virtual distance of camera to screen (determines focus and depth of stereo view)
} *hVIEW;

////////////////////////////////////////////////
// Callbacks

//Command Callback
typedef void (* COMMANDPROC) ();

#endif