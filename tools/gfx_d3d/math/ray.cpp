#include "common.h"
#include "GFX\gfx_math.h"

//ray stuff

#define EPSILON 1.0e-10 //0.03125f
#define CLOSE_TOLERANCE 1.0e-6
#define DEPTH_TOLERANCE 1.0e-6

//normals for the box
f32	g_boxNorm[6][3] =
{{-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1}};

/////////////////////////////////////
// Name:	GFXIntersectBox
// Purpose:	not really related to
//			anything Graphics but nice
//			to have.  Intersect given
//			ray to axis aligned bound box.
// Output:	tOut and nOut filled
// Return:	scalar t, if 1, then no intersect
/////////////////////////////////////
f32 F_API RayIntersectBox(const Vec3D *p, const Vec3D *dir,
					const Vec3D *boxMin, const Vec3D *boxMax,
					Vec3D *nOut)
{
	f32 t, tmin, tmax;
	s32 minSide=0, maxSide=0;

	tmin = 0.0;
	//tmax = 100000000;
	tmax = 1;

	/*
	* Sides first.
	*/

	if (dir->x < -EPSILON)
	{
		t = (boxMin->x - p->x) / dir->x;

		if (t < tmin) return 1.0f;

		if (t <= tmax)
		{
			maxSide = 0;
			tmax = t;
		}

		t = (boxMax->x - p->x) / dir->x;

		if (t >= tmin)
		{
			if (t > tmax) return 1.0f;

			minSide = 1;
			tmin = t;
		}
	}
	else
	{
		if (dir->x > EPSILON)
		{
			t = (boxMax->x - p->x) / dir->x;

			if (t < tmin) return 1.0f;

			if (t <= tmax)
			{
				maxSide = 1;
				tmax = t;
			}

			t = (boxMin->x - p->x) / dir->x;

			if (t >= tmin)
			{
				if (t > tmax) return 1.0f;

				minSide = 0;
				tmin = t;
			}
		}
		else
		{
			if ((p->x < boxMin->x) || (p->x > boxMax->x))
			{
				return 1.0f;
			}
		}
	}

	/*
	* Check Top/Bottom.
	*/

	if (dir->y < -EPSILON)
	{
		t = (boxMin->y - p->y) / dir->y;

		if (t < tmin) return 1.0f;

		if (t <= tmax - CLOSE_TOLERANCE)
		{
			maxSide = 2;
			tmax = t;
		}

		t = (boxMax->y - p->y) / dir->y;

		if (t >= tmin + CLOSE_TOLERANCE)
		{
			if (t > tmax) return 1.0f;

			minSide = 3;
			tmin = t;
		}
	}
	else
	{
		if (dir->y > EPSILON)
		{
			t = (boxMax->y - p->y) / dir->y;

			if (t < tmin) return 1.0f;

			if (t <= tmax - CLOSE_TOLERANCE)
			{
				maxSide = 3;
				tmax = t;
			}

			t = (boxMin->y - p->y) / dir->y;

			if (t >= tmin + CLOSE_TOLERANCE)
			{
				if (t > tmax) return 1.0f;

				minSide = 2;
				tmin = t;
			}
		}
		else
		{
			if ((p->y < boxMin->y) || (p->y > boxMax->y))
			{
				return 1.0f;
			}
		}
	}

	/* Now front/back */

	if (dir->z < -EPSILON)
	{
		t = (boxMin->z - p->z) / dir->z;

		if (t < tmin) return 1.0f;

		if (t <= tmax - CLOSE_TOLERANCE)
		{
			maxSide = 4;
			tmax = t;
		}

		t = (boxMax->z - p->z) / dir->z;

		if (t >= tmin + CLOSE_TOLERANCE)
		{
			if (t > tmax) return 1.0f;

			minSide = 5;
			tmin = t;
		}
	}
	else
	{
		if (dir->z > EPSILON)
		{
			t = (boxMax->z - p->z) / dir->z;

			if (t < tmin) return 1.0f;

			if (t <= tmax - CLOSE_TOLERANCE)
			{
				maxSide = 5;
				tmax = t;
			}

			t = (boxMin->z - p->z) / dir->z;

			if (t >= tmin + CLOSE_TOLERANCE)
			{
				if (t > tmax) return 1.0f;

				minSide = 4;
				tmin = t;
			}
		}
		else
		{
			if ((p->z < boxMin->z) || (p->z > boxMax->z))
			{
				return 1.0f;
			}
		}
	}

	if (tmax < DEPTH_TOLERANCE)
	{
		return 1.0f;
	}

	//if(minSide>= 0)
	{

		if(nOut)
			memcpy(nOut, g_boxNorm[minSide], sizeof(f32)*3);
	}
	//else
	//{
	//	*tOut = tmax;

		//if(nOut)
			//memcpy(nOut, g_boxNorm[maxSide], sizeof(float)*eMaxPt);
	//}


	return tmin;
}