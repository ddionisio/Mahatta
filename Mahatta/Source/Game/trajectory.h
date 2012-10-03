#ifndef _trajectory_h
#define _trajectory_h

#include "game_share.h"

u8 TrajectGetAngles(f32 x, f32 y, f32 speed, f32 gravity, f32 angleOut[2]);
u8 TrajectGetSpeed(f32 x, f32 y, f32 angle, f32 gravity, f32 & speedOut);

/// @brief Trajectory structure
class Trajectory {
private:
	Vec3D mGravity;	///< Current gravity vector
	Vec3D mLaunch;	///< Launch velocity
	Vec3D mStart;	///< Start of trajectory curve in space
public:
	Trajectory (void) {}
	Trajectory (Vec3D const & start, Vec3D const & launch);

	void Build (Vec3D const & start, Vec3D const & launch);
	void GetMaxHeight (f32 & maxH);
	void GetPosVel (f32 t, Vec3D & pOut, Vec3D & vOut);

	Vec3D GetPosition (f32 t);
	Vec3D GetVelocity (f32 t);

	bool GetAverageHeight (f32 dx, f32 & avgH);
	bool GetXTime (f32 x, f32 & tOut);

	u8 GetYTimes (f32 y, f32 tOut[2]);
};

void luabind_Trajectory (void);

#endif