#include "common.h"
#include "GFX\gfx_math.h"

u8 F_API MtxInverse(const Matrix *mtx, Matrix *mOut)
{
	/*D3DXMatrixInverse((D3DXMATRIX *)mOut,
    0,
    (CONST D3DXMATRIX *)mtx
);*/

	f32 d00, d01, d02, d03;
	f32 d10, d11, d12, d13;
	f32 d20, d21, d22, d23;
	f32 d30, d31, d32, d33;
	f32 m00, m01, m02, m03;
	f32 m10, m11, m12, m13;
	f32 m20, m21, m22, m23;
	f32 m30, m31, m32, m33;

	f32 d;

	m00 = mtx->_11;  m01 = mtx->_12;  m02 = mtx->_13;  m03 = mtx->_14;
	m10 = mtx->_21;  m11 = mtx->_22;  m12 = mtx->_23;  m13 = mtx->_24;
	m20 = mtx->_31;  m21 = mtx->_32;  m22 = mtx->_33;  m23 = mtx->_34;
	m30 = mtx->_43;  m31 = mtx->_42;  m32 = mtx->_43;  m33 = mtx->_44;

	d00 = m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m31*m22*m13 - m32*m23*m11 - m33*m21*m12;
	d01 = m10*m22*m33 + m12*m23*m30 + m13*m20*m32 - m30*m22*m13 - m32*m23*m10 - m33*m20*m12;
	d02 = m10*m21*m33 + m11*m23*m30 + m13*m20*m31 - m30*m21*m13 - m31*m23*m10 - m33*m20*m11;
	d03 = m10*m21*m32 + m11*m22*m30 + m12*m20*m31 - m30*m21*m12 - m31*m22*m10 - m32*m20*m11;

	d10 = m01*m22*m33 + m02*m23*m31 + m03*m21*m32 - m31*m22*m03 - m32*m23*m01 - m33*m21*m02;
	d11 = m00*m22*m33 + m02*m23*m30 + m03*m20*m32 - m30*m22*m03 - m32*m23*m00 - m33*m20*m02;
	d12 = m00*m21*m33 + m01*m23*m30 + m03*m20*m31 - m30*m21*m03 - m31*m23*m00 - m33*m20*m01;
	d13 = m00*m21*m32 + m01*m22*m30 + m02*m20*m31 - m30*m21*m02 - m31*m22*m00 - m32*m20*m01;

	d20 = m01*m12*m33 + m02*m13*m31 + m03*m11*m32 - m31*m12*m03 - m32*m13*m01 - m33*m11*m02;
	d21 = m00*m12*m33 + m02*m13*m30 + m03*m10*m32 - m30*m12*m03 - m32*m13*m00 - m33*m10*m02;
	d22 = m00*m11*m33 + m01*m13*m30 + m03*m10*m31 - m30*m11*m03 - m31*m13*m00 - m33*m10*m01;
	d23 = m00*m11*m32 + m01*m12*m30 + m02*m10*m31 - m30*m11*m02 - m31*m12*m00 - m32*m10*m01;

	d30 = m01*m12*m23 + m02*m13*m21 + m03*m11*m22 - m21*m12*m03 - m22*m13*m01 - m23*m11*m02;
	d31 = m00*m12*m23 + m02*m13*m20 + m03*m10*m22 - m20*m12*m03 - m22*m13*m00 - m23*m10*m02;
	d32 = m00*m11*m23 + m01*m13*m20 + m03*m10*m21 - m20*m11*m03 - m21*m13*m00 - m23*m10*m01;
	d33 = m00*m11*m22 + m01*m12*m20 + m02*m10*m21 - m20*m11*m02 - m21*m12*m00 - m22*m10*m01;

	d = m00*d00 - m01*d01 + m02*d02 - m03*d03;

	if (d == 0)
	{
		return 0;
	}

	mOut->_11 =  d00/d; mOut->_12 = -d10/d;  mOut->_13 =  d20/d; mOut->_14 = -d30/d;
	mOut->_21 = -d01/d; mOut->_22 =  d11/d;  mOut->_23 = -d21/d; mOut->_24 =  d31/d;
	mOut->_31 =  d02/d; mOut->_32 = -d12/d;  mOut->_33 =  d22/d; mOut->_34 = -d32/d;
	mOut->_41 = -d03/d; mOut->_42 =  d13/d;  mOut->_43 = -d23/d; mOut->_44 =  d33/d;

	return 1;
}

f32 F_API MtxDeterminant(const Matrix *mtx)
{
	f32 d00, d01, d02, d03;
	f32 m00, m01, m02, m03;
	f32 m10, m11, m12, m13;
	f32 m20, m21, m22, m23;
	f32 m30, m31, m32, m33;

	m00 = mtx->_11;  m01 = mtx->_12;  m02 = mtx->_13;  m03 = mtx->_14;
	m10 = mtx->_21;  m11 = mtx->_22;  m12 = mtx->_23;  m13 = mtx->_24;
	m20 = mtx->_31;  m21 = mtx->_32;  m22 = mtx->_33;  m23 = mtx->_34;
	m30 = mtx->_43;  m31 = mtx->_42;  m32 = mtx->_43;  m33 = mtx->_44;

	d00 = m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m31*m22*m13 - m32*m23*m11 - m33*m21*m12;
	d01 = m10*m22*m33 + m12*m23*m30 + m13*m20*m32 - m30*m22*m13 - m32*m23*m10 - m33*m20*m12;
	d02 = m10*m21*m33 + m11*m23*m30 + m13*m20*m31 - m30*m21*m13 - m31*m23*m10 - m33*m20*m11;
	d03 = m10*m21*m32 + m11*m22*m30 + m12*m20*m31 - m30*m21*m12 - m31*m22*m10 - m32*m20*m11;

	return (m00*d00 - m01*d01 + m02*d02 - m03*d03);
}

void F_API MtxTranspose(const Matrix *mtx, Matrix *mOut)
{
	mOut->_11 = mtx->_11; mOut->_12 = mtx->_21; mOut->_13 = mtx->_31; mOut->_14 = mtx->_41;
	mOut->_21 = mtx->_12; mOut->_22 = mtx->_22; mOut->_23 = mtx->_32; mOut->_24 = mtx->_42;
	mOut->_31 = mtx->_13; mOut->_32 = mtx->_23; mOut->_33 = mtx->_33; mOut->_34 = mtx->_43;
	mOut->_41 = mtx->_14; mOut->_42 = mtx->_24; mOut->_43 = mtx->_34; mOut->_44 = mtx->_44;
}

void F_API MtxTranslate(const Vec3D *pt, Matrix *mOut)
{
	mOut->_11 = 1;	   mOut->_12 = 0;	  mOut->_13 = 0;	 mOut->_14 = 0;
	mOut->_21 = 0;	   mOut->_22 = 1;	  mOut->_23 = 0;	 mOut->_24 = 0;
	mOut->_31 = 0;     mOut->_32 = 0;	  mOut->_33 = 1;	 mOut->_34 = 0;
	mOut->_41 = pt->x; mOut->_42 = pt->y; mOut->_43 = pt->z; mOut->_44 = 1;
}

void F_API MtxScale(const Vec3D *scale, Matrix *mOut)
{
	mOut->_11 = scale->x; mOut->_12 = 0;		mOut->_13 = 0;		  mOut->_14 = 0;
	mOut->_21 = 0;		  mOut->_22 = scale->y; mOut->_23 = 0;		  mOut->_24 = 0;
	mOut->_31 = 0;		  mOut->_32 = 0;		mOut->_33 = scale->z; mOut->_34 = 0;
	mOut->_41 = 0;		  mOut->_42 = 0;		mOut->_43 = 0;		  mOut->_44 = 1;
}

void F_API MtxRotateX(f32 angle, Matrix *mOut)
{
	f32 c = cosf(angle);
	f32 s = sinf(angle);

	mOut->_11 = 1; mOut->_21 = 0;  mOut->_31 = 0; mOut->_41 = 0;
	mOut->_12 = 0; mOut->_22 = c;  mOut->_32 = s; mOut->_42 = 0;
	mOut->_13 = 0; mOut->_23 = -s; mOut->_33 = c; mOut->_43 = 0;
	mOut->_14 = 0; mOut->_24 = 0;  mOut->_34 = 0; mOut->_44 = 1;
}

void F_API MtxRotateY(f32 angle, Matrix *mOut)
{
	f32 c = cosf(angle);
	f32 s = sinf(angle);

	mOut->_11 = c;  mOut->_21 = 0; mOut->_31 = s; mOut->_41 = 0;
	mOut->_12 = 0;  mOut->_22 = 1; mOut->_32 = 0; mOut->_42 = 0;
	mOut->_13 = -s; mOut->_23 = 0; mOut->_33 = c; mOut->_43 = 0;
	mOut->_14 = 0;  mOut->_24 = 0; mOut->_34 = 0; mOut->_44 = 1;
}

void F_API MtxRotateZ(f32 angle, Matrix *mOut)
{
	f32 c = cosf(angle);
	f32 s = sinf(angle);

	mOut->_11 = c;  mOut->_21 = s; mOut->_31 = 0; mOut->_41 = 0;
	mOut->_12 = -s; mOut->_22 = c; mOut->_32 = 0; mOut->_42 = 0;
	mOut->_13 = 0;  mOut->_23 = 0; mOut->_33 = 1; mOut->_43 = 0;
	mOut->_14 = 0;  mOut->_24 = 0; mOut->_34 = 0; mOut->_44 = 1;
}

void F_API MtxYawPitchRoll(f32 yaw, f32 pitch, f32 roll, Matrix *mOut)
{
	f32 cr = cosf(pitch);
	f32 sr = sinf(pitch);
	f32 cp = cosf(yaw);
	f32 sp = sinf(yaw);
	f32 cy = cosf(roll);
	f32 sy = sinf(roll);

	f32 srsp = sr*sp;
	f32 crsp = cr*sp;

	mOut->_11 = cp*cy;
	mOut->_12 = cp*sy;
	mOut->_13 = -sp;
	mOut->_14 = 0;

	mOut->_21 = srsp*cy-cr*sy;
	mOut->_22 = srsp*sy+cr*cy;
	mOut->_23 = sr*cp;
	mOut->_24 = 0;

	mOut->_31 = crsp*cy+sr*sy;
	mOut->_32 = crsp*sy-sr*cy;
	mOut->_33 = cr*cp;
	mOut->_34 = 0;
	
	mOut->_41 = 0;
	mOut->_42 = 0;
	mOut->_43 = 0;
	mOut->_44 = 1;
}

void F_API MtxAxisRotation(const Vec3D *axis, f32 angle, Matrix *mOut)
{
	//Vec3D axisN;

	f32 cosx, sinx;

	//len = Vec3DNormal(axis, &axisN);

	cosx = cosf(angle);
	sinx = sinf(angle);

	//funky stuff
	mOut->_11 = axis->x * axis->x + cosx * (1.0f - axis->x * axis->x);
	mOut->_12 = axis->x * axis->y * (1.0f - cosx) + axis->z * sinx;
	mOut->_13 = axis->x * axis->z * (1.0f - cosx) - axis->y * sinx;
	mOut->_14 = 0;

	mOut->_21 = axis->x * axis->y * (1.0f - cosx) - axis->z * sinx;
	mOut->_22 = axis->y * axis->y + cosx * (1.0f - axis->y * axis->y);
	mOut->_23 = axis->y * axis->z * (1.0f - cosx) + axis->x * sinx;
	mOut->_24 = 0;

	mOut->_31 = axis->x * axis->z * (1.0f - cosx) + axis->y * sinx;
	mOut->_32 = axis->y * axis->z * (1.0f - cosx) - axis->x * sinx;
	mOut->_33 = axis->z * axis->z + cosx * (1.0f - axis->z * axis->z);
	mOut->_34 = 0;

	mOut->_41 = 0;
	mOut->_42 = 0;
	mOut->_43 = 0;
	mOut->_44 = 1;
}

void F_API MtxQuaternion(const Quaternion *q, Matrix *mOut)
{
	mOut->_11 = 1.0f - 2.0f*(q->y*q->y + q->z*q->z);
	mOut->_12 = 2.0f*(q->x*q->y + q->w*q->z);
	mOut->_13 = 2.0f*(q->x*q->z - q->w*q->y);
	mOut->_14 = 0;

	mOut->_21 = 2.0f*(q->x*q->y - q->w*q->z);
	mOut->_22 = 1.0f - 2.0f*(q->x*q->x + q->z*q->z);
	mOut->_23 = 2.0f*(q->y*q->z + q->w*q->x);
	mOut->_24 = 0;

	mOut->_31 = 2.0f*(q->x*q->z + q->w*q->y);
	mOut->_32 = 2.0f*(q->y*q->z - q->w*q->x);
	mOut->_33 = 1.0f - 2.0f*(q->x*q->x + q->y*q->y);
	mOut->_34 = 0;

	mOut->_41 = 0;
	mOut->_42 = 0;
	mOut->_43 = 0;
	mOut->_44 = 1;
}

//Matrix Transformation for 3D (using the Right Hand rule)

//w = width, h = height, zn = z near, zf = z far
void F_API MtxOrtho(f32 w, f32 h, f32 zn, f32 zf, Matrix *mOut)
{
	mOut->_11 = 2/w; mOut->_12 = 0;   mOut->_13 = 0;          mOut->_14 = 0;
	mOut->_21 = 0;   mOut->_22 = 2/h; mOut->_23 = 0;          mOut->_24 = 0;
	mOut->_31 = 0;   mOut->_32 = 0;   mOut->_33 = 1/(zn-zf);  mOut->_34 = 0;
	mOut->_41 = 0;   mOut->_42 = 0;   mOut->_43 = zn/(zn-zf); mOut->_44 = 1;
}

//l = left, r = right, t = top, b = bottom, zn = z near, zf = z far
void F_API MtxOrthoOffCenter(f32 l, f32 r, f32 t, f32 b, f32 zn, f32 zf, Matrix *mOut)
{
	mOut->_11 = 2/(r-l);     mOut->_12 = 0;           mOut->_13 = 0;          mOut->_14 = 0;
	mOut->_21 = 0;           mOut->_22 = 2/(t-b);     mOut->_23 = 0;          mOut->_24 = 0;
	mOut->_31 = 0;           mOut->_32 = 0;           mOut->_33 = 1/(zn-zf);  mOut->_34 = 0;
	mOut->_41 = (1+r)/(l-r); mOut->_42 = (t+b)/(b-t); mOut->_43 = zn/(zn-zf); mOut->_44 = 1;
}

//w = width, h = height, zn = z near, zf = z far
void F_API MtxPerspective(f32 w, f32 h, f32 zn, f32 zf, Matrix *mOut)
{
	mOut->_11 = 2*zn/w; mOut->_12 = 0;		mOut->_13 = 0;			   mOut->_14 = 0;
	mOut->_21 = 0;		mOut->_22 = 2*zn/h; mOut->_23 = 0;			   mOut->_24 = 0;
	mOut->_31 = 0;		mOut->_32 = 0;		mOut->_33 = zf/(zn-zf);	   mOut->_34 = -1;
	mOut->_41 = 0;		mOut->_42 = 0;		mOut->_43 = zn*zf/(zn-zf); mOut->_44 = 0;
}

//l = left, r = right, t = top, b = bottom, zn = z near, zf = z far
void F_API MtxPerspectiveOffCenter(f32 l, f32 r, f32 t, f32 b, f32 zn, f32 zf, Matrix *mOut)
{
	mOut->_11 = 2*zn/(r-l);  mOut->_12 = 0;			  mOut->_13 = 0;			 mOut->_14 = 0;
	mOut->_21 = 0;           mOut->_22 = 2*zn*(t-b);  mOut->_23 = 0;			 mOut->_24 = 0;
	mOut->_31 = (l+r)/(r-l); mOut->_32 = (t+b)/(t-b); mOut->_33 = zf/(zn-zf);    mOut->_34 = -1;
	mOut->_41 = 0;			 mOut->_42 = 0;			  mOut->_43 = zn*zf/(zn-zf); mOut->_44 = 0;
}

//fov = in radians, aspect = ratio: w/h, zn = z near, zf = z far
void F_API MtxPerspectiveFOV(f32 fov, f32 aspect, f32 zn, f32 zf, Matrix *mOut)
{
	f32 h = cosf(fov/2) / sinf(fov/2);
	f32 w = h / aspect;

	mOut->_11 = w; mOut->_12 = 0; mOut->_13 = 0;			 mOut->_14 = 0;
	mOut->_21 = 0; mOut->_22 = h; mOut->_23 = 0;			 mOut->_24 = 0;
	mOut->_31 = 0; mOut->_32 = 0; mOut->_33 = zf/(zn-zf);    mOut->_34 = -1;
	mOut->_41 = 0; mOut->_42 = 0; mOut->_43 = zn*zf/(zn-zf); mOut->_44 = 0;
}

void F_API MtxLookAt(const Vec3D *center, const Vec3D *dir, const Vec3D *up, Matrix *mOut)
{
	//let's create the w vector(direction normalized)
	Vec3D w((*center) - (*dir)); Vec3DNormal(&w, &w);

	//project 'up' to w
	Vec3D u; Vec3DCross(up, &w, &u); Vec3DNormal(&u, &u);

	//get the real up vector
	Vec3D v; Vec3DCross(&w, &u, &v);

	mOut->_11 = u.x;				   mOut->_12 = v.x;				  mOut->_13 = w.x;				     mOut->_14 = 0;
	mOut->_21 = u.y;				   mOut->_22 = v.y;				  mOut->_23 = w.y;				     mOut->_24 = 0;
	mOut->_31 = u.z;				   mOut->_32 = v.z;			      mOut->_33 = w.z;				     mOut->_34 = 0;
	mOut->_41 = -Vec3DDot(&u, center); mOut->_42 = -Vec3DDot(&v, center); mOut->_43 = -Vec3DDot(&w, center); mOut->_44 = 1;
}