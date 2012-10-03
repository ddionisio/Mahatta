#ifndef _GAME_BALL_H
#define _GAME_BALL_H

#include "game_local.h"

struct ballInit {
	s32 mSndBounce;	///<
	s32 mSndExplode;///<
};

class Ball{
	public:
		/// @brief Part of a forward-looking ball path
		struct PathItem {
			Vec3D mPosition;///< Ball position at given time
			Vec3D mVelocity;///< Ball velocity at given time
			float mTime;///< Time, in seconds, along path
			bool mTeam1;///< If true, ball is (at least partially) on team 1's side
			bool mTeam2;///< If true, ball is (at least partially) on team 2's side

			PathItem (Vec3D pos, Vec3D vel, float time, bool bTeam1 = false, bool bTeam2 = false) : mPosition(pos), mVelocity(vel), mTime(time)
			{
				Mark(bTeam1, bTeam2);
			}

			void Mark (bool bTeam1, bool bTeam2) { mTeam1 = bTeam1, mTeam2 = bTeam2; }
		};

		// singleton thing, call this to get the object then call its methods
		static Ball* getMe();
		~Ball();

		void reset();		// reinitialize ball's paremeters

		Vec3D* getPos(){
			return &ball->m_loc;
		}
		void setPos(Vec3D pos);

		Vec3D GetVel (void);

		hENTITY getEnt(){
			return ball;
		}

		void hit(Vec3D impulse);

		void Scout (float time);

		std::list<PathItem> const & GetPath (void) { return mPath; }

		friend void LuaBall_A (void * pData);

	protected:	
		// declared here to avoid instancing
		Ball();
		Ball& operator= (const Ball&);

	private:
		hENTITY		ball;

		std::list<PathItem> mPath;	///< Forward-looking ball path
};

hENTITY BallCreate(const tCHAR *mdlStr, const ballInit & bi);

void OnCourtsideHit (hENTITY scout, int team);

void luabind_Ball (void);

#endif