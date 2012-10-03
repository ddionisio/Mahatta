#include "common.h"
#include "GFX\gfx_math.h"

//
// 2D
//

void F_API Vec2DCatMullRom(const Vec2D *v1, const Vec2D *v2, const Vec2D *v3, const Vec2D *v4, f32 s, Vec2D *vOut)
{
	/*
	v1 = p2
	v2 = p3
	t1 = (p3 - p1) / 2
	t2 = (p4 - p2) / 2
	*/
	Vec2D t1((v3->x-v1->x)*0.5f, (v3->y-v1->y)*0.5f);
	Vec2D t2((v4->x-v2->x)*0.5f, (v4->y-v2->y)*0.5f);

	Vec2DHermite(v2, &t1, v3, &t2, s, vOut);
}

void F_API Vec2DHermite(const Vec2D *v1, const Vec2D *t1, const Vec2D *v2, const Vec2D *t2, f32 s, Vec2D *vOut)
{
	f32 s2 = s*s, s3 = s2*s;

	f32 h1 =  2*s3 - 3*s2 + 1;          // calculate basis function 1
	f32 h2 = -2*s3 + 3*s2;              // calculate basis function 2
	f32 h3 =  s3   - 2*s2 + s;         // calculate basis function 3
	f32 h4 =  s3   - s2;              // calculate basis function 4

	vOut->x = h1*v1->x + h2*v2->x + h3*t1->x + h4*t2->x;
	vOut->y = h1*v1->y + h2*v2->y + h3*t1->y + h4*t2->y;
}

f32 F_API Vec2DNormal(const Vec2D *v, Vec2D *vOut)
{
	f32 len = Vec2DDot(v,v);

	if(len)
	{
		len = sqrtf(len);
		vOut->x = v->x/len;
		vOut->y = v->y/len;
	}

	return len;
}

void F_API Vec2DTransform(const Vec2D *v, const Matrix *mtx, Vec4D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y + mtx->_41;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y + mtx->_42;
	vOut->z = mtx->_13*v->x + mtx->_23*v->y + mtx->_43;
	vOut->w = mtx->_14*v->x + mtx->_24*v->y + mtx->_44;
}

void F_API Vec2DTransformCoord(const Vec2D *v, const Matrix *mtx, Vec2D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y + mtx->_41;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y + mtx->_42;
}

void F_API Vec2DTransformNormal(const Vec2D *v, const Matrix *mtx, Vec2D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y;
}

f32 F_API Vec2DGetAngle(const Vec2D *v)
{
	if(v->x == 0 && v->y == 0)
		return 0;

	f32 rot = v->y != 0 ? atanf ( v->x/v->y ) : v->x <= 0 ? (PI*0.5f) : (PI*1.5f);
	
	if ( v->y > 0 ) rot += 3.14f;

	return rot;
}

//
// 3D
//

void F_API Vec3DCatMullRom(const Vec3D *v1, const Vec3D *v2, const Vec3D *v3, const Vec3D *v4, f32 s, Vec3D *vOut)
{
	/*
	v1 = p2
	v2 = p3
	t1 = (p3 - p1) / 2
	t2 = (p4 - p2) / 2
	*/
	Vec3D t1((v3->x-v1->x)/2, (v3->y-v1->y)/2, (v3->z-v1->z)/2);
	Vec3D t2((v4->x-v2->x)/2, (v4->y-v2->y)/2, (v4->z-v2->z)/2);

	Vec3DHermite(v2, &t1, v3, &t2, s, vOut);
}

void F_API Vec3DHermite(const Vec3D *v1, const Vec3D *t1, const Vec3D *v2, const Vec3D *t2, f32 s, Vec3D *vOut)
{
	f32 s2 = s*s, s3 = s2*s;

	f32 h1 =  2*s3 - 3*s2 + 1;          // calculate basis function 1
	f32 h2 = -2*s3 + 3*s2;              // calculate basis function 2
	f32 h3 =  s3   - 2*s2 + s;         // calculate basis function 3
	f32 h4 =  s3   - s2;              // calculate basis function 4

	vOut->x = h1*v1->x + h2*v2->x + h3*t1->x + h4*t2->x;
	vOut->y = h1*v1->y + h2*v2->y + h3*t1->y + h4*t2->y;
	vOut->z = h1*v1->z + h2*v2->z + h3*t1->z + h4*t2->z;
}

f32 F_API Vec3DNormal(const Vec3D *v, Vec3D *vOut)
{
	f32 len = Vec3DDot(v,v);

	if(len)
	{
		len = sqrtf(len);
		vOut->x = v->x/len;
		vOut->y = v->y/len;
		vOut->z = v->z/len;
	}

	return len;
}

void F_API Vec3DTransform(const Vec3D *v, const Matrix *mtx, Vec4D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y + mtx->_31*v->z + mtx->_41;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y + mtx->_32*v->z + mtx->_42;
	vOut->z = mtx->_13*v->x + mtx->_23*v->y + mtx->_33*v->z + mtx->_43;
	vOut->w = mtx->_14*v->x + mtx->_24*v->y + mtx->_34*v->z + mtx->_44;
}

void F_API Vec3DTransformCoord(const Vec3D *v, const Matrix *mtx, Vec3D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y + mtx->_31*v->z + mtx->_41;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y + mtx->_32*v->z + mtx->_42;
	vOut->z = mtx->_13*v->x + mtx->_23*v->y + mtx->_33*v->z + mtx->_43;
}

void F_API Vec3DTransformNormal(const Vec3D *v, const Matrix *mtx, Vec3D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y + mtx->_31*v->z;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y + mtx->_32*v->z;
	vOut->z = mtx->_13*v->x + mtx->_23*v->y + mtx->_33*v->z;
}

//we assume that given vector is normalized
void F_API Vec3DPerpendicular(const Vec3D *v, Vec3D *vOut)
{
	s32	pos;
	f32 minelem = 1.0f;
	Vec3D tmpVec(0,0,0);

	/*
	** find the smallest magnitude axially aligned vector
	*/
	if(fabs(v->x) < minelem)
	{ pos = 0; minelem = fabsf(v->x); }

	if(fabs(v->y) < minelem)
	{ pos = 1; minelem = fabsf(v->y); }

	if(fabs(v->z) < minelem)
	{ pos = 2; minelem = fabsf(v->z); }

	
	switch(pos)
	{
	case 0:
		tmpVec.x = 1.0f;
		break;
	case 1:
		tmpVec.y = 1.0f;
		break;
	case 2:
		tmpVec.z = 1.0f;
		break;
	}

	/*
	** project the point onto the plane defined by src
	*/
	ProjectPointOnPlane(&tmpVec, v, vOut);

	/*
	** normalize the result
	*/
	Vec3DNormal(vOut, vOut);
}

void F_API Vec3DMakeNormalVectors(const Vec3D *forward, Vec3D *right, Vec3D *up)
{
	f32		d;

	// this rotate and negate guarantees a vector
	// not colinear with the original
	right->y = -forward->x;
	right->z = forward->y;
	right->x = forward->z;

	d = Vec3DDot(right, forward);

	*right = (*right) + -d*(*forward);

	Vec3DNormal(right, right);

	Vec3DCross(right, forward, up);
}

void F_API Vec3DRotatePointAroundVector(const Vec3D *dir, const Vec3D *point, f32 rad, Vec3D *vOut)
{
	f32		t0, t1;
	f32		c, s=rad;
	Vec3D	vr, vu, vf(*dir);

	c = cosf(s);
	s = sinf(s);

	Vec3DMakeNormalVectors(&vf, &vr, &vu);

	//basically, we are transforming the point

	t0 = vr.x *  c + vu.x * -s;
	t1 = vr.x *  s + vu.x *  c;
	vOut->x = (t0 * vr.x + t1 * vu.x + vf.x * vf.x) * point->x
		+ (t0 * vr.y + t1 * vu.y + vf.x * vf.y) * point->y
		+ (t0 * vr.z + t1 * vu.z + vf.x * vf.z) * point->z;

	t0 = vr.y *  c + vu.y * -s;
	t1 = vr.y *  s + vu.y *  c;
	vOut->y = (t0 * vr.x + t1 * vu.x + vf.y * vf.x) * point->x
		+ (t0 * vr.y + t1 * vu.y + vf.y * vf.y) * point->y
		+ (t0 * vr.z + t1 * vu.z + vf.y * vf.z) * point->z;

	t0 = vr.z *  c + vu.z * -s;
	t1 = vr.z *  s + vu.z *  c;
	vOut->z = (t0 * vr.x + t1 * vu.x + vf.z * vf.x) * point->x
		+ (t0 * vr.y + t1 * vu.y + vf.z * vf.y) * point->y
		+ (t0 * vr.z + t1 * vu.z + vf.z * vf.z) * point->z;
}

//
// 4D
//

void F_API Vec4DCross(const Vec4D *v1, const Vec4D *v2, const Vec4D *v3, Vec4D *vOut)
{
	f32  a, b, c, d, e, f;       // Intermediate Values

    // Calculate intermediate values.

    a = (v2->x * v3->y) - (v2->y * v3->x);
    b = (v2->x * v3->z) - (v2->z * v3->x);
    c = (v2->x * v3->w) - (v2->w * v3->x);
    d = (v2->y * v3->z) - (v2->z * v3->y);
    e = (v2->y * v3->w) - (v2->w * v3->y);
    f = (v2->z * v3->w) - (v2->w * v3->z);

    // Calculate the result-vector components.

    vOut->x =   (v1->y * f) - (v1->z * e) + (v1->w * d);
    vOut->y = - (v1->x * f) + (v1->z * c) - (v1->w * b);
    vOut->z =   (v1->x * e) - (v1->y * c) + (v1->w * a);
    vOut->w = - (v1->x * d) + (v1->y * b) - (v1->z * a);
}

void F_API Vec4DCatMullRom(const Vec4D *v1, const Vec4D *v2, const Vec4D *v3, const Vec4D *v4, f32 s, Vec4D *vOut)
{
	/*
	v1 = p2
	v2 = p3
	t1 = (p3 - p1) / 2
	t2 = (p4 - p2) / 2
	*/
	Vec4D t1((v3->x-v1->x)/2, (v3->y-v1->y)/2, (v3->z-v1->z)/2, (v3->w-v1->w)/2);
	Vec4D t2((v4->x-v2->x)/2, (v4->y-v2->y)/2, (v4->z-v2->z)/2, (v4->w-v2->w)/2);

	Vec4DHermite(v2, &t1, v3, &t2, s, vOut);
}

void F_API Vec4DHermite(const Vec4D *v1, const Vec4D *t1, const Vec4D *v2, const Vec4D *t2, f32 s, Vec4D *vOut)
{
	f32 s2 = s*s, s3 = s2*s;

	f32 h1 =  2*s3 - 3*s2 + 1;          // calculate basis function 1
	f32 h2 = -2*s3 + 3*s2;              // calculate basis function 2
	f32 h3 =  s3   - 2*s2 + s;         // calculate basis function 3
	f32 h4 =  s3   - s2;              // calculate basis function 4

	vOut->x = h1*v1->x + h2*v2->x + h3*t1->x + h4*t2->x;
	vOut->y = h1*v1->y + h2*v2->y + h3*t1->y + h4*t2->y;
	vOut->z = h1*v1->z + h2*v2->z + h3*t1->z + h4*t2->z;
	vOut->w = h1*v1->w + h2*v2->w + h3*t1->w + h4*t2->w;
}

f32 F_API Vec4DNormal(const Vec4D *v, Vec4D *vOut)
{
	f32 len = Vec4DDot(v,v);
	
	if(len)
	{
		len = sqrtf(len);
		vOut->x = v->x/len;
		vOut->y = v->y/len;
		vOut->z = v->z/len;
		vOut->w = v->w/len;
	}

	return len;
}

void F_API Vec4DTransform(const Vec4D *v, const Matrix *mtx, Vec4D *vOut)
{
	vOut->x = mtx->_11*v->x + mtx->_21*v->y + mtx->_31*v->z + mtx->_41*v->w;
	vOut->y = mtx->_12*v->x + mtx->_22*v->y + mtx->_32*v->z + mtx->_42*v->w;
	vOut->z = mtx->_13*v->x + mtx->_23*v->y + mtx->_33*v->z + mtx->_43*v->w;
	vOut->w = mtx->_14*v->x + mtx->_24*v->y + mtx->_34*v->z + mtx->_44*v->w;
}