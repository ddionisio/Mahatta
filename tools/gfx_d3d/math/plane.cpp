#include "common.h"
#include "GFX\gfx_math.h"

void F_API PlaneFromPoints(const Vec3D *pt1, const Vec3D *pt2, const Vec3D *pt3, Plane *plOut)
{
	Vec3D v1((*pt2)-(*pt1));
	Vec3D v2((*pt3)-(*pt1));
	Vec3D norm; Vec3DCross(&v2, &v1, &norm);
	Vec3DNormal(&norm, &norm);

	PlaneFromPointNormal(&norm, pt1, plOut);
}

void F_API PlaneNormalize(const Plane *p, Plane *pOut)
{
	f32 len = sqrtf(p->a*p->a + p->b*p->b + p->c*p->c);
	pOut->a = p->a/len;
	pOut->b = p->b/len;
	pOut->c = p->c/len;
}

//mtx = transpose(inv(mtx))
void F_API PlaneTransform(const Matrix *mtx, const Plane *p, Plane *pOut)
{
	pOut->a = mtx->_11*p->a + mtx->_21*p->b + mtx->_31*p->c + mtx->_41*p->d;
	pOut->b = mtx->_12*p->a + mtx->_22*p->b + mtx->_32*p->c + mtx->_42*p->d;
	pOut->c = mtx->_13*p->a + mtx->_23*p->b + mtx->_33*p->c + mtx->_43*p->d;
	pOut->d = mtx->_14*p->a + mtx->_24*p->b + mtx->_34*p->c + mtx->_44*p->d;
}

void F_API ProjectPointOnPlane(const Vec3D *p, const Vec3D *norm, Vec3D *vOut)
{
	f32 d;
	Vec3D n;
	f32 inv_denom;

	inv_denom = 1.0f / Vec3DDot(norm,norm);

	d = Vec3DDot(norm,p) * inv_denom;

	n.x = norm->x * inv_denom;
	n.y = norm->y * inv_denom;
	n.z = norm->z * inv_denom;

	vOut->x = p->x - d * n.x;
	vOut->y = p->y - d * n.y;
	vOut->z = p->z - d * n.z;
}

//for easy side testing, you should use this
void F_API PlaneCategorize(Plane *p)
{
	p->signbits = 0;
	p->type = PLANE_NONAXIAL;

	if (p->a == 1.0f)
		p->type = 0;
	if(p->a < 0)
		p->signbits |= 1;

	if (p->b == 1.0f)
		p->type = 1;
	if(p->b < 0)
		p->signbits |= 2;

	if (p->c == 1.0f)
		p->type = 2;
	if(p->c < 0)
		p->signbits |= 4;
}

/*
=================
PlaneTypeForNormal
=================
*/
s16	F_API PlaneTypeForNormal(const Plane *p)
{
// NOTE: should these have an epsilon around 1.0?		
	if ( p->a >= 1.0)
		return PLANE_X;
	if ( p->b >= 1.0 )
		return PLANE_Y;
	if ( p->c >= 1.0 )
		return PLANE_Z;
		
	return PLANE_NONAXIAL;
}

/*
=================
SignbitsForPlane
=================
*/
s16 F_API SignbitsForPlane(const Plane *p)
{
	s16	bits=0;

	// for fast box on planeside test
	if(p->a < 0)
		bits |= 1;
	if(p->b < 0)
		bits |= 2;
	if(p->c < 0)
		bits |= 4;

	return bits;
}

/*
==================
BoxOnPlaneSide
Assumes you categorized
the plane (PlaneCategorize)

Returns 1, 2, or 1 + 2
==================
*/
s32 F_API BoxOnPlaneSide (const Vec3D *mins, const Vec3D *maxs, const Plane *p)
{
	f32	dist1, dist2;
	s32	sides;

	const f32 *emins = (const f32*)(*mins);
	const f32 *emaxs = (const f32*)(*maxs);

// fast axial cases
	if (p->type < 3)
	{
		if (p->d <= emins[p->type])
			return 1;
		if (p->d >= emaxs[p->type])
			return 2;
		return 3;
	}
	
// general case
	switch (p->signbits)
	{
	case 0:
dist1 = p->a*emaxs[0] + p->b*emaxs[1] + p->c*emaxs[2];
dist2 = p->a*emins[0] + p->b*emins[1] + p->c*emins[2];
		break;
	case 1:
dist1 = p->a*emins[0] + p->b*emaxs[1] + p->c*emaxs[2];
dist2 = p->a*emaxs[0] + p->b*emins[1] + p->c*emins[2];
		break;
	case 2:
dist1 = p->a*emaxs[0] + p->b*emins[1] + p->c*emaxs[2];
dist2 = p->a*emins[0] + p->b*emaxs[1] + p->c*emins[2];
		break;
	case 3:
dist1 = p->a*emins[0] + p->b*emins[1] + p->c*emaxs[2];
dist2 = p->a*emaxs[0] + p->b*emaxs[1] + p->c*emins[2];
		break;
	case 4:
dist1 = p->a*emaxs[0] + p->b*emaxs[1] + p->c*emins[2];
dist2 = p->a*emins[0] + p->b*emins[1] + p->c*emaxs[2];
		break;
	case 5:
dist1 = p->a*emins[0] + p->b*emaxs[1] + p->c*emins[2];
dist2 = p->a*emaxs[0] + p->b*emins[1] + p->c*emaxs[2];
		break;
	case 6:
dist1 = p->a*emaxs[0] + p->b*emins[1] + p->c*emins[2];
dist2 = p->a*emins[0] + p->b*emaxs[1] + p->c*emaxs[2];
		break;
	case 7:
dist1 = p->a*emins[0] + p->b*emins[1] + p->c*emins[2];
dist2 = p->a*emaxs[0] + p->b*emaxs[1] + p->c*emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		assert( 0 );
		break;
	}

	sides = 0;
	if (dist1 >= p->d)
		sides = 1;
	if (dist2 < p->d)
		sides |= 2;

	assert( sides != 0 );

	return sides;
}