#ifndef _gfx_math_types_h
#define _gfx_math_types_h

//defines
#ifndef PI
#define PI			3.141592654f
#endif

#define I_PI		0.318309886f

#define TO_RAD( degree ) ((degree) * (PI / 180.0f))
#define TO_DEGREE( radian ) ((radian) * (180.0f / PI))

/////////////////////////////////////////////
// Macros
//

/////////////////////
//other stuff

#define Q_rint(x)	((x) < 0 ? ((s32)((x)-0.5f)) : ((s32)((x)+0.5f)))

/////////////////////
//Vector 2D

#define Vec2DAdd(v1, v2, vOut) ((vOut)->x = (v1)->x+(v2)->x, (vOut)->y = (v1)->y+(v2)->y)
#define Vec2DSub(v1, v2, vOut) ((vOut)->x = (v1)->x-(v2)->x, (vOut)->y = (v1)->y-(v2)->y)
#define Vec2DScale(v1, s, vOut) ((vOut)->x = (v1)->x*(s), (vOut)->y = (v1)->y*(s))
#define Vec2DInvScale(v1, s, vOut) ((vOut)->x = (v1)->x/(s), (vOut)->y = (v1)->y/(s))
#define Vec2DDot(v1, v2) ((v1)->x*(v2)->x + (v1)->y*(v2)->y)
#define Vec2DLen(v) (sqrtf((v)->x*(v)->x + (v)->y*(v)->y))
#define Vec2DLenSQ(v) ((v)->x*(v)->x + (v)->y*(v)->y)
#define Vec2DScaleTo(v, s, vOut) { float vs = s / Vec2DLen(v); Vec2DScale(v, vs, vOut); }
#define Vec2DMax(v1, v2, vOut) ((vOut)->x = MAX((v1)->x,(v2)->x), (vOut)->y = MAX((v1)->y,(v2)->y))
#define Vec2DMin(v1, v2, vOut) ((vOut)->x = MIN((v1)->x,(v2)->x), (vOut)->y = MIN((v1)->y,(v2)->y))
#define Vec2DCCW(v1, v2) ((v1)->x*(v2)->y - (v1)->y*(v2)->x)
#define Vec2DLerp(v1, v2, t, vOut) ((vOut)->x = (v1)->x + (t)*((v2)->x-(v1)->x), \
	                                (vOut)->y = (v1)->y + (t)*((v2)->y-(v1)->y))

//V1 + f(V2-V1) + g(V3-V1)
#define Vec2DBaryCentric(v1, v2, v3, f, g, vOut) ((vOut)->x = (v1)->x + (f)*((v2)->x-(v1)->x) + (g)*((v3)->x-(v1)->x), \
												  (vOut)->y = (v1)->y + (f)*((v2)->y-(v1)->y) + (g)*((v3)->y-(v1)->y))

/////////////////////
//Vector 3D

#define Vec3DAdd(v1, v2, vOut) ((vOut)->x = (v1)->x+(v2)->x, (vOut)->y = (v1)->y+(v2)->y, (vOut)->z = (v1)->z+(v2)->z)
#define Vec3DSub(v1, v2, vOut) ((vOut)->x = (v1)->x-(v2)->x, (vOut)->y = (v1)->y-(v2)->y, (vOut)->z = (v1)->z-(v2)->z)
#define Vec3DScale(v1, s, vOut) ((vOut)->x = (v1)->x*(s), (vOut)->y = (v1)->y*(s), (vOut)->z = (v1)->z*(s))
#define Vec3DInvScale(v1, s, vOut) ((vOut)->x = (v1)->x/(s), (vOut)->y = (v1)->y/(s), (vOut)->z = (v1)->z/(s))
#define Vec3DDot(v1, v2) ((v1)->x*(v2)->x + (v1)->y*(v2)->y + (v1)->z*(v2)->z)
#define Vec3DLen(v) (sqrtf((v)->x*(v)->x + (v)->y*(v)->y + (v)->z*(v)->z))
#define Vec3DLenSQ(v) ((v)->x*(v)->x + (v)->y*(v)->y + (v)->z*(v)->z)
#define Vec3DxzL(v) (sqrtf((v)->x * (v)->x + (v)->z * (v)->z))
#define Vec3DxzA(v) (atan2f((v)->y, Vec3DxzL(v)))
#define Vec3DScaleTo(v, s, vOut) { float vs = s / Vec3DLen(v); Vec3DScale(v, vs, vOut); }
#define Vec3DMax(v1, v2, vOut) ((vOut)->x = MAX((v1)->x,(v2)->x), (vOut)->y = MAX((v1)->y,(v2)->y), (vOut)->z = MAX((v1)->z,(v2)->z))
#define Vec3DMin(v1, v2, vOut) ((vOut)->x = MIN((v1)->x,(v2)->x), (vOut)->y = MIN((v1)->y,(v2)->y), (vOut)->z = MIN((v1)->z,(v2)->z))
#define Vec3DCross(v1, v2, vOut) ((vOut)->x = (v1)->y*(v2)->z - (v1)->z*(v2)->y, \
								  (vOut)->y = ((v1)->z*(v2)->x - (v1)->x*(v2)->z), \
								  (vOut)->z = (v1)->x*(v2)->y - (v1)->y*(v2)->x)

#define Vec3DLerp(v1, v2, t, vOut) ((vOut)->x = (v1)->x + (t)*((v2)->x-(v1)->x), \
	                                (vOut)->y = (v1)->y + (t)*((v2)->y-(v1)->y), \
									(vOut)->z = (v1)->z + (t)*((v2)->z-(v1)->z))

//V1 + f(V2-V1) + g(V3-V1)
#define Vec3DBaryCentric(v1, v2, v3, f, g, vOut) ((vOut)->x = (v1)->x + (f)*((v2)->x-(v1)->x) + (g)*((v3)->x-(v1)->x), \
												  (vOut)->y = (v1)->y + (f)*((v2)->y-(v1)->y) + (g)*((v3)->y-(v1)->y), \
												  (vOut)->z = (v1)->z + (f)*((v2)->z-(v1)->z) + (g)*((v3)->z-(v1)->z))

/////////////////////
//Vector 4D

#define Vec4DAdd(v1, v2, vOut) ((vOut)->x = (v1)->x+(v2)->x, (vOut)->y = (v1)->y+(v2)->y, (vOut)->z = (v1)->z+(v2)->z, (vOut)->w = (v1)->w+(v2)->w)
#define Vec4DSub(v1, v2, vOut) ((vOut)->x = (v1)->x-(v2)->x, (vOut)->y = (v1)->y-(v2)->y, (vOut)->z = (v1)->z-(v2)->z, (vOut)->w = (v1)->w-(v2)->w)
#define Vec4DScale(v1, s, vOut) ((vOut)->x = (v1)->x*(s), (vOut)->y = (v1)->y*(s), (vOut)->z = (v1)->z*(s), (vOut)->w = (v1)->w*(s))
#define Vec4DInvScale(v1, s, vOut) ((vOut)->x = (v1)->x/(s), (vOut)->y = (v1)->y/(s), (vOut)->z = (v1)->z/(s), (vOut)->w = (v1)->w/(s))
#define Vec4DDot(v1, v2) ((v1)->x*(v2)->x + (v1)->y*(v2)->y + (v1)->z*(v2)->z + (v1)->w*(v2)->w)
#define Vec4DLen(v) (sqrtf((v)->x*(v)->x + (v)->y*(v)->y + (v)->z*(v)->z + (v)->w*(v)->w))
#define Vec4DLenSQ(v) ((v)->x*(v)->x + (v)->y*(v)->y + (v)->z*(v)->z + (v)->w*(v)->w)
#define Vec4DScaleTo(v, s, vOut) { float vs = s / Vec4DLen(v); Vec4DScale(v, vs, vOut); }
#define Vec4DMax(v1, v2, vOut) ((vOut)->x = MAX((v1)->x,(v2)->x), (vOut)->y = MAX((v1)->y,(v2)->y), (vOut)->z = MAX((v1)->z,(v2)->z), (vOut)->w = MAX((v1)->w,(v2)->w))
#define Vec4DMin(v1, v2, vOut) ((vOut)->x = MIN((v1)->x,(v2)->x), (vOut)->y = MIN((v1)->y,(v2)->y), (vOut)->z = MIN((v1)->z,(v2)->z), (vOut)->w = MIN((v1)->w,(v2)->w))

#define Vec4DLerp(v1, v2, t, vOut) ((vOut)->x = (v1)->x + (t)*((v2)->x-(v1)->x), \
	                                (vOut)->y = (v1)->y + (t)*((v2)->y-(v1)->y), \
									(vOut)->z = (v1)->z + (t)*((v2)->z-(v1)->z), \
									(vOut)->w = (v1)->w + (t)*((v2)->w-(v1)->w))

//V1 + f(V2-V1) + g(V3-V1)
#define Vec4DBaryCentric(v1, v2, v3, f, g, vOut) ((vOut)->x = (v1)->x + (f)*((v2)->x-(v1)->x) + (g)*((v3)->x-(v1)->x), \
												  (vOut)->y = (v1)->y + (f)*((v2)->y-(v1)->y) + (g)*((v3)->y-(v1)->y), \
												  (vOut)->z = (v1)->z + (f)*((v2)->z-(v1)->z) + (g)*((v3)->z-(v1)->z), \
												  (vOut)->w = (v1)->w + (f)*((v2)->w-(v1)->w) + (g)*((v3)->w-(v1)->w))

/////////////////////
//Quaternion

#define QuatIdentity(qOut) ((qOut)->w = 1, (qOut)->x = 0, (qOut)->y = 0, (qOut)->z = 0)
#define QuatDot(q1, q2) (Vec4DDot(q1, q2))
#define QuatLen(q) (Vec4DLen(q))
#define QuatLenSq(q) (Vec4DLenSQ(q))
#define QuatConjugate(q, qOut) ((qOut)->w = (q)->w, (qOut)->x = -(q)->x, (qOut)->y = -(q)->y, (qOut)->z = -(q)->z)
#define QuatAdd(q1, q2, qOut) (Vec4DAdd(q1,q2,qOut))
#define QuatSub(q1, q2, qOut) (Vec4DSub(q1,q2,qOut))
#define QuatMult(q1, q2, qOut) \
	((qOut)->x = (q1)->w * (q2)->x + (q1)->x * (q2)->w + (q1)->y * (q2)->z - (q1)->z * (q2)->y, \
	 (qOut)->y = (q1)->w * (q2)->y - (q1)->x * (q2)->z + (q1)->y * (q2)->w + (q1)->z * (q2)->x, \
	 (qOut)->z = (q1)->w * (q2)->z + (q1)->x * (q2)->y - (q1)->y * (q2)->x + (q1)->z * (q2)->w, \
	 (qOut)->w = (q1)->w * (q2)->w - (q1)->x * (q2)->x - (q1)->y * (q2)->y - (q1)->z * (q2)->z)

/////////////////////
//Matrix

#define MtxIdentity(mOut) \
   ((mOut)->_11 = 1, (mOut)->_12 = 0, (mOut)->_13 = 0, (mOut)->_14 = 0,\
	(mOut)->_21 = 0, (mOut)->_22 = 1, (mOut)->_23 = 0, (mOut)->_24 = 0,\
	(mOut)->_31 = 0, (mOut)->_32 = 0, (mOut)->_33 = 1, (mOut)->_34 = 0,\
	(mOut)->_41 = 0, (mOut)->_42 = 0, (mOut)->_43 = 0, (mOut)->_44 = 1)

#define MtxMultiply(m1, m2, mOut) \
   ((mOut)->_11 = (m1)->_11*(m2)->_11+(m1)->_12*(m2)->_21+(m1)->_13*(m2)->_31+(m1)->_14*(m2)->_41,\
	(mOut)->_21 = (m1)->_21*(m2)->_11+(m1)->_22*(m2)->_21+(m1)->_23*(m2)->_31+(m1)->_24*(m2)->_41,\
	(mOut)->_31 = (m1)->_31*(m2)->_11+(m1)->_32*(m2)->_21+(m1)->_33*(m2)->_31+(m1)->_34*(m2)->_41,\
	(mOut)->_41 = (m1)->_41*(m2)->_11+(m1)->_42*(m2)->_21+(m1)->_43*(m2)->_31+(m1)->_44*(m2)->_41,\
	(mOut)->_12 = (m1)->_11*(m2)->_12+(m1)->_12*(m2)->_22+(m1)->_13*(m2)->_32+(m1)->_14*(m2)->_42,\
	(mOut)->_22 = (m1)->_21*(m2)->_12+(m1)->_22*(m2)->_22+(m1)->_23*(m2)->_32+(m1)->_24*(m2)->_42,\
	(mOut)->_32 = (m1)->_31*(m2)->_12+(m1)->_32*(m2)->_22+(m1)->_33*(m2)->_32+(m1)->_34*(m2)->_42,\
	(mOut)->_42 = (m1)->_41*(m2)->_12+(m1)->_42*(m2)->_22+(m1)->_43*(m2)->_32+(m1)->_44*(m2)->_42,\
	(mOut)->_13 = (m1)->_11*(m2)->_13+(m1)->_12*(m2)->_23+(m1)->_13*(m2)->_33+(m1)->_14*(m2)->_43,\
	(mOut)->_23 = (m1)->_21*(m2)->_13+(m1)->_22*(m2)->_23+(m1)->_23*(m2)->_33+(m1)->_24*(m2)->_43,\
	(mOut)->_33 = (m1)->_31*(m2)->_13+(m1)->_32*(m2)->_23+(m1)->_33*(m2)->_33+(m1)->_34*(m2)->_43,\
	(mOut)->_43 = (m1)->_41*(m2)->_13+(m1)->_42*(m2)->_23+(m1)->_43*(m2)->_33+(m1)->_44*(m2)->_43,\
	(mOut)->_14 = (m1)->_11*(m2)->_14+(m1)->_12*(m2)->_24+(m1)->_13*(m2)->_34+(m1)->_14*(m2)->_44,\
	(mOut)->_24 = (m1)->_21*(m2)->_14+(m1)->_22*(m2)->_24+(m1)->_23*(m2)->_34+(m1)->_24*(m2)->_44,\
	(mOut)->_34 = (m1)->_31*(m2)->_14+(m1)->_32*(m2)->_24+(m1)->_33*(m2)->_34+(m1)->_34*(m2)->_44,\
	(mOut)->_44 = (m1)->_41*(m2)->_14+(m1)->_42*(m2)->_24+(m1)->_43*(m2)->_34+(m1)->_44*(m2)->_44)

/////////////////////
//Plane

#define PlaneFromPointNormal(n, pt, plOut) ((plOut)->a = (n)->x, \
											(plOut)->b = (n)->y, \
											(plOut)->c = (n)->z, \
											(plOut)->d = -Vec3DDot(n, pt))

#define PlaneDotPt(pt, plane) ((pt)->x*(plane)->a + (pt)->y*(plane)->b + (pt)->z*(plane)->c + (plane)->d)
#define PlaneDotNormal(v, plane) ((v)->x*(plane)->a + (v)->y*(plane)->b + (v)->z*(plane)->c)

#define PlaneDiff(point,plane) (((plane)->type < 3 ? ((f32*)(*(point)))[(plane)->type] : (point)->x*(plane)->a + (point)->y*(plane)->b + (point)->z*(plane)->c) - (plane)->d)

/////////////////////////////////////////////
// Types
//

//2D Vector
typedef struct Vec2D
{
#ifdef __cplusplus
public:
    Vec2D() {};
    Vec2D( const f32 *f ) { x = f[0]; y = f[1]; }

	Vec2D( f32 cx, f32 cy ) { x = cx; y = cy; }

    // casting
	operator f32* () { return (f32*)&x; }
	operator const f32* () const { return (const f32*)&x; }

    // assignment operators
    Vec2D& operator += ( const Vec2D & v )
	{
		x += v.x;
		y += v.y;

		return *this;
	}

    Vec2D& operator -= ( const Vec2D & v )
	{
		x -= v.x;
		y -= v.y;

		return *this;
	}

    Vec2D& operator *= ( f32 f )
	{
		x *= f;
		y *= f;

		return *this;
	}

    Vec2D& operator /= ( f32 f )
	{
		f32 invF = 1.0f/f;
		x *= invF;
		y *= invF;

		return *this;
	}

    // unary operators
	Vec2D operator + () const { return *this; }
	Vec2D operator - () const { return Vec2D(-x, -y); }

    // binary operators
    Vec2D operator + ( const Vec2D & v ) const { return Vec2D(x+v.x, y+v.y); }
    Vec2D operator - ( const Vec2D & v ) const { return Vec2D(x-v.x, y-v.y); }
	Vec2D operator * ( const Vec2D & v ) const { return Vec2D(x*v.x, y*v.y); }

	Vec2D operator * ( f32 f ) const { return Vec2D(x*f, y*f); }
    Vec2D operator / ( f32 f ) const { f32 invF = 1.0f/f; return Vec2D(x*invF, y*invF); }

    friend Vec2D operator * ( f32, const Vec2D & );

	u8 operator == ( const Vec2D & v ) const { return x == v.x && y == v.y; }
	u8 operator != ( const Vec2D & v ) const { return x != v.x || y != v.y; }


public:
#endif
    f32 x, y;
} Vec2D, *PVec2D;

inline Vec2D operator * ( f32 f, const Vec2D & v ) { return Vec2D(f*v.x, f*v.y); }

//3D Vector
typedef struct Vec3D
{
#ifdef __cplusplus
public:
    Vec3D() {};
	Vec3D( const f32 * f ) { x = f[0]; y = f[1]; z = f[2]; }
	Vec3D( f32 cx, f32 cy, f32 cz ) { x = cx; y = cy; z = cz; }

    // casting
	operator f32* () { return (f32*)&x; }
	operator f32* () const { return (f32*)&x; }
	operator const f32* () const { return (const f32*)&x; }

    // assignment operators
    Vec3D& operator += ( const Vec3D & v )
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

    Vec3D& operator -= ( const Vec3D & v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

    Vec3D& operator *= ( f32 f )
	{
		x *= f;
		y *= f;
		z *= f;

		return *this;
	}

    Vec3D& operator /= ( f32 f )
	{
		f32 invF = 1.0f/f;
		x *= invF;
		y *= invF;
		z *= invF;

		return *this;
	}

    // unary operators
	Vec3D operator + () const { return *this; }
	Vec3D operator - () const { return Vec3D(-x,-y,-z); }

    // binary operators
	Vec3D operator + ( const Vec3D & v ) const { return Vec3D(x+v.x, y+v.y, z+v.z); }
    Vec3D operator - ( const Vec3D & v ) const { return Vec3D(x-v.x, y-v.y, z-v.z); }
	Vec3D operator * ( const Vec3D & v ) const { return Vec3D(x*v.x, y*v.y, z*v.z); }

    Vec3D operator * ( f32 f ) const { return Vec3D(x*f, y*f, z*f); }
    Vec3D operator / ( f32 f ) const { f32 invF = 1.0f/f; return Vec3D(x*invF, y*invF, z*invF); }

    friend Vec3D operator * ( f32, const Vec3D& );

	u8 operator == ( const Vec3D & v ) const { return x == v.x && y == v.y && z == v.z; }
	u8 operator != ( const Vec3D & v ) const { return x != v.x || y != v.y || z != v.z; }


public:
#endif
    f32 x, y, z;
} Vec3D, *PVec3D;

inline Vec3D operator * ( f32 f, const Vec3D & v ) { return Vec3D(f*v.x, f*v.y, f*v.z); }

//4D Vector
typedef struct Vec4D
{
#ifdef __cplusplus
public:
    Vec4D() {};
	Vec4D( const f32 * f ) { if(f) {x = f[0]; y = f[1]; z = f[2]; w = f[3];}else{x = y = z = w = 0;} }
	Vec4D( f32 cx, f32 cy, f32 cz, f32 cw ) { x = cx; y = cy; z = cz; w = cw; }

    // casting
	operator f32* () { return (f32 *)&x; }
    operator const f32* () const { return (const f32 *)&x; }

    // assignment operators
    Vec4D& operator += ( const Vec4D & v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;
	}

    Vec4D& operator -= ( const Vec4D & v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;
	}

    Vec4D& operator *= ( f32 f )
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;

		return *this;
	}

    Vec4D& operator /= ( f32 f )
	{
		f32 invF = 1.0f/f;

		x *= invF;
		y *= invF;
		z *= invF;
		w *= invF;

		return *this;
	}

    // unary operators
	Vec4D operator + () const { return *this; }
	Vec4D operator - () const { return Vec4D(-x,-y,-z,-w); }

    // binary operators
    Vec4D operator + ( const Vec4D & v) const { return Vec4D(x+v.x, y+v.y, z+v.z, w+v.w); }
    Vec4D operator - ( const Vec4D & v ) const { return Vec4D(x-v.x, y-v.y, z-v.z, w-v.w); }
	Vec4D operator * ( const Vec4D & v ) const { return Vec4D(x*v.x, y*v.y, z*v.z, w*v.w); }

    Vec4D operator * ( f32 f ) const { return Vec4D(x*f, y*f, z*f, w*f); }
    Vec4D operator / ( f32 f ) const { f32 invF = 1.0f/f; return Vec4D(x*invF, y*invF, z*invF, w*invF); }

    friend Vec4D operator * ( f32, const Vec4D& );

	u8 operator == ( const Vec4D & v ) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    u8 operator != ( const Vec4D & v ) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

public:
#endif
    f32 x, y, z, w;
} Vec4D, *PVec4D;

inline Vec4D operator * ( f32 f, const Vec4D & v ) { return Vec4D(f*v.x, f*v.y, f*v.z, f*v.w); }

//The Matrix(tm) ;)
typedef struct Matrix
{
#ifdef __cplusplus
public:
    Matrix() {};
	Matrix( const f32 * mtx ) { memcpy(m, mtx, sizeof(m)); }
    Matrix( f32 c_11, f32 c_21, f32 c_31, f32 c_41,
            f32 c_12, f32 c_22, f32 c_32, f32 c_42,
            f32 c_13, f32 c_23, f32 c_33, f32 c_43,
            f32 c_14, f32 c_24, f32 c_34, f32 c_44 )
	{
		_11 = c_11; _12 = c_12; _13 = c_13; _14 = c_14;
        _21 = c_21; _22 = c_22; _23 = c_23; _24 = c_24;
        _31 = c_31; _32 = c_32; _33 = c_33; _34 = c_34;
        _41 = c_41; _42 = c_42; _43 = c_43; _44 = c_44;
	}

    // access grants
	f32& operator () ( u8 Row, u8 Col ) { return m[Row][Col]; }
    f32  operator () ( u8 Row, u8 Col ) const { return m[Row][Col]; }

    // casting operators
	operator f32* () { return (f32 *) &_11; }
    operator const f32* () const { return (const f32 *) &_11; }

    // assignment operators
    Matrix& operator *= ( const Matrix & cm)
	{
		Matrix mOut;	

		MtxMultiply(this, &cm, &mOut);

		*this = mOut;
	    
		return *this;
	}
    
	Matrix& operator += ( const Matrix & cm )
	{
		_11 += cm._11; _12 += cm._12; _13 += cm._13; _14 += cm._14;
        _21 += cm._21; _22 += cm._22; _23 += cm._23; _24 += cm._24;
        _31 += cm._31; _32 += cm._32; _33 += cm._33; _34 += cm._34;
        _41 += cm._41; _42 += cm._42; _43 += cm._43; _44 += cm._44;

		return *this;
	}
    
	Matrix& operator -= ( const Matrix & cm )
	{
		_11 -= cm._11; _12 -= cm._12; _13 -= cm._13; _14 -= cm._14;
        _21 -= cm._21; _22 -= cm._22; _23 -= cm._23; _24 -= cm._24;
        _31 -= cm._31; _32 -= cm._32; _33 -= cm._33; _34 -= cm._34;
        _41 -= cm._41; _42 -= cm._42; _43 -= cm._43; _44 -= cm._44;

		return *this;
	}

    Matrix& operator *= ( f32 f )
	{
		_11 *= f; _12 *= f; _13 *= f; _14 *= f;
        _21 *= f; _22 *= f; _23 *= f; _24 *= f;
        _31 *= f; _32 *= f; _33 *= f; _34 *= f;
        _41 *= f; _42 *= f; _43 *= f; _44 *= f;

		return *this;
	}

    Matrix& operator /= ( f32 f )
	{
		f32 invF = 1.0f/f;

		_11 *= invF; _12 *= invF; _13 *= invF; _14 *= invF;
        _21 *= invF; _22 *= invF; _23 *= invF; _24 *= invF;
        _31 *= invF; _32 *= invF; _33 *= invF; _34 *= invF;
        _41 *= invF; _42 *= invF; _43 *= invF; _44 *= invF;

		return *this;
	}

    // unary operators
	Matrix operator + () const { return *this; }
    Matrix operator - () const
	{
		return Matrix(_11, _12, _13, _14,
					  _21, _22, _23, _24,
					  _31, _32, _33, _34,
					  _41, _42, _43, _44);
	}

    // binary operators
    Matrix operator * ( const Matrix & cm) const
	{
		Matrix matT;
		MtxMultiply(this, &cm, &matT);
		return matT;
	}
    
	Matrix operator + ( const Matrix & cm ) const
	{
		return Matrix(_11+cm._11, _12+cm._11, _13+cm._13, _14+cm._14,
					  _21+cm._21, _22+cm._21, _23+cm._23, _24+cm._24,
					  _31+cm._31, _32+cm._31, _33+cm._33, _34+cm._34,
					  _41+cm._41, _42+cm._41, _43+cm._43, _44+cm._44);
	}

    Matrix operator - ( const Matrix & cm ) const
	{
		return Matrix(_11-cm._11, _12-cm._11, _13-cm._13, _14-cm._14,
					  _21-cm._21, _22-cm._21, _23-cm._23, _24-cm._24,
					  _31-cm._31, _32-cm._31, _33-cm._33, _34-cm._34,
					  _41-cm._41, _42-cm._41, _43-cm._43, _44-cm._44);
	}

    Matrix operator * ( f32 f ) const
	{
		return Matrix(_11*f, _12*f, _13*f, _14*f,
					  _21*f, _22*f, _23*f, _24*f,
					  _31*f, _32*f, _33*f, _34*f,
					  _41*f, _42*f, _43*f, _44*f);
	}

    Matrix operator / ( f32 f ) const
	{
		f32 invF = 1.0f/f;

		return Matrix(_11*invF, _12*invF, _13*invF, _14*invF,
					  _21*invF, _22*invF, _23*invF, _24*invF,
					  _31*invF, _32*invF, _33*invF, _34*invF,
					  _41*invF, _42*invF, _43*invF, _44*invF);
	}

    friend Matrix operator * ( f32, const Matrix& );

	u8 operator == ( const Matrix & cm ) const { return 0 == memcmp(this, &cm, sizeof(Matrix)); }
    u8 operator != ( const Matrix & cm ) const { return 0 != memcmp(this, &cm, sizeof(Matrix)); }

public:
#endif
	union {
        struct {
            f32        _11, _12, _13, _14;
            f32        _21, _22, _23, _24;
            f32        _31, _32, _33, _34;
            f32        _41, _42, _43, _44;

        };

        f32 m[4][4];
    };
} Matrix, *PMatrix;

inline Matrix operator * ( f32 f, const Matrix & mtx )
{
    return Matrix(mtx._11*f, mtx._12*f, mtx._13*f, mtx._14*f,
				  mtx._21*f, mtx._22*f, mtx._23*f, mtx._24*f,
				  mtx._31*f, mtx._32*f, mtx._33*f, mtx._34*f,
				  mtx._41*f, mtx._42*f, mtx._43*f, mtx._44*f);
}

//Quaternion
typedef struct Quaternion
{
#ifdef __cplusplus
public:
    Quaternion() {}
	Quaternion( const f32 * f ) { x = f[0]; y = f[1]; z = f[2]; w = f[3]; }
	Quaternion( f32 cx, f32 cy, f32 cz, f32 cw ) { x = cx; y = cy; z = cz; w = cw; }

    // casting
	operator f32* () { return (f32 *)&x; }
    operator const f32* () const { return (const f32 *)&x; }

    // assignment operators
    Quaternion& operator += ( const Quaternion & cq )
	{
		x += cq.x;
		y += cq.y;
		z += cq.z;
		w += cq.w;

		return *this;
	}

    Quaternion& operator -= ( const Quaternion & cq )
	{
		x -= cq.x;
		y -= cq.y;
		z -= cq.z;
		w -= cq.w;

		return *this;
	}

    Quaternion& operator *= ( const Quaternion & cq )
	{
		Quaternion rQ;

		QuatMult(this, &cq, &rQ);

		*this = rQ;

		return *this;
	}

    Quaternion& operator *= ( f32 f )
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;

		return *this;
	}

    Quaternion& operator /= ( f32 f )
	{
		f32 invF = 1.0f/f;
		x *= invF;
		y *= invF;
		z *= invF;
		w *= invF;

		return *this;
	}

    // unary operators
	Quaternion  operator + () const { return *this; }
	Quaternion  operator - () const { return Quaternion(-x,-y,-z,-w); }

    // binary operators
	Quaternion operator + ( const Quaternion& cq ) const { return Quaternion(x+cq.x, y+cq.y, z+cq.z, w+cq.w); }

    Quaternion operator - ( const Quaternion& cq ) const { return Quaternion(x-cq.x, y-cq.y, z-cq.z, w-cq.w); }
	
    Quaternion operator * ( const Quaternion & cq ) const
	{
		Quaternion rQ;

		QuatMult(this, &cq, &rQ);
		
		return rQ;
	}

    Quaternion operator * ( f32 f ) const { return Quaternion(x*f, y*f, z*f, w*f); }

    Quaternion operator / ( f32 f ) const { f32 invF = 1.0f; return Quaternion(x*invF, y*invF, z*invF, w*invF); }

    friend Quaternion operator * (f32, const Quaternion& );

	u8 operator == ( const Quaternion & cq ) const { return x == cq.x && y == cq.y && z == cq.z && w == cq.w; }
    u8 operator != ( const Quaternion & cq ) const { return x != cq.x || y != cq.y || z != cq.z || w != cq.w; }

#endif //__cplusplus
    f32 x, y, z, w;
} Quaternion, *PQuaternion;

inline Quaternion operator * (f32 f, const Quaternion & cq )
{
	return Quaternion(cq.x*f, cq.y*f, cq.z*f, cq.w*f);
}

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3 is not axial
#define	PLANE_NONAXIAL	3

//Plane
typedef struct Plane
{
#ifdef __cplusplus
public:
    Plane() {}
	Plane( const f32 * f) { a = f[0]; b = f[1]; c = f[2]; d = f[3]; }
	Plane( f32 ca, f32 cb, f32 cc, f32 cd ) { a = ca; b = cb; c = cc; d = cd; }

    // casting
	operator f32* () { return (f32 *)&a; }
    operator const f32* () const { return (const f32 *)&a; }

    // unary operators
	Plane operator + () const { return *this; }
	Plane operator - () const { return Plane(-a,-b,-c,-d); }

    // binary operators
	u8 operator == ( const Plane & cp) const { return a == cp.a && b == cp.b && c == cp.c && d == cp.d; }
    u8 operator != ( const Plane & cp) const { return a != cp.a || b != cp.b || c != cp.c || d != cp.d; }

#endif //__cplusplus
    f32 a, b, c, d;

	s16	type;			// for fast side tests
	s16	signbits;		// signx + (signy<<1) + (signz<<1)
} Plane, *PPlane;

//structs
typedef struct _Ray {
	Vec3D			p0;		//point
	Vec3D			v0;		//dir
} Ray3D;

typedef struct {
	Vec3D	origin;	//min
	Vec3D	extent;	//max
} Box3D;

typedef struct _LineSeg3D {
	Vec3D		p0;
	Vec3D		p1;
} LineSeg3D;

typedef struct _Sphere3D {
	Vec3D		c;			//center
	f64			r;			//radius
} Sphere3D;

#endif