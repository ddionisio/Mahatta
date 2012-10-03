#ifndef _GAME_MATCH_H
#define _GAME_MATCH_H

#include "game_local.h"
#include "game_score.h"

#define SCORE_HITRING 1
#define SCORE_INTORING 3

typedef enum{
	// SINGLE
	MATCHTYPE_SINGLE_STORY,
	MATCHTYPE_SINGLE_QUICKGAME,

	// VS
	MATCHTYPE_VS_CREATEGAME,
	MATCHTYPE_VS_JOINGAME,

	MATCHTYPE_MAX
}MatchType;

typedef enum{
	MOV_FORWARD,
	MOV_RIGHT,
	MOV_BACKWARD,
	MOV_LEFT,

	MOV_MAX    
}MovementDir;

class Match{
	public:
		// singleton thing, call this to get the object then call its methods
		static Match* getMe();
		~Match();

		class Team* getTeam(u8 teamid);

		void update();
		void render();

		void setBallCollision(wstring entityname,	// entities call this
							  wstring side_id); 
		void score();								// begin score processing
		void checkBallPosition();					// see on what side of the
													//  court the ball is, if any

		bool InPlay (void);

		friend void LuaMatch_A (void * pData);

		void setMovementDir(MovementDir movDir);

/// DEBUG
void AddLine (Vec3D const & v1, Vec3D const & v2, u32 color)
{
	Line line;

	line.vs[0] = v1;
	line.vs[1] = v2;
	line.color = color;

	lines.push_back(line);
}
/// DEBUG
	protected:	
		// declared here to avoid instancing
		Match();
		Match& operator= (const Match&);

	private:
		int	mMatchProc;	///< Match coroutine

		f32			matchtimer;
		u8			matchminutes, matchseconds, matchcentiseconds;

		// match status flags
		u8			scoring;				// a score was made, so process whatever
		f32			scoringduration;		// duration of scoring process
		f32			scoringtime;			// time doing the scoring process

		// flags for input
		u8			bmoveleft, bmoveright,
					bmoveforward, bmovebackward;

		// collision flags for ball
		u8			collisionring1,
					collisionscore1,
					collisioncourt1;
		u8			collisionring2,
					collisionscore2,
					collisioncourt2;

		// score counters
		u8			scoreteam1,
					scoreteam2;

		tCHAR		message[255];				//string holding a message to display

/// DEBUG
struct Line {
	Vec3D vs[2];
	u32 color;
};

std::list<Line> lines;
/// DEBUG

		// methods
		void processInput();
		
};

void luabind_Match (void);

#endif
