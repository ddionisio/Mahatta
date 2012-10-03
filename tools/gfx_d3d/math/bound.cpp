#include "common.h"
#include "GFX\gfx_math.h"

u8 F_API BoundsIntersect(const Vec3D & mins1, const Vec3D & maxs1, 
						const Vec3D & mins2, const Vec3D & maxs2)
{
	return (mins1.x <= maxs2.x && mins1.y <= maxs2.y && mins1.z <= maxs2.z &&
		 maxs1.x >= mins2.x && maxs1.y >= mins2.y && maxs1.z >= mins2.z);
}

void F_API BoundsClear(Vec3D & mins, Vec3D & maxs)
{
	mins.x = mins.y = mins.z = 99999;
	maxs.x = maxs.y = maxs.z = -99999;
}

void F_API BoundsAddPoint(const Vec3D & v, Vec3D & mins, Vec3D & maxs)
{
	if(v.x < mins.x)
		mins.x = v.x;
	if(v.x > maxs.x)
		maxs.x = v.x;

	if(v.y < mins.y)
		mins.y = v.y;
	if(v.y > maxs.y)
		maxs.y = v.y;

	if(v.z < mins.z)
		mins.z = v.z;
	if(v.z > maxs.z)
		maxs.z = v.z;
}