#ifndef _gfx_math_h
#define _gfx_math_h

#include "GFX\gfx_math_types.h"

//
// Vector
//

DLL_API void F_API Vec2DCatMullRom(const Vec2D *v1, const Vec2D *v2, const Vec2D *v3, const Vec2D *v4, f32 s, Vec2D *vOut);
DLL_API void F_API Vec2DHermite(const Vec2D *v1, const Vec2D *t1, const Vec2D *v2, const Vec2D *t2, f32 s, Vec2D *vOut);
DLL_API f32 F_API Vec2DNormal(const Vec2D *v, Vec2D *vOut);

DLL_API void F_API Vec2DTransform(const Vec2D *v, const Matrix *mtx, Vec4D *vOut);
DLL_API void F_API Vec2DTransformCoord(const Vec2D *v, const Matrix *mtx, Vec2D *vOut);
DLL_API void F_API Vec2DTransformNormal(const Vec2D *v, const Matrix *mtx, Vec2D *vOut);
DLL_API f32  F_API Vec2DGetAngle(const Vec2D *v);

DLL_API void F_API Vec3DCatMullRom(const Vec3D *v1, const Vec3D *v2, const Vec3D *v3, const Vec3D *v4, f32 s, Vec3D *vOut);
DLL_API void F_API Vec3DHermite(const Vec3D *v1, const Vec3D *t1, const Vec3D *v2, const Vec3D *t2, f32 s, Vec3D *vOut);
DLL_API f32  F_API Vec3DNormal(const Vec3D *v, Vec3D *vOut);

DLL_API void F_API Vec3DTransform(const Vec3D *v, const Matrix *mtx, Vec4D *vOut);
DLL_API void F_API Vec3DTransformCoord(const Vec3D *v, const Matrix *mtx, Vec3D *vOut);
DLL_API void F_API Vec3DTransformNormal(const Vec3D *v, const Matrix *mtx, Vec3D *vOut);

//we assume that given vector is normalized
DLL_API void F_API Vec3DPerpendicular(const Vec3D *v, Vec3D *vOut);

DLL_API void F_API Vec3DMakeNormalVectors(const Vec3D *forward, Vec3D *right, Vec3D *up);

DLL_API void F_API Vec3DRotatePointAroundVector(const Vec3D *dir, const Vec3D *point, f32 rad, Vec3D *vOut);

DLL_API void F_API Vec4DCatMullRom(const Vec4D *v1, const Vec4D *v2, const Vec4D *v3, const Vec4D *v4, f32 s, Vec4D *vOut);
DLL_API void F_API Vec4DHermite(const Vec4D *v1, const Vec4D *t1, const Vec4D *v2, const Vec4D *t2, f32 s, Vec4D *vOut);
DLL_API f32 F_API Vec4DNormal(const Vec4D *v, Vec4D *vOut);

DLL_API void F_API Vec4DCross(const Vec4D *v1, const Vec4D *v2, const Vec4D *v3, Vec4D *vOut);

DLL_API void F_API Vec4DTransform(const Vec4D *v, const Matrix *mtx, Vec4D *vOut);

//
// Quaternion
//

DLL_API void F_API QuatLN(const Quaternion *q, Quaternion *qOut);
DLL_API void F_API QuatExp(const Quaternion *q, Quaternion *qOut);
DLL_API f32 F_API QuatNorm(const Quaternion *q, Quaternion *qOut); //qNorm = q/len(q)
DLL_API void F_API QuatInv(const Quaternion *q, Quaternion *qOut); //qInv = qConj / len(q)
DLL_API void F_API QuatRotAxis(const Vec3D *axis, f32 theta, Quaternion *qOut);
DLL_API void F_API QuatRotEuler(const Vec3D *angles, Quaternion *qOut);
DLL_API void F_API QuatRotMtx(const Matrix *mtx, Quaternion *qOut);
DLL_API void F_API QuatToAxisAngle(const Quaternion *q, Vec3D *axisOut, f32 *thetaOut);

DLL_API void F_API QuatSlerp(const Quaternion *q1, const Quaternion *q2, f32 t, Quaternion *qOut);

DLL_API void F_API QuatBaryCentric(const Quaternion *q1, const Quaternion *q2, 
							const Quaternion *q3, f32 f, f32 g, Quaternion *qOut);

DLL_API void F_API QuatSquadSetup(const Quaternion *q0, const Quaternion *q1, const Quaternion *q2, const Quaternion *q3,
						   Quaternion *aOut, Quaternion *bOut, Quaternion *cOut);

DLL_API void F_API QuatSquad(const Quaternion *q1, const Quaternion *a, const Quaternion *b, const Quaternion *c,
					  f32 t, Quaternion *qOut);

//
// Plane
//

DLL_API void F_API PlaneFromPoints(const Vec3D *pt1, const Vec3D *pt2, const Vec3D *pt3, Plane *plOut);

DLL_API void F_API PlaneNormalize(const Plane *p, Plane *pOut);

//mtx = transpose(inv(mtx))
DLL_API void F_API PlaneTransform(const Matrix *mtx, const Plane *p, Plane *pOut);

DLL_API void F_API ProjectPointOnPlane(const Vec3D *p, const Vec3D *norm, Vec3D *vOut);

//for easy side testing, you should use this
DLL_API void F_API PlaneCategorize(Plane *p);

/*
=================
PlaneTypeForNormal
returns: PLANE_X, PLANE_Y, PLANE_Z, or PLANE_NONAXIAL
=================
*/
DLL_API s16	F_API PlaneTypeForNormal(const Plane *p);

/*
=================
SignbitsForPlane
=================
*/
DLL_API s16 F_API SignbitsForPlane(const Plane *p);

/*
==================
BoxOnPlaneSide
Assumes you categorized
the plane (PlaneCategorize)

Returns 1, 2, or 1 + 2
==================
*/
DLL_API s32 F_API BoxOnPlaneSide (const Vec3D *mins, const Vec3D *maxs, const Plane *p);

//
// Matrix
//

DLL_API u8 F_API MtxInverse(const Matrix *mtx, Matrix *mOut); //returns 0 if mtx is non-invertable
DLL_API f32 F_API MtxDeterminant(const Matrix *mtx);
DLL_API void F_API MtxTranspose(const Matrix *mtx, Matrix *mOut);

DLL_API void F_API MtxTranslate(const Vec3D *pt, Matrix *mOut);
DLL_API void F_API MtxScale(const Vec3D *scale, Matrix *mOut);
DLL_API void F_API MtxRotateX(f32 angle, Matrix *mOut);
DLL_API void F_API MtxRotateY(f32 angle, Matrix *mOut);
DLL_API void F_API MtxRotateZ(f32 angle, Matrix *mOut);
DLL_API void F_API MtxYawPitchRoll(f32 yaw, f32 pitch, f32 roll, Matrix *mOut);
DLL_API void F_API MtxAxisRotation(const Vec3D *axis, f32 angle, Matrix *mOut);
DLL_API void F_API MtxQuaternion(const Quaternion *q, Matrix *mOut);

//Matrix Transformation for 3D (using the Right Hand rule)

//w = width, h = height, zn = z near, zf = z far
DLL_API void F_API MtxOrtho(f32 w, f32 h, f32 zn, f32 zf, Matrix *mOut);

//l = left, r = right, t = top, b = bottom, zn = z near, zf = z far
DLL_API void F_API MtxOrthoOffCenter(f32 l, f32 r, f32 t, f32 b, f32 zn, f32 zf, Matrix *mOut);

//w = width, h = height, zn = z near, zf = z far
DLL_API void F_API MtxPerspective(f32 w, f32 h, f32 zn, f32 zf, Matrix *mOut);

//l = left, r = right, t = top, b = bottom, zn = z near, zf = z far
DLL_API void F_API MtxPerspectiveOffCenter(f32 l, f32 r, f32 t, f32 b, f32 zn, f32 zf, Matrix *mOut);

//fov = in radians, aspect = ratio: w/h, zn = z near, zf = z far
DLL_API void F_API MtxPerspectiveFOV(f32 fov, f32 aspect, f32 zn, f32 zf, Matrix *mOut);

DLL_API void F_API MtxLookAt(const Vec3D *center, const Vec3D *dir, const Vec3D *up, Matrix *mOut);

//
//extra Math stuff beyond the basic elements
//

//
// Rays
//

/////////////////////////////////////
// Name:	GFXIntersectBox
// Purpose:	not really related to
//			anything Graphics but nice
//			to have.  Intersect given
//			ray to axis aligned bound box.
// Output:	tOut and nOut filled
// Return:	scalar t, if 1, then no intersect
/////////////////////////////////////
DLL_API f32 F_API RayIntersectBox(const Vec3D *p, const Vec3D *dir,
					const Vec3D *boxMin, const Vec3D *boxMax,
					Vec3D *nOut);

//
// Bounds
//

DLL_API u8 F_API BoundsIntersect(const Vec3D & mins1, const Vec3D & maxs1, const Vec3D & mins2, const Vec3D & maxs2);

DLL_API void F_API BoundsClear(Vec3D & mins, Vec3D & maxs);

DLL_API void F_API BoundsAddPoint(const Vec3D & v, Vec3D & mins, Vec3D & maxs);

//
// Randomizers
//
DLL_API f64 F_API RandDouble();

DLL_API f32 F_API RandRngFloat(f32 a, f32 b);

DLL_API s32 F_API RandRngInt(s32 a1, s32 a2);

#endif