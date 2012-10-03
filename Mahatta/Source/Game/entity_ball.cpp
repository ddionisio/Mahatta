#include "game_ball.h"
#include "game_local.h"
#include "trajectory.h"
#include "game_teamplayer.h"
#include <cfloat>

/// @brief Structure used to represent ball entity data
struct ballDat {
	Trajectory mTrajectory;	///< Curve trajectory
	Quaternion mOQ;	///< Orientation quaternion
	Vec3D mAV;	///< Angular velocity
	hENTITY mScout;	///< Scout to send ahead as path builder
	f32 mCurveT;///< Time lapsed since trajectory began
	s32 mSndBounce;	///< ID of sound played on bounce
    s32 mSndExplode;///< ID of sound played on explosion

	ballDat (ballInit const & bi) : mCurveT(0.0f), mSndBounce(bi.mSndBounce), mSndExplode(bi.mSndExplode), mAV(0.0f, 0.0f, 0.0f), mOQ(0.0f, 0.0f, 0.0f, 1.0f) {}
};

/// @brief Structure used to represent ball scout entity data
struct ballSD {
	std::list<Ball::PathItem> & mPath;	///< Path to build up on scouting run
	Trajectory mTrajectory;	///< Curve trajectory
	hENTITY mSelf;	///< Ball scout entity
	f32 mCur;	///< Current scouting time
	f32 mCurveT;///< Time lapsed since trajectory began
	f32 mDelta;	///< Time lapse on current scouting pass
	f32 mTime;	///< Time to scout over
	bool mAdd;	///< If true, addition was made on current pass
	bool mTeam1;///< If true, ball is (at least partially) on team 1's side on current pass
	bool mTeam2;///< If true, ball is (at least partially) on team 2's side on current pass

	ballSD (std::list<Ball::PathItem> & path, ballDat const & bDat, hENTITY ent) : mPath(path), mSelf(ent), mCurveT(0.0f) {}

	void Scout (hQBSP qbsp, float time);
};

void Ball::setPos (Vec3D pos)
{
	ball->m_loc = pos;
	ball->m_vel = Vec3D(0.0f, 0.0f, 0.0f);

	ballDat * pDat = static_cast<ballDat*>(ball->m_usrDat);

	pDat->mCurveT = 0.0f;
	pDat->mAV = Vec3D(0.0f, 0.0f, 0.0f);
	pDat->mOQ = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	pDat->mTrajectory.Build(pos, Vec3D(0.0f, 0.0f, 0.0f));
}

Vec3D Ball::GetVel (void)
{
	ballDat * pDat = static_cast<ballDat*>(ball->m_usrDat);

	return pDat->mTrajectory.GetVelocity(pDat->mCurveT);
}

/// @brief Height minimization enumeration
/// @param ent Entity used to test bounds
/// @param curh Current minimum height
static void Nadir (hENTITY ent, void * curh)
{
	if (ent->GetType() != ENTITY_TP) return;

	float * height = static_cast<float*>(curh);

	if (ent->m_loc.y + ent->m_boundMin.y < *height) *height = ent->m_loc.y + ent->m_boundMin.y;
}

void Ball::hit (Vec3D motion)
{
	// Begin a new path.
	mPath.clear();

	//
	ballDat * pDat = static_cast<ballDat*>(ball->m_usrDat);

	pDat->mCurveT = 0.0f;

	// Compute the curve trajectory.
	pDat->mTrajectory.Build(ball->m_loc, motion);

	ball->m_vel = pDat->mTrajectory.GetPosition(g_game.mPhysicsStep) - ball->m_loc;

	// Find the ground level, and determine how much time will elapse before the ball hits it,
	// with a little padding. Build a path of collisions over this timespan.
	float height = FLT_MAX, times[2];

	g_gi.WorldEnumEntities(Nadir, &height);

	u8 count = pDat->mTrajectory.GetYTimes(ball->m_loc.y + ball->m_boundRadius - height, times);

	float t = count > 0 ? times[count - 1] + g_game.mPhysicsStep : 0.0f;

	Scout(t > 3.0f ? t : 3.0f);
}

/// @brief Scouts ahead a given period of time to build a path
/// @param time Total time to scout ahead
void Ball::Scout (float time)
{
	ballDat * bDat = static_cast<ballDat*>(ball->m_usrDat);

	ballSD sDat(mPath, *bDat, bDat->mScout);
	
	bDat->mScout->m_usrDat = &sDat;

	sDat.mTrajectory = bDat->mTrajectory;

	// Place and run the scout.
	bDat->mScout->m_loc = ball->m_loc;

	sDat.Scout(g_gi.WorldGetMap(), time);
}

/// @brief Adds courtside entities to a list
/// @param ent Entity to check
/// @param el Entity list
void AddCS (hENTITY ent, void * el)
{
	std::list<hENTITY> * pEL = static_cast<std::list<hENTITY>*>(el);

	if (ent->GetType() == ENTITY_COURTSIDE) pEL->push_back(ent);
}

/// @brief Scouts ahead and builds up a path
/// @param qbsp Map to scout
/// @param time Time to scout over
void ballSD::Scout (hQBSP qbsp, float time)
{
	// Compile a list of the scout and any interacting entities.
	std::list<hENTITY> ents;

	ents.push_back(mSelf);

	g_gi.WorldEnumEntities(AddCS, &ents);

	// Add the start point to the path. Simulate ball collisions via the scout. Clear the
	// team flags on each pass to detect out-of-bounds and side switches. Quit early if the
	// ball gets stuck. (Things are ordered a bit strangely to get the initial ball side.)
	mCur = 0.0f;
	mTime = time;

	Vec3D loc(mSelf->m_loc), vel = mTrajectory.GetVelocity(0.0f);

	do {
		mAdd = mTeam1 = mTeam2 = false;
		mDelta = 1.0f;

		g_gi.WorldPreviewEx(qbsp, ents);

		if (mCur < 1e-6f) mPath.push_front(Ball::PathItem(loc, vel, 0.0f, mTeam1, mTeam2));

		if (mAdd)
		{
			mPath.back().mTeam1 = mTeam1;
			mPath.back().mTeam2 = mTeam2;
		}
	} while ((mCur += mDelta * g_game.mPhysicsStep) < mTime && mDelta * g_game.mPhysicsStep > 1e-6f);

	// Add the final point to the path, correcting for excess simulator time.
	mTrajectory.GetPosVel(mCurveT - (1.0f - mDelta) * g_game.mPhysicsStep, loc, vel);

	mPath.push_back(Ball::PathItem(loc, vel, mCur - mDelta * g_game.mPhysicsStep, mTeam1, mTeam2));
}

/// @brief Initializes the ball entity
/// @param ent Entity to process
/// @param initDat Entity initialization data
/// @return A boolean indicating success
static s32 Ball_Init (hENTITY ent, void * initDat)
{
	try {
		ent->m_usrDat = new ballDat(*static_cast<ballInit*>(initDat));
	} catch (std::bad_alloc &) {
		g_gi.LogMsg(LOG_FILE, L"Ball_Init", L"Unable to allocate data");
		
		return FALSE;
	}

	ent->m_flag = ENTITY_FLAG_NOGRAVITY;

	return TRUE;
}

/// @brief Updates the ball entity
/// @param ent Entity to process
/// @return A boolean indicating success
static s32 Ball_Update (hENTITY ent)
{
	ballDat * pDat = static_cast<ballDat*>(ent->m_usrDat);

	pDat->mCurveT += g_game.mPhysicsStep;

	ent->m_vel = pDat->mTrajectory.GetPosition(pDat->mCurveT) - ent->m_loc;

	// Rotate ball and apply drag.
	float av2 = Vec3DLenSQ(&pDat->mAV);

	if (av2 > 1e-3f)
	{
		float len = sqrtf(av2);

		Quaternion rotate;
		
		g_gi.QuatRotAxis(&(pDat->mAV / len), len * g_game.mPhysicsStep, &rotate);
		g_gi.QuatNorm(&(rotate * pDat->mOQ), &pDat->mOQ);
	}

//	pDat->mAV *= 0.95f;	// TODO: Read rotational drag constant

	Matrix om;	g_gi.MtxQuaternion(&pDat->mOQ, &om);

	ent->m_obj->SetAxisMtx(&om);

	return TRUE;
}

/// @brief Destroys the ball entity
/// @param ent Entity to process
/// @return A boolean indicating success
static s32 Ball_Destroy (hENTITY ent)
{
	ballDat * pDat = static_cast<ballDat*>(ent->m_usrDat);

	if (pDat->mScout != 0) pDat->mScout->Release();

	delete pDat;

	return TRUE;
}

/// @brief Responds to a ball collision with the world
/// @param ent Entity to process
/// @param qbsp The world's BSP data
/// @param pTrace A trace of the collision
/// @return A boolean indicating success
static s32 Ball_WorldCollide (hENTITY ent, hQBSP qbsp, gfxTrace * pTrace)
{
	ballDat * pDat = static_cast<ballDat*>(ent->m_usrDat);

	// Apply dampening. Bounce off the surface.
	Vec3D motion = pDat->mTrajectory.GetVelocity(pDat->mCurveT - (1.0f - pTrace->t) * g_game.mPhysicsStep);

	Vec3D vN = -Vec3DDot(&motion, &pTrace->norm) * pTrace->norm, vT = motion + vN;
float kK = 0.05f;	// TODO: Read kinetic friction constant (pTrace->material...)
float kR = 0.9f;	// TODO: Read restitution constant (ditto)

	// Apply torque to ball, using as force the friction acting at the point of contact.
	// (All mass terms cancel out.)
	Vec3D radius = -pTrace->norm * ent->m_boundRadius, brake = -kK * vT, torque;

	Vec3DCross(&brake, &radius, &torque);

	pDat->mAV += 2.5f * torque / (ent->m_boundRadius * ent->m_boundRadius);
// SCRIPT: onWorldHit?
	// Cue a sound if the ball bounced hard enough.
	// if (something > some amount)
	g_gi.SoundPlay(pDat->mSndBounce, ent->m_loc);

	// Trigger any surface effects.
	// ...
// SCRIPT
	// Do the hit.
	Ball::getMe()->hit(vT + brake + kR * vN);

	return TRUE;
}

/// @brief Responds to a ball collision with a second entity
/// @param ent Entity to process
/// @param otherEnt Second entity to process
/// @param pTrace A trace of the collision
/// @return A boolean indicating success
static s32 Ball_EntityCollide (hENTITY ent, hENTITY otherEnt, gfxTrace * pTrace)
{
	ballDat * pDat = static_cast<ballDat*>(ent->m_usrDat);
	
	if (0 == otherEnt) return FALSE;

	// Track the last collision for path building.
	if (otherEnt->GetType() == ENTITY_SCOREAREA ||
		otherEnt->GetType() == ENTITY_RINGAREA)
		return TRUE;				

	// check to see if ball is hitting a teamplayer
	if(otherEnt->GetType() == ENTITY_TP){

		ballDat * ballpDat = static_cast<ballDat*>(ent->m_usrDat);
		TPDat * TPpDat = static_cast<TPDat*>(otherEnt->m_usrDat);

		// check to see if teamplayer is 'invincible'
		if(TPpDat->mPlayer->isInvincible())
			return FALSE;

		/*/ TODO: what happens to the ball's trajectory?
		Vec3D motion;
			
		pDat->mTrajectory.GetVelocity(pDat->mCurveT - (1.0f - pTrace->t) * g_game.mPhysicsStep, motion);

		motion *= 0.6f;
		motion -= 2.0f * Vec3DDot(&motion, &TPpDat->m_dirCur) * TPpDat->m_dirCur;

		Ball::getMe()->hit(motion);//*/

		// drain teamplayer's health
		TPpDat->mPlayer->drainHealth(10, &ent->m_loc);

		return TRUE;
	}	

	return FALSE;
}

/// @brief Determines whether a ball may collide with a second entity
/// @param ent Entity to process
/// @param otherEnt Second entity to process
/// @return A boolean indicating success
static s32 Ball_AllowCollide (hENTITY ent, hENTITY otherEnt)
{
	return otherEnt->GetType() != ENTITY_TP;
}

/// @brief Initializes the ball scout entity
/// @param ent Entity to process
/// @param initDat Entity initialization data
/// @return A boolean indicating success
static s32 Ball_Scout_Init (hENTITY ent, void * initDat)
{
	ent->m_flag = ENTITY_FLAG_NOGRAVITY;

	return TRUE;
}

/// @brief Updates the ball scout entity
/// @param ent Entity to process
/// @return A boolean indicating success
static s32 Ball_Scout_Update (hENTITY ent)
{
	ballSD * pDat = static_cast<ballSD*>(ent->m_usrDat);

	pDat->mCurveT += g_game.mPhysicsStep;

	ent->m_vel = pDat->mTrajectory.GetPosition(pDat->mCurveT) - ent->m_loc;

	return TRUE;
}

/// @brief Responds to a ball scout collision with the world
/// @param ent Entity to process
/// @param qbsp The world's BSP data
/// @param pTrace A trace of the collision
/// @return A boolean indicating success
static s32 Ball_Scout_WorldCollide (hENTITY ent, hQBSP qbsp, gfxTrace * pTrace)
{
	ballSD * pDat = static_cast<ballSD*>(ent->m_usrDat);

	// Apply dampening. Bounce off the surface.
	Vec3D motion = pDat->mTrajectory.GetVelocity(pDat->mCurveT - (1.0f - pTrace->t) * g_game.mPhysicsStep);

	Vec3D vN = -Vec3DDot(&motion, &pTrace->norm) * pTrace->norm, vT = motion + vN;
float kK = 0.05f;	// TODO: Read kinetic friction constant
float kR = 0.9f;	// TODO: Read restitution constant

	pDat->mCurveT = 0.0f;
	pDat->mTrajectory.Build(ent->m_loc, (1.0f - kK) * vT + kR * vN);

	if (pDat->mCur + pTrace->t * g_game.mPhysicsStep <= pDat->mTime)
	{
		pDat->mAdd = true;
		pDat->mDelta = pTrace->t;

		pDat->mPath.push_back(Ball::PathItem(ent->m_loc, pDat->mTrajectory.GetVelocity(0.0f), pDat->mCur + pTrace->t * g_game.mPhysicsStep));
	}

	ent->m_vel = pDat->mTrajectory.GetPosition(g_game.mPhysicsStep) - ent->m_loc;

	return TRUE;
}

/// @brief Generates a ball entity
/// @param mdlStr Name of ball model
/// @param bi Ball initialization data
/// @return The created ball entity
hENTITY BallCreate (tCHAR const * mdlStr, ballInit const & bi)
{
	EntityVFTable vftbl = { 0 }, svftbl = { 0 };
	
	vftbl.Init = Ball_Init;
	vftbl.Update = Ball_Update;
	vftbl.Destroy = Ball_Destroy;
	vftbl.WorldCollide = Ball_WorldCollide;
	vftbl.EntityCollide = Ball_EntityCollide;
	vftbl.AllowCollide = Ball_AllowCollide;

	// Instantiate and perform setup on the ball entity.
	hENTITY ent = g_gi.EntCreate(L"ball", ENTITY_BALL, mdlStr, vftbl, (void*)&bi);

	ent->m_obj->GetAABBWorld(&ent->m_boundMin, &ent->m_boundMax);

	Vec3D diff = ent->m_boundMax - ent->m_boundMin;

	for (int index = 0; index < 3; ++index) ent->m_boundRadius = diff[index] > ent->m_boundRadius ? diff[index] : ent->m_boundRadius;

	// Bind a scout to the ball.
	svftbl.Init = Ball_Scout_Init;
	svftbl.Update = Ball_Scout_Update;
	svftbl.WorldCollide = Ball_Scout_WorldCollide;

	ballDat * pDat = static_cast<ballDat*>(ent->m_usrDat);

	pDat->mScout = g_gi.EntCreate(L"ballscout", ENTITY_BALL_SCOUT, 0, svftbl, 0);

	// Replicate ball parameters for scout.
	pDat->mScout->m_boundMin = ent->m_boundMin;
	pDat->mScout->m_boundMax = ent->m_boundMax;
	pDat->mScout->m_boundRadius = ent->m_boundRadius;

	return ent;
}

/// @brief Responds to scout/courtside collisions
/// @param scout Scout entity
/// @param team Team to flag as containing ball
void OnCourtsideHit (hENTITY scout, int team)
{
	switch (team)
	{
		case 1:
			static_cast<ballSD*>(scout->m_usrDat)->mTeam1 = true;
			break;
		case 2:
			static_cast<ballSD*>(scout->m_usrDat)->mTeam2 = true;
			break;
	}
}
