#ifndef GAME_TEAMPLAYER_H
#define GAME_TEAMPLAYER_H

#include "game_share.h"
#include "game_steering.h"

#define TEAMPLAYERTYPE_AI 1
#define TEAMPLAYERTYPE_HUMANREMOTE 2
#define TEAMPLAYERTYPE_HUMANLOCAL 3

typedef enum {
	TP_HUD_FACE_NORMAL,
	TP_HUD_FACE_BAD,
	TP_HUD_FACE_KO,
	TP_HUD_FACE_HIT,

	TP_HUD_NAME_NORMAL,
	TP_HUD_NAME_BAD,
	TP_HUD_NAME_KO,

	TP_HUD_MAXTEXTURES
} TPHudTexture; // for getting textures

typedef enum {
	TP_HUDSTATE_NORMAL,
	TP_HUDSTATE_BAD,
	TP_HUDSTATE_KO,
	TP_HUDSTATE_HIT,

	TP_HUDSTATE_MAX
} TPHudState;

typedef enum {
	TP_HUDELEMENT_FACE,
	TP_HUDELEMENT_NAME,

	TP_HUDELEMENT_MAX
} TPHudTextureElement;

// cycle animations
typedef enum {
	TP_STATE_IDLE1,
	TP_STATE_IDLE2,
	TP_STATE_IDLE3,
	TP_STATE_IDLE4,
	TP_STATE_IDLE5,

	TP_STATE_WALKF, 
	TP_STATE_WALKSTRAFER,
	TP_STATE_WALKSTRAFERF,
	TP_STATE_WALKSTRAFERB,
	TP_STATE_WALKSTRAFEL,
	TP_STATE_WALKSTRAFELF,
	TP_STATE_WALKSTRAFELB,
	TP_STATE_WALKB,

	TP_STATE_RUNF, 
	TP_STATE_RUNSTRAFER,
	TP_STATE_RUNSTRAFERF,
	TP_STATE_RUNSTRAFERB,
	TP_STATE_RUNSTRAFEL,
	TP_STATE_RUNSTRAFELF,
	TP_STATE_RUNSTRAFELB,
	TP_STATE_RUNB,

	TP_STATE_DOWNED,

	TP_STATE_SCAREU,
	TP_STATE_SCARED,
	
	TP_STATE_MAX
} TPMove;

// action animations
typedef enum {
	TP_ACTION_SERVE,

	TP_ACTION_SHOOTJL,
	TP_ACTION_SHOOTUL,
	TP_ACTION_SHOOTDL,

	TP_ACTION_SHOOTJR,
	TP_ACTION_SHOOTUR,
	TP_ACTION_SHOOTDR,

	TP_ACTION_HITU,
	TP_ACTION_HITM,
	TP_ACTION_HITD,

	TP_ACTION_DROPDOWN,
	TP_ACTION_RISE,

	TP_ACTION_WINPOINT1,
	TP_ACTION_WINPOINT2,
	TP_ACTION_WINPOINT3,

	TP_ACTION_WINMATCH1,
	TP_ACTION_WINMATCH2,
	TP_ACTION_WINMATCH3,

	TP_ACTION_TAUNT1,
	TP_ACTION_TAUNT2,
	TP_ACTION_TAUNT3,

	TP_ACTION_MAX
} TPAction;

//speeds
typedef enum {
	SPEED_WALKF,
	SPEED_WALKSTRAFE,
	SPEED_WALKB,
	SPEED_RUNF,
	SPEED_RUNSTRAFE,
	SPEED_RUNB,
	
	SPEED_MAX
} SpeedType;

struct TPDat {

	std::wstring teamsideid; // team side ID, this is for loading the corresponding player
							 // files as specified by the selected team pointer in match

	std::wstring playerposid; // player position ID, this is for mapping which player will
							  // appear on 'this' slot

	//*/when player gets near, delay for a bit once the player is away
	f32			m_pauseCurDelay;
	f32			m_pauseDelay;

	//slowly get to the next direction
	Vec3D		m_dirCur, m_dirNext;			//the next direction
	f32			m_dirCurDelay;
	f32			m_dirDelay;

	f32			m_idleCurDelay;
	f32			m_idleDelay;

	// BEGIN Player 'inherited' things
	s32			m_movAnim[TP_STATE_MAX];	//the animation IDs for each state
	s32			m_animAction[TP_ACTION_MAX];	//the animation IDs for each action
	f32			m_speed[SPEED_MAX];		//various speeds
	
	Vec3D		m_lookatOfs;		//the look-at offset
	f32			m_lookDist;			//how far do we look away

	TPMove		m_move;				//current movement
	TPMove		m_moveNext;			//next movement (m_move is set to this on move update)

	f32			m_runDelay;			//delay from walk<->run
	f32			m_runCurDelay;		//current counter delay from walk<->run

	s32			m_soundArray[TP_STATE_MAX]; //array that contains all the ids of the sounds loaded
	s32			m_soundCur;	///< Sound that is being played

	float mNeighborRange;	///< Range within which neighbors fall

	Vec3D mFacing;	///< Direction player should face when idle
	Vec3D mMotion;	///< Current amount of player motion
	Vec3D mOrigin;	///< Original player position

	std::list<Vec3D> mPath;	///< Set of temporary waypoints to follow

	class TeamPlayer * mPlayer;	///< Player bound to entity

	int mSteering;	///< Steering features
	int mProc;	///< Update coroutine

	TPDat (void);
	~TPDat (void);

	void StopSound (void);
	void PlaySound (u32 index);

	// END
};

class TeamPlayer: public Flag {
	public:
		TeamPlayer(tCHAR const name[], tCHAR const file[]);
		TeamPlayer();
		~TeamPlayer();
		
		// general configuration methods
		tCHAR* getName(){
			return name;
		}
		tCHAR* getFile(){
			return file;
		}
		
		// hud related
		std::wstring getHudFile(u8 id){
			return hudTexturesFiles[id];
		}
		TPHudState getHudState(){
			return hudstate;
		}

		// attribute related
		f32 getHealthPercent(){
			return (currhealth * 1.0f) / (maxhealth * 1.0f);
		}
		f32 getMagicPercent(){
			return (currmagic * 1.0f) / (maxmagic * 1.0f);
		}
		void drainHealth(u8 amount, Vec3D *projectilepos = NULL);
		void drainMagic(u8 amount);

		// player type related methods
		void setType(u8 otype){
			type = otype;
		}
		u8 getType(){
			return type;
		}

		// action duration related methods
		void setActionDuration(f32 duration){
			actionduration = duration;
			actiontime = 0.0f;
			bcanbecontrolled = FALSE;
		}
		/*f32 getActionDuration(){
			return actionduration;
		}
		f32 getActionTime(){
			return actiontime;
		}*/
		void falldown(f32 duration){
			bfallingdown = TRUE;
			fallingdownduration = duration;
			fallingdowntime = 0.0f;
		}
		void getup(f32 duration){
			bgettingup = TRUE;
			gettingupduration = duration;
			gettinguptime = 0.0f;
		}
		void updateTime(f32 deltatime);
		u8 isControllable(){
			return bcanbecontrolled;
		}
		u8 isInvincible(){
			return !bcanbehurt;
		}

		// God mode (Viewer)
		void activateGodMode();
		u8 isGodModeOn(){
			return bGodMode;	
		}

		// BEGIN Player 'inherited' methods
		void setEnt(hENTITY ent);
		hENTITY GetEnt(){ // capital G for 'legacy' purposes :P
			return m_ent;
		}
		void Rotate(f32 amt);
		void ClearAnimations();

		void Move(TPMove move);

		void MoveV(f32 amount); // for God mode only

		void determineShootAction();
		void PerformAction(TPAction a);

		void UpdateView(f32 mX, f32 mY);

		class Team * GetTeam (void);

		// END

		// determine if ball can be hit according to distance and valid hit angle
		u8 canHitBall();
	private:
		u8 type;
		tCHAR name[50], file[100];

		// hit ball parameters
		f32 hitAngle, hitDistance2; // max. angle at which ball can be hit (from player's view)
									// and hit distance squared at which ball can be hit

		hENTITY m_ent;

		std::vector <std::wstring> hudTexturesFiles; // filename of textures used on HUD

		u8 bGodMode;

		TPHudState hudstate;

		// attributes
		u16 maxhealth, maxmagic,
			currhealth, currmagic;

		// timings
		// action animation
		f32 actionduration, actiontime;
		u8 bcanbecontrolled;
		// botch
		f32 botchduration, botchtime;
		u8 bcanbehurt;
		// koed related
		f32 fallingdownduration, fallingdowntime,
			gettingupduration, gettinguptime;
		u8 bfallingdown, bkoed, bgettingup;
		// recovery
		f32 hprecoveryfrac, hprecoveryratepersec,
			mprecoveryfrac, mprecoveryratepersec;
		f32 hppercentforrecovery;
		u8 bcanrecoverhp, bcanrecovermp;

		// hits

		void hitBall();
};

void luabind_TeamPlayer (void);

#endif
