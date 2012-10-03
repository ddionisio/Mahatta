#include "game_local.h"

//flags
#define DOOR_FLAG_OPEN		1	//we start as opened
#define DOOR_FLAG_MANUAL	2	//door will not generate doortrigger

typedef enum {
	DOOR_CLOSE,		//the door is totally close
	DOOR_OPEN,		//the door is fully open
	DOOR_CLOSING,	//the door is closing down
	DOOR_OPENING,	//the door is opening
	DOOR_STATES_TOTAL  //total of door states
} eDOORSTATE;

struct DoorDat {	
	f32			curWait;		//current wait elapse
	f32			wait;			//how long do we want to wait? (in millisecs)

	f32			curDelay;		//current delay elapse
	f32			delay;			//delay on getting from start to end and vice versa

	u8			bTouch;			//is someone touching us!!??

	Vec3D		sLoc,eLoc;			//starting and ending loc.

	eDOORSTATE state;				//current door state

	FLAGS		dFlags;				//door flags

	s32			sndChannelDoorArr[DOOR_STATES_TOTAL];  //array with channels of sound for the doors
};

struct DoorTrigDat {
	hENTITY			door;				//our door to report to
};

//the teams list
typedef map<wstring, list<hENTITY> >::iterator TmDrsIt;
map<wstring, list<hENTITY> > g_tmDrs;

static u8 _DoorTeamExist(const tCHAR *name)
{
	TmDrsIt it = g_tmDrs.find(name);

	return it != g_tmDrs.end();
}

static void _DoorTeamAdd(hENTITY ent)
{
	if(ent->m_name[0] == 0) return;

	if(!_DoorTeamExist(ent->m_name))
	{
		list<hENTITY> newTeam;
		g_tmDrs[ent->m_name] = newTeam;
	}

	g_tmDrs[ent->m_name].push_back(ent);
}

static void _DoorTeamRemove(hENTITY ent)
{
	if(ent->m_name[0] == 0) return;

	if(_DoorTeamExist(ent->m_name))
	{
		g_tmDrs[ent->m_name].remove(ent);

		if(g_tmDrs[ent->m_name].size() == 0)
			g_tmDrs.erase(ent->m_name);
	}
}

static u8 _DoorTeamIsTouch(hENTITY ent)
{
	if(_DoorTeamExist(ent->m_name))
	{
		for(list<hENTITY>::iterator itE = g_tmDrs[ent->m_name].begin(); itE != g_tmDrs[ent->m_name].end(); itE++)
		{
			DoorDat *pDat = (DoorDat *)(*itE)->m_usrDat;

			if(pDat->bTouch)
				return TRUE;
		}
	}

	return FALSE;
}

//open all doors of this ent's team
static void _DoorTeamOpen(hENTITY ent)
{
	if(_DoorTeamExist(ent->m_name))
	{
		for(list<hENTITY>::iterator itE = g_tmDrs[ent->m_name].begin(); itE != g_tmDrs[ent->m_name].end(); itE++)
		{
			DoorDat *pDat = (DoorDat *)(*itE)->m_usrDat;

			pDat->state = DOOR_OPENING;
			pDat->curDelay = 0;
			(*itE)->SetAreaPortalState(TRUE);

			//Door sounds
			g_gi.SoundPlay(pDat->sndChannelDoorArr[DOOR_OPENING], ent->m_loc);
		}
	}
}

static DoorTrigDat *doorTrig_pDat;

s32 DoorTrigger_Init(hENTITY ent, void *initDat)
{
	doorTrig_pDat = (DoorTrigDat *)ent->m_usrDat;

	//allocate door data
	doorTrig_pDat = (DoorTrigDat *)g_gi.MemAlloc(sizeof(DoorTrigDat));
	ent->m_usrDat = doorTrig_pDat;

	if(!doorTrig_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"DoorTrigger_Init", L"Unable to allocate door trigger data"); return FALSE; }

	hENTITY pDoor = (hENTITY)initDat;

	//set door
	doorTrig_pDat->door = pDoor; pDoor->AddRef();

	//set the bound to be equivalent to door
	ent->m_boundMin = pDoor->m_boundMin;
	ent->m_boundMax = pDoor->m_boundMax;
	ent->m_loc      = pDoor->m_loc;

	ent->m_flag     = ENTITY_FLAG_NOWORLDCOLLIDE | ENTITY_FLAG_NONSOLID | ENTITY_FLAG_NOGRAVITY;

	return TRUE;
}

s32 DoorTrigger_Update(hENTITY ent)
{
	doorTrig_pDat = (DoorTrigDat *)ent->m_usrDat;

	//no entity collided to us, so no one is touching us
	if(!ent->GetLastCollide())
	{
		if(doorTrig_pDat->door && doorTrig_pDat->door->m_usrDat)
			((DoorDat*)doorTrig_pDat->door->m_usrDat)->bTouch = FALSE;
	}

	return TRUE;
}

s32 DoorTrigger_Destroy(hENTITY ent)
{
	doorTrig_pDat = (DoorTrigDat *)ent->m_usrDat;

	if(doorTrig_pDat)
	{
		if(doorTrig_pDat->door) doorTrig_pDat->door->Release();

		g_gi.MemFree(doorTrig_pDat);
	}

	return TRUE;
}

//an entity is colliding to us
s32 DoorTrigger_EntityCollide(hENTITY ent, hENTITY otherEnt, gfxTrace *pTrace)
{
	doorTrig_pDat = (DoorTrigDat *)ent->m_usrDat;
			
	if(otherEnt)
	{
		//tell the door someone is nearby
		if(doorTrig_pDat->door && doorTrig_pDat->door->m_usrDat)
			((DoorDat*)doorTrig_pDat->door->m_usrDat)->bTouch = TRUE;
	}

	return TRUE;
}

//allow ourself to collide with given entity?
//return TRUE if you want, FALSE o.w. (default: TRUE)
s32 DoorTrigger_AllowCollide(hENTITY ent, hENTITY otherEnt)
{
	if(otherEnt)
	{
		//don't collide with door entities
		if(otherEnt->GetType() == ENTITY_DOOR)
			return FALSE;
	}

	return TRUE;
}

static EntityVFTable doortrig_vftbl = {DoorTrigger_Init, DoorTrigger_Update, 0, DoorTrigger_Destroy, 0,
DoorTrigger_EntityCollide, DoorTrigger_AllowCollide, 0};

static DoorDat *door_pDat;

s32 Door_Init(hENTITY ent, void *initDat)
{
	door_pDat = (DoorDat *)ent->m_usrDat;

	//allocate door data
	door_pDat = (DoorDat *)g_gi.MemAlloc(sizeof(DoorDat));
	ent->m_usrDat = door_pDat;

	if(!door_pDat)
	{ g_gi.LogMsg(LOG_FILE, L"EntCB_Door", L"Unable to allocate door data"); return FALSE; }

	const tCHAR *val;
	EntityParse *pEntDat = (EntityParse *)initDat;

	//Get time delay
	val = pEntDat->GetVal(L"time");
	if(val) swscanf(val, L"%f", &door_pDat->delay);
	door_pDat->curDelay = door_pDat->delay;

	//Get wait delay
	val = pEntDat->GetVal(L"wait");
	if(val) swscanf(val, L"%f", &door_pDat->wait);
	door_pDat->curWait = door_pDat->wait;

	//set starting location
	door_pDat->sLoc = ent->m_loc;

	//calculate end location
	f32 angle;

	Vec3D dir;

	val = pEntDat->GetVal(L"angle");
	if(val)
	{
		swscanf(val, L"%f", &angle);

		if(angle == -1) { dir.x = dir.z = 0; dir.y = 1; }
		else if(angle == -2) { dir.x = dir.z = 0; dir.y = -1; }
		else
		{
			Vec3D tdir(1,0,0);

			angle = TO_RAD(angle);

			Matrix mtx;
			g_gi.MtxRotateY(angle, &mtx);
			g_gi.Vec3DTransformNormal(&tdir, &mtx, &dir);
		}
	}

	//get flags
	val = pEntDat->GetVal(L"flags");
	if(val)
		swscanf(val, L"%d", &door_pDat->dFlags);

	//get the max length
	Vec3D v(ent->m_boundMax - ent->m_boundMin);
	f32 len = Vec3DLen(&v);
	door_pDat->eLoc = door_pDat->sLoc + len*dir;

	//set this entity's flags
	ent->m_flag = ENTITY_FLAG_NOWORLDCOLLIDE | ENTITY_FLAG_NOGRAVITY;

	//set current state and position, depending on FLAG
	if(TESTFLAGS(door_pDat->dFlags, DOOR_FLAG_OPEN))
	{
		door_pDat->state = DOOR_OPEN;
		ent->m_loc = door_pDat->eLoc;
		ent->SetAreaPortalState(TRUE);
	}
	else
		door_pDat->state = DOOR_CLOSE;

	//create a trigger entity for automatic door opening
	//then add it to world
	if(!TESTFLAGS(door_pDat->dFlags, DOOR_FLAG_MANUAL))
	{

		hENTITY trigEnt = g_gi.EntCreate(L"door_trigger", ENTITY_DOOR_TRIGGER, 0, 
			doortrig_vftbl, ent);

		if(trigEnt)
		{
			//extend trigger's bound
			val = pEntDat->GetVal(L"extend");
			if(val)
			{
				Vec3D ext;
				swscanf(val, L"%f %f %f", &ext.x, &ext.y, &ext.z);

				trigEnt->m_boundMax += ext;
				trigEnt->m_boundMin -= ext;
			}
			
			g_gi.WorldAddEntity(trigEnt);
			trigEnt->Release();
		}
	}

	//HARDCODED PATH FOR SOUNDS REMOVE ME!!
	door_pDat->sndChannelDoorArr[DOOR_OPENING] = -1;
	door_pDat->sndChannelDoorArr[DOOR_CLOSING] = -1;

	g_gi.SoundSysGetID(L"dooropen", &door_pDat->sndChannelDoorArr[DOOR_OPENING]);
	g_gi.SoundSysGetID(L"doorclose", &door_pDat->sndChannelDoorArr[DOOR_CLOSING]);

	return TRUE;
}

s32 Door_Update(hENTITY ent)
{
	f32 tick = g_gi.EngineGetTick();

	door_pDat = (DoorDat *)ent->m_usrDat;

	switch(door_pDat->state)
	{
	case DOOR_CLOSE:
		//do we want to open?
		if(door_pDat->bTouch)
			_DoorTeamOpen(ent);
		
		break;

	case DOOR_OPEN:
		//if(!door_pDat->bTouch)
		if(!_DoorTeamIsTouch(ent))
			door_pDat->curWait += tick;
		else
			door_pDat->curWait = 0; //wait forever until no one is around

		if(door_pDat->curWait >= door_pDat->wait)
		{
			door_pDat->state = DOOR_CLOSING;
			door_pDat->curDelay = 0;
			//ADD SOUNDS FOR CLOSING THE DOOR
			g_gi.SoundPlay(door_pDat->sndChannelDoorArr[DOOR_CLOSING], ent->m_loc);
		}
		break;

	case DOOR_CLOSING:
		{
			f32 t;

			door_pDat->curDelay += tick;

			t = door_pDat->curDelay/door_pDat->delay;

			//done?
			if(t >= 1.0f)
			{
				t = 1.0f;

				door_pDat->state = DOOR_CLOSE;
				ent->SetAreaPortalState(FALSE);
			}

			ent->m_vel += (door_pDat->eLoc + t*(door_pDat->sLoc - door_pDat->eLoc)) - ent->m_loc;
		}
		break;

	case DOOR_OPENING:
		{
			f32 t;

			door_pDat->curDelay += tick;

			t = door_pDat->curDelay/door_pDat->delay;

			//done?
			if(t >= 1.0f)
			{
				t = 1.0f;

				door_pDat->state = DOOR_OPEN;
				door_pDat->curWait = 0;
			}

			ent->m_vel += (door_pDat->sLoc + t*(door_pDat->eLoc - door_pDat->sLoc)) - ent->m_loc;
		}
		break;
	}

	return TRUE;
}

s32 Door_Destroy(hENTITY ent)
{
	door_pDat = (DoorDat *)ent->m_usrDat;

	_DoorTeamRemove(ent);

	if(door_pDat)
	{
		g_gi.MemFree(door_pDat);
	}

	return TRUE;
}

//allow ourself to collide with given entity?
//return TRUE if you want, FALSE o.w. (default: TRUE)
s32 Door_AllowCollide(hENTITY ent, hENTITY otherEnt)
{
	if(otherEnt)
	{
		//don't collide with door entities
		if(otherEnt->GetType() == ENTITY_DOOR)
			return FALSE;
	}

	return TRUE;
}

static EntityVFTable door_vftbl = {Door_Init, Door_Update, 0, Door_Destroy, 0,0, Door_AllowCollide, 0};

void Spawn_Door(const EntityParse & entDat)
{
	const tCHAR *team, *mdlStr;

	hENTITY newEnt;

	team = entDat.GetVal(L"team");

	mdlStr = entDat.GetVal(L"model");

	//set the bound to be just like the model ID
	newEnt = g_gi.EntCreate(team ? team : L"\0", ENTITY_DOOR, mdlStr, door_vftbl, (void*)&entDat);

	if(newEnt)
	{
		//add to its door team
		_DoorTeamAdd(newEnt);

		g_gi.WorldAddEntity(newEnt);
		newEnt->Release(); //let the world take care of its death
	}
}