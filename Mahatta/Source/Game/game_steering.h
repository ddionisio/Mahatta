#ifndef GAME_STEERING_H
#define GAME_STEERING_H

#include "game_share.h"

#include <bitset>
#include <list>
#include <string>
#include <vector>

/// @brief Steering information for entities
class Steering {
public:
	// Enumerations
	enum Force {
		eAlignment,	///< Force used to keep related entities lined up
		eCohesion,	///< Force used to keep related entities clustered
		eSeparation,///< Force used to maintain distance between related entities
		eTarget,///< Force used to direct entities toward or away from a target
		eWall,	///< Force used for wall-entity interactions
		eWander,///< Force used to send entity wandering
			eNForces	///< Force type count
	};
	enum Property {
		eAlignmentScale,///< Alignment force scale factor
		eCohesionScale,	///< Cohesion force scale factor
		eSeparationScale,	///< Separation force scale factor
		eTargetApproach,///< Target approach distance
		eTargetScale,	///< Target force scale factor
		eTargetSpeed,	///< Targeting speed
		eWallScale,	///< Wall force scale factor
		eWanderAngle,	///< Maximum veer angle
		eWanderDistance,///< Distance along wander direction
		eWanderRadius,	///< Radius of wander sphere
		eWanderScale,	///< Wander force scale factor
			eNProperties	///< Property type count
	};

	// Typedefs
	typedef float (*RandomFunc)(void);	///< Random number function
private:
	// Members
	std::bitset<sizeof(int) * 8> mStatus;	///< Current status of steering unit
	std::list<Force> mOrder;///< Ordered force priority list
	std::vector<Vec3D> mForces;	///< Steering forces accumulated over a pass
	std::vector<float> mProperties;	///< Properties used to modulate forces
	Vec3D mTarget;	///< Target position
	RandomFunc mRandomFunc;	///< Current random function

	// Implementation
	Vec3D GetFinalForce (void);

	void AddForce (Force which, Vec3D const & force);
	void GetGroupForces (hENTITY ent, std::list<hENTITY> & entities);
	void GetTargetForce (hENTITY ent);
	void GetWanderForce (hENTITY ent);
public:
	// Lifetime
	Steering (void);
	~Steering (void);

	// Methods
	std::list<Force> GetOrder (void);

	Vec3D GetForce (hENTITY ent, std::list<hENTITY> & entities, hQBSP qbsp);

	float GetProperty (Property property);

	void EnableForce (Force force, bool bEnable);
	void SetOrder (std::list<Force> const & order);
	void SetProperty (Property property, float value);
	void SetTarget (Vec3D const & target);
	void SetRandomFunc (RandomFunc randomFunc);
};

void luabind_Steering (void);

#endif	// GAME_STEERING_H