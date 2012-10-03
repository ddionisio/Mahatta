#include "engine_common.h"

#include "entity.h"

#include "world.h"

///////////////////////////////
//Bound Stuff
//

#define BOUND_DIST_EPSILON  0.03125f
#define MAX_PLANE_SIDE		6

Plane	g_boxPlanes[MAX_PLANE_SIDE];	//the planes for the bounds

//just initialize the global box convex hull
void IEntity::sBoundInit()
{
	memset(g_boxPlanes, 0, sizeof(g_boxPlanes));

	g_boxPlanes[0].c = -1;
	g_boxPlanes[1].c =  1;
	g_boxPlanes[2].a = -1;
	g_boxPlanes[3].a =  1;
	g_boxPlanes[4].b =  1;
	g_boxPlanes[5].b = -1;
}

static inline void _CollBoxPlane(const Vec3D & srcMins, const Vec3D & srcMaxs,
						  const Vec3D & srcPt,
						  const Vec3D & mins, const Vec3D & maxs, 
						  const Vec3D & pt, const Vec3D & vel, 
						  gfxTrace *pTrace)
{
	s32 txtInd = -1;

	//go through each half-space and determine the interval t[min,max]
	f32 tMin=0, tMax=1;

	f32 vDotn, pDotn, tInt, dist;

	Vec3D ofs, srcOfs;

	s32 indMin=0, indMax=0;

	Plane *pPlane;

	//go through the sides
	for(s32 i = 0; i < MAX_PLANE_SIDE; i++)
	{
		if(tMin > tMax) //no intersect!
			break;

		pPlane = &g_boxPlanes[i];

		if(mins != maxs)
		{	// general box case
			// push the plane out apropriately for mins/maxs
			
			//we are assuming the planes are axis aligned
			if(pPlane->a < 0)
				dist = pPlane->a*maxs.x - ((srcPt.x+srcMins.x)*pPlane->a);
			else if(pPlane->a > 0)
				dist = pPlane->a*mins.x - ((srcPt.x+srcMaxs.x)*pPlane->a);
			else if(pPlane->b < 0)
				dist = pPlane->b*maxs.y - ((srcPt.y+srcMins.y)*pPlane->b);
			else if(pPlane->b > 0)
				dist = pPlane->b*mins.y - ((srcPt.y+srcMaxs.y)*pPlane->b);
			else if(pPlane->c < 0)
				dist = pPlane->c*maxs.z - ((srcPt.z+srcMins.z)*pPlane->c);
			else if(pPlane->c > 0)
				dist = pPlane->c*mins.z - ((srcPt.z+srcMaxs.z)*pPlane->c);
		}
		else
		{	// special point case
			if(pPlane->a < 0)
				dist = -((srcPt.x+srcMins.x)*pPlane->a);
			else if(pPlane->a > 0)
				dist = -((srcPt.x+srcMaxs.x)*pPlane->a);
			else if(pPlane->b < 0)
				dist = -((srcPt.y+srcMins.y)*pPlane->b);
			else if(pPlane->b > 0)
				dist = -((srcPt.y+srcMaxs.y)*pPlane->b);
			else if(pPlane->c < 0)
				dist = -((srcPt.z+srcMins.z)*pPlane->c);
			else if(pPlane->c > 0)
				dist = -((srcPt.z+srcMaxs.z)*pPlane->c);
		}

		//normal dot ray vector
		vDotn = pPlane->a*vel.x + pPlane->b*vel.y + pPlane->c*vel.z;

		pDotn = pPlane->a*pt.x + pPlane->b*pt.y + pPlane->c*pt.z;

		if(vDotn < 0)
		{
			tInt = -(pDotn + dist - BOUND_DIST_EPSILON)/vDotn;

			if (tInt > tMin) {
			  tMin = tInt;
			  indMin = i;
			}
			
		}
		else if(vDotn > 0)
		{
			tInt = -(pDotn + dist + BOUND_DIST_EPSILON)/vDotn;

			if(tInt < tMax)
			{
				tMax = tInt;
				indMax = i;
			}

		}
		else
		{
			if(pDotn + dist > 0)
				tMin = tMax+1;
		}
	}

	//now let's see if we intersect something...
	if(tMin < tMax && tMin < pTrace->t)
	{
		pTrace->t = tMin;

		pTrace->norm.x = g_boxPlanes[indMin].a;
		pTrace->norm.y = g_boxPlanes[indMin].b;
		pTrace->norm.z = g_boxPlanes[indMin].c;
		pTrace->d		 = g_boxPlanes[indMin].d;

		pTrace->txtInd   = -1;
		
		pTrace->bStartSolid = tMin < 0 ? TRUE : FALSE;
		pTrace->bAllSolid = (tMax < 0 && pTrace->bStartSolid) ? TRUE : FALSE;

		pTrace->brushIndex = -1;
	}
	else
	{
		if(tMin < 0)
		{
			pTrace->bStartSolid = TRUE;

			if(tMax < 0)
				pTrace->bAllSolid = TRUE;

			pTrace->brushIndex = -1;
		}
	}
}

//collide ent2 to us
//testT is the 't' comparison for testing if we already
//hit a minimum value.
//NOTE: you can set qbsp to 0 to force box collision
u8 IEntity::BoundCollide(hENTITY ent2, hQBSP qbsp, gfxTrace *pTrace, f32 testT)
{
	Vec3D minOfs = m_loc + m_boundMin + ent2->m_boundMin;
	Vec3D maxOfs = m_loc + m_boundMax + ent2->m_boundMax;

	if(m_mapMdlID != -1 && qbsp)
	{
		qbsp->ModelCollision(m_mapMdlID, 
			&ent2->m_boundMin, &ent2->m_boundMax,
			ent2->m_loc, ent2->m_loc+ent2->m_vel, 
			pTrace, testT);
	}
	else// if(GFXIntersectBox((float*)pt, (float*)vel, (float*)minOfs, (float*)maxOfs, &t, norm))//&& t < 1)
	{
		_CollBoxPlane(m_boundMin, m_boundMax, m_loc,
			ent2->m_boundMin, ent2->m_boundMax, ent2->m_loc, ent2->m_vel, 
			pTrace);
	}
	/*
	I don't think this should ever happen anyway...
	else if(BoundBoxCheck(thisPt, otherBound, pt))
	{
		pTrace->txtInd = -1;
		pTrace->brushIndex = -1;
		pTrace->leafIndex = -1;
		pTrace->d = 0;
		pTrace->t = 0;
		memset(pTrace->norm, 0, sizeof(pTrace->norm));

		pTrace->bStartSolid = true;

		return true;
	}*/

	if(pTrace->t < testT)
		return TRUE;

	return FALSE;
}

//this will only check if sphere intersects
//both entities
u8 IEntity::BoundSphereCheck(hENTITY ent2)
{
	Vec3D d;
	f32 R = m_boundRadius + ent2->m_boundRadius;

	d = m_loc - ent2->m_loc;

	f32 lSqr = Vec3DDot(&d, &d);

	if(lSqr <= R*R )
		return TRUE;

	return FALSE;
}

//AABB to AABB collide check
u8 IEntity::BoundBoxCheck(hENTITY ent2)
{
	//just check to see if pt. is in bound box
	Vec3D minOfs = m_loc + m_boundMin + ent2->m_boundMin;
	Vec3D maxOfs = m_loc + m_boundMax + ent2->m_boundMax;

	if(ent2->m_loc.x > minOfs.x && ent2->m_loc.x < maxOfs.x
		&& ent2->m_loc.y > minOfs.y && ent2->m_loc.y < maxOfs.y
		&& ent2->m_loc.z > minOfs.z && ent2->m_loc.z < maxOfs.z)
		return TRUE;

	return FALSE;
}