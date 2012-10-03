#include "common.h"
#include "d3dx9math.h"
#include "GFX\gfx_math.h"

#define SLERP_TOLER	0.00000001f

void F_API QuatLN(const Quaternion *q, Quaternion *qOut)
{
	f32 theta = acosf(q->w);

	f32 s = theta >= SLERP_TOLER ? acosf(q->w)/sqrtf(1-(q->w*q->w)) : 0;

	qOut->w = 0;
	qOut->x = s*q->x;
	qOut->y = s*q->y;
	qOut->z = s*q->z;
}

void F_API QuatExp(const Quaternion *q, Quaternion *qOut)
{
	f32 vlen = sqrtf(q->x*q->x+q->y*q->y+q->z*q->z);

	if(vlen >= SLERP_TOLER)
	{
		f32 s = sinf(vlen)/vlen;

		qOut->w = cosf(vlen);
		qOut->x = s*q->x;
		qOut->y = s*q->y;
		qOut->z = s*q->z;
	}
	else
		QuatIdentity(qOut);
}

f32 F_API QuatNorm(const Quaternion *q, Quaternion *qOut)
{
	f32 len = QuatDot(q,q);
	
	if(len)
	{
		len = sqrtf(len);
		qOut->w = q->w/len;
		qOut->x = q->x/len;
		qOut->y = q->y/len;
		qOut->z = q->z/len;
	}

	return len;
}

void F_API QuatInv(const Quaternion *q, Quaternion *qOut)
{
	QuatConjugate(q, qOut);
	QuatNorm(qOut, qOut);
}

void F_API QuatRotAxis(const Vec3D *axis, f32 theta, Quaternion *qOut)
{
	f32 sinA = sinf(theta*0.5f);

    qOut->x = axis->x/sinA;
    qOut->y = axis->y/sinA;
    qOut->z = axis->z/sinA;
    qOut->w = cosf(theta*0.5f);
}

void F_API QuatRotEuler(const Vec3D *angles, Quaternion *qOut)
{
	f32 angle;
	f32 sr, sp, sy, cr, cp, cy;

	angle = angles->z*0.5f;
	sy = sinf(angle);
	cy = cosf(angle);
	angle = angles->y*0.5f;
	sp = sinf(angle);
	cp = cosf(angle);
	angle = angles->x*0.5f;
	sr = sinf(angle);
	cr = cosf(angle);

	f32 crcp = cr*cp;
	f32 srsp = sr*sp;

	qOut->x = sr*cp*cy-cr*sp*sy;
	qOut->y = cr*sp*cy+sr*cp*sy;
	qOut->z = crcp*sy-srsp*cy;
	qOut->w = crcp*cy+srsp*sy;
}

void F_API QuatRotMtx(const Matrix *mtx, Quaternion *qOut)
{
	//TODO: Implement me
	D3DXQuaternionRotationMatrix((D3DXQUATERNION *)qOut, (CONST D3DXMATRIX *)mtx);
}

void F_API QuatToAxisAngle(const Quaternion *q, Vec3D *axisOut, f32 *thetaOut)
{
	*thetaOut = 2*acosf(q->w);

	f32 s = sqrtf(q->x*q->x+q->y*q->y+q->z*q->z);

	axisOut->x = q->x/s;
	axisOut->y = q->y/s;
	axisOut->z = q->z/s;
}

void F_API QuatSlerp(const Quaternion *q1, const Quaternion *q2, f32 t, Quaternion *qOut)
{
	Quaternion qq2;

	//check to see if any of the quaternions is backwards
	f32 a = (q1->x-q2->x)*(q1->x-q2->x)
		+ (q1->y-q2->y)*(q1->y-q2->y)
		+ (q1->z-q2->z)*(q1->z-q2->z)
		+ (q1->w-q2->w)*(q1->w-q2->w);

	f32 b = (q1->x+q2->x)*(q1->x+q2->x)
		+ (q1->y+q2->y)*(q1->y+q2->y)
		+ (q1->z+q2->z)*(q1->z+q2->z)
		+ (q1->w+q2->w)*(q1->w+q2->w);

	if(a > b)
	{
		qq2.x = -q2->x;
		qq2.y = -q2->y;
		qq2.z = -q2->z;
		qq2.w = -q2->w;
	}
	else
		memcpy(&qq2, q2, sizeof(Quaternion));

	f32 cosom = QuatDot(q1, &qq2);

	f32 sclq1, sclq2;

	if ((1.0f+cosom) > SLERP_TOLER)
	{
		if ((1.0f-cosom) > SLERP_TOLER)
		{
			f32 omega = acosf(cosom);
			f32 sinom = sinf(omega);
			sclq1 = sinf((1.0f-t)*omega)/sinom;
			sclq2 = sinf(t*omega)/sinom;
		}
		else
		{
			sclq1 = 1.0f-t;
			sclq2 = t;
		}

		qOut->x = sclq1*q1->x+sclq2*qq2.x;
		qOut->y = sclq1*q1->y+sclq2*qq2.y;
		qOut->z = sclq1*q1->z+sclq2*qq2.z;
		qOut->w = sclq1*q1->w+sclq2*qq2.w;
	}
	else
	{
		qOut->x = -q1->y;
		qOut->y =  q1->x;
		qOut->z = -q1->w;
		qOut->w =  q1->z;

		sclq1 = sinf((1.0f-t)*0.5f*PI);
		sclq2 = sinf(t*0.5f*PI);

		qOut->x = sclq1*q1->x+sclq2*qOut->x;
		qOut->y = sclq1*q1->y+sclq2*qOut->y;
		qOut->z = sclq1*q1->z+sclq2*qOut->z;
	}
}

void F_API QuatBaryCentric(const Quaternion *q1, const Quaternion *q2, const Quaternion *q3, f32 f, f32 g,
							Quaternion *qOut)
{
	Quaternion sub_q1, sub_q2;

	QuatSlerp(q1, q2, f+g, &sub_q1);
	QuatSlerp(q1, q3, f+g, &sub_q2);

	QuatSlerp(&sub_q1, &sub_q2, g/(f+g), qOut);
}

void F_API QuatSquadSetup(const Quaternion *q0, const Quaternion *q1, const Quaternion *q2, const Quaternion *q3,
						   Quaternion *aOut, Quaternion *bOut, Quaternion *cOut)
{
	D3DXQuaternionSquadSetup((D3DXQUATERNION *)aOut, (D3DXQUATERNION *)bOut,
		(D3DXQUATERNION *)cOut, (const D3DXQUATERNION *)q0, (const D3DXQUATERNION *)q1,
		(const D3DXQUATERNION *)q2, (const D3DXQUATERNION *)q3);
}

void F_API QuatSquad(const Quaternion *q1, const Quaternion *a, const Quaternion *b, const Quaternion *c,
					  f32 t, Quaternion *qOut)
{
	D3DXQuaternionSquad((D3DXQUATERNION *)qOut,
    (CONST D3DXQUATERNION *)q1,
    (CONST D3DXQUATERNION *)a,
    (CONST D3DXQUATERNION *)b,
    (CONST D3DXQUATERNION *)c,
    t);
}