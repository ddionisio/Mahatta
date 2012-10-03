#ifndef _game_local_h
#define _game_local_h

//Our local header for the game module (game.dll)
//Only this module includes this header
//This is the common header for all source in game.dll

#include "game_share.h"

#include "game.h"

////////////////////////////////////////////////////////////////
// constants
#define		VALUE_SHADOW_TYPE			(L"shadow")

////////////////////////////////////////////////////////////////
// flags

//game
#define		GAME_HELP_TOGGLE	0x00000001l
#define		GAME_SHOW_FPS		0x00000002l

//player
#define		PLYR_FLAG_RUN			0x00000001l		//player is running
#define		PLYR_FLAG_RUNTOGGLE		0x00000002l		//running is toggled

////////////////////////////////////////////////////////////////
// enumerates

typedef enum {
	//objects
	ENTITY_DOOR,
	ENTITY_DOOR_TRIGGER,

	ENTITY_LOADMAP,

	//characters
	ENTITY_NPC,
	ENTITY_PLAYER,
	ENTITY_TP, // Team player (rodo)
	ENTITY_TP_SCOUT,

	//items

	//misc
	ENTITY_DOODAD,
	ENTITY_BLOCK,
	ENTITY_BULLET,
	ENTITY_BALL,
	ENTITY_BALL_SCOUT,

	//areas
	ENTITY_SCOREAREA, // (rodo)
	ENTITY_RINGAREA, // (rodo)
	ENTITY_COURTSIDE, // (rodo)

	ENTITY_MAX_TYPE
} EntityType;

//player movement animations
typedef enum {
	PLYR_STATE_IDLE_0,
	PLYR_STATE_IDLE_1,
	PLYR_STATE_IDLE_2,
	PLYR_STATE_IDLE_MAX,

	PLYR_STATE_WALK = PLYR_STATE_IDLE_MAX,
	PLYR_STATE_RUN,
	PLYR_STATE_BWALK,
	PLYR_STATE_STRAFE_L,
	PLYR_STATE_STRAFE_R,

	PLYR_STATE_MAX
} PlyrMovAnim;

//movement states
typedef enum {
	PLYR_MOVE_IDLE,			//idle, various
	PLYR_MOVE_WALK,			//walking
	PLYR_MOVE_RUN,			//running
	PLYR_MOVE_STRAFE_L,		//strafing left
	PLYR_MOVE_STRAFE_R,		//strafing right
	PLYR_MOVE_BWALK,		//back walking

	PLYR_MOVE_WALKTORUN,	//going from walk to run cycle
	PLYR_MOVE_RUNTOWALK,	//going from run to walk cycle

	PLYR_MOVE_MAX
} PlyrMove;

typedef enum {
	PLYR_ACTION_SHOOT,
	PLYR_ACTION_BALL,

	PLYR_ACTION_MAX
} PlyrAction;

//npc enums

typedef enum {
	NPC_STATE_IDLE_0,
	NPC_STATE_IDLE_1,
	NPC_STATE_IDLE_2,
	NPC_STATE_IDLE_MAX,

	NPC_STATE_WALK = NPC_STATE_IDLE_MAX,

	NPC_STATE_MAX
} NPCMovAnim;

////////////////////////////////////////////////////////////////
// classes/structures

class Player : public Flag {
public:
	Player(const tCHAR *name, const tCHAR *datName);
	~Player();

	//set the movement
	void Move(PlyrMove move);

	void PerformAction(PlyrAction a);

	void AddToWorld(const Vec3D & loc, const Vec3D & dir);

	void Rotate(f32 amt);

	void UpdateView(f32 mX, f32 mY);

	hENTITY GetEnt() { return m_ent; }

	void SetTargetStr(const tCHAR *name) { if(name) m_tgtSpawnStr = name; else m_tgtSpawnStr = L"\0"; }
	void GetTargetStr(tCHAR *pStr) { wcscpy(pStr, m_tgtSpawnStr.c_str()); }

	void ClearMovStates();				//clear all movement animations

private:
	hENTITY		m_ent;					//the playa' entity
	wstring		m_tgtSpawnStr;			//the target spawn string for info_player_deathmatch
};

struct bulletInit {
	s32 maxBounce;
	f32 spd;

	s32 sndBounce;
	s32 sndExplode;
};

hENTITY BulletCreate(const tCHAR *mdlStr, const bulletInit & bi);

//the local struct global for this module
typedef struct {
	
	class TeamPlayer	*player;
	class Team			*localteam;

	lua_State	* mLua;

	hVIEW		view;

	u8			viewbLook;				//are we in look-at mode?
	Vec3D		viewDir;				//view direction from player
	Vec3D		viewDirDefault;			//view default direction from player
	f32			viewRotX,viewRotY;		//view rotation (radians)

	f32			mPhysicsStep;	//< Time step for physical updates

	vector<hEFFECT>	sceneFX;			//the scene FXs
	s32				curFX;				//-1 for no selection

	hTXT			loadImg;			//the load image

	hFNT			dbgFnt;				//for debug purposes

	u32				numTri;

	Vec3D			flareLoc;

	hTXT			flareImg;

	FLAGS			flag;				//flags for the game
} GameLocal;

//////////////////////////////////////////////
// Globals

//provides us with the global from the engine
extern GameImport	g_gi;
extern GameExport	g_ge;

extern GameLocal	g_game;

//////////////////////////////////////////////
// Functions

/////////////////////////////////////
// Name:	GameDisplayLoad
// Purpose:	display the loading thing
//			centered
// Output:	stuff
// Return:	none
/////////////////////////////////////
void GameDisplayLoad(u8 bClear);

//
// Game Entity
//

/////////////////////////////////////
// Name:	GameEntityLoad
// Purpose:	generic entity load from
//			ini file (Data\Entity)
// Output:	entity loaded
// Return:	
/////////////////////////////////////
hENTITY GameEntityLoad(const tCHAR *name, u32 type, const tCHAR *dataName, const EntityVFTable & vftbl);

/////////////////////////////////////
// Name:	GameCmdInit
// Purpose:	initialize all commands
//			for game module
// Output:	all commands initialized
// Return:	none
/////////////////////////////////////
void GameCmdInit();

//
// HUD
//

void HUDInit();

void HUDDestroy();

void HUDRender();

#endif