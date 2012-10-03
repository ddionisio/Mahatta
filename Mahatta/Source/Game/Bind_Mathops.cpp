#include "game_lua.h"
#include "game_lua_i.h"

using namespace Lua;

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% COMMON %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

template<typename T> static bool _eqT (lua_State * L)
{
	lua_pushboolean(L, *_pT<T>(L, 1, "==") == *_pT<T>(L, 2, "=="));

	return 1;
}

template<typename T> static int _addT (lua_State * L)
{
	return _copyT(L, *_pT<T>(L, 1, "+") + *_pT<T>(L, 2, "+"));
}

template<typename T> static int _subT (lua_State * L)
{
	return _copyT(L, *_pT<T>(L, 1, "-") - *_pT<T>(L, 2, "-"));
}

template<typename T> static bool _mulT (lua_State * L)
{
	if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2))	return false;
	
	// One numerical argument -> Scaling
	std::pair<int, int> p(1, 2);

	if (lua_isnumber(L, 2)) std::swap(p.first, p.second);

	_copyT(L, *_pT<T>(L, p.second) * lua_tonumber(L, p.first));

	return true;
}

template<typename T> static int _divT (lua_State * L)
{
	if (!lua_isnumber(L, 2)) luaL_error(L, "%s /: non-number arg #2", _typeT<T>());

	return _copyT(L, *_pT<T>(L, 1) / lua_tonumber(L, 2));
}

template<typename T> static int _unmT (lua_State * L)
{
	return _copyT(L, -*_pT<T>(L, 1));
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% COMPLEX %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

//
// Complex/RComplex utlities
//
template<> char const * Lua::_typeT<std::complex<float> > (void)
{
	return "Complex";
}

template<> char const * Lua::_rtypeT<std::complex<float> > (void)
{
	return "RComplex";
}

std::complex<float> * Lua::Complex_ (lua_State * L, int index, char const * what)
{
	return _pT<std::complex<float> >(L, index, what);
}

static int Complex_new (lua_State * L, std::complex<float> & c)
{
	return _copyT(L, c);
}

//
// Complex/RComplex metamethods
//
static int Complex_eq (lua_State * L)
{
	return _eqT<std::complex<float> >(L);
}

static int Complex_add (lua_State * L)
{
	return _addT<std::complex<float> >(L);
}

static int Complex_sub (lua_State * L)
{
	return _subT<std::complex<float> >(L);
}

static int Complex_mul (lua_State * L)
{
	if (!_mulT<std::complex<float> >(L))	// Two complex numbers -> Complex number multiplication
	{
		Complex_new(L, *Complex_(L, 1, "*") * *Complex_(L, 2, "*"));// C1 * C2
	}

	return 1;
}

static int Complex_div (lua_State * L)
{
	return _divT<std::complex<float> >(L);
}

static int Complex_unm (lua_State * L)
{
	return _unmT<std::complex<float> >(L);
}

static int Complex_len (lua_State * L)
{
	lua_pushnumber(L, std::abs(*Complex_(L, 1)));

	return 1;
}

//
// Complex/RComplex methods
//
static int Complex_Dup (lua_State * L)
{
	return Complex_new(L, *Complex_(L, 1));
}

//
// Complex getters
//
static int ComplexGet_r (lua_State * L)
{
	lua_pushnumber(L, Complex_(L, 1)->real());

	return 1;
}

static int ComplexGet_i (lua_State * L)
{
	lua_pushnumber(L, Complex_(L, 1)->imag());

	return 1;
}

//
// Complex setters
//
static int ComplexSet_r (lua_State * L)
{
	std::complex<float> * pC = Complex_(L, 1);

	*pC += std::complex<float>(F(L, 2) - pC->real(), 0.0f);

	return 0;
}

static int ComplexSet_i (lua_State * L)
{
	std::complex<float> * pC = Complex_(L, 1);

	*pC += std::complex<float>(0.0f, F(L, 2) - pC->imag());

	return 0;
}

//
// Complex new
//
static int ComplexNew_Default (lua_State * L)
{
	new (UD(L, 1)) std::complex<float>;

	return 0;
}

static int ComplexNew_Float2 (lua_State * L)
{
	float real = F(L, 2), imag = F(L, 3);

	std::complex<float> cpx(real, imag);

	if (lua_toboolean(L, 4)) cpx = std::polar(real, imag);

	*Complex_(L, 1) = cpx;

	return 0;
}

static int ComplexNew_C (lua_State * L)
{
	*Complex_(L, 1) = *Complex_(L, 2, "new Complex");

	return 0;
}

//
// RComplex new
//
static int RComplex_New (lua_State * L)
{
	*(std::complex<float>**)UD(L, 1) = Complex_(L, 2, "new RComplex");

	return 0;
}

///
/// Complex utilities
///
static int _Complex (lua_State * L)
{
	return GenericNew(L, "Complex", 2);
}

/// @brief Defines the complex class
/// @param L Lua state
static void define_Complex (lua_State * L)
{
	// Install complex coordinate members.
	luaL_reg getters[] = {
		{ "r", ComplexGet_r },
		{ "i", ComplexGet_i },
		{ 0, 0 }
	}, setters[] = {
		{ "r", ComplexSet_r },
		{ "i", ComplexSet_i },
		{ 0, 0 }
	};

	MembersBind(L, getters, setters, 0, 0, eThis, 0, false);// __index, __newindex

	// Supply an overloaded new.
	Overload ComplexCons(L, 3);	// __index, __newindex, G
		ComplexCons.AddDef(ComplexNew_Default, "Complex", "nil", "nil");
		ComplexCons.AddDef(ComplexNew_Float2, "Complex", "number", "number");
		ComplexCons.AddDef(ComplexNew_C, "Complex", "userdata", "nil");
		ComplexCons.AddDef(ComplexNew_C, "Complex", "Complex", "nil");

	// Define the complex number.
	luaL_reg funcs[] = {
		{ "__eq", Complex_eq },
		{ "__add", Complex_add },
		{ "__sub", Complex_sub },
		{ "__mul", Complex_mul },
		{ "__div", Complex_div },
		{ "__unm", Complex_unm },
		{ "__len", Complex_len },
		{ "Dup", Complex_Dup },
		{ 0, 0 }
	};

	char const * closures[] = { "__index", "__newindex", 0 };

	class_Define(L, "Complex", funcs, closures, 0, sizeof(std::complex<float>));// stack clear

	// Install reference complex number coordinate members.
	MembersBind(L, getters, setters, 0, 0, ePointerTo, 0, false);	// __index, __newindex

	// Define the reference complex number.
	class_Define(L, "RComplex", funcs, closures, RComplex_New, "Complex", sizeof(std::complex<float>*));// stack clear

	// Install a helper function.
	lua_register(L, "Complex", _Complex);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% MATRIX %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

//
// Matrix/RMatrix utlities
//
template<> char const * Lua::_typeT<Matrix> (void)
{
	return "Matrix";
}

template<> char const * Lua::_rtypeT<Matrix> (void)
{
	return "RMatrix";
}

Matrix * Lua::Matrix_ (lua_State * L, int index, char const * what)
{
	return _pT<Matrix>(L, index, what);
}

static int Matrix_new (lua_State * L, Matrix & m)
{
	return _copyT(L, m);
}

//
// Matrix/RMatrix metamethods
//
static int Matrix_eq (lua_State * L)
{
	return _eqT<Matrix>(L);
}

static int Matrix_add (lua_State * L)
{
	return _addT<Matrix>(L);
}

static int Matrix_sub (lua_State * L)
{
	return _subT<Matrix>(L);
}

static int Matrix_mul (lua_State * L)
{
	if (!_mulT<Matrix>(L)) // One numerical argument -> Matrix scaling
	{
		Matrix_new(L, *Matrix_(L, 1) * *Matrix_(L, 2));	// M1 * M2
	}

	return 1;
}

static int Matrix_div (lua_State * L)
{
	return _divT<Matrix>(L);
}

static int Matrix_unm (lua_State * L)
{
	return _unmT<Matrix>(L);
}

static int Matrix_len (lua_State * L)
{
	Matrix * pM = Matrix_(L, 1);

	lua_pushnumber(L, g_gi.MtxDeterminant(pM));

	return 1;
}

//
// Matrix/RMatrix methods
//
static int Matrix_Dup (lua_State * L)
{
	return Matrix_new(L, *Matrix_(L, 1));
}

//
// Matrix new
//
static int MatrixNew_Default (lua_State * L)
{
	new (UD(L, 1)) Matrix;

	return 0;
}

static int MatrixNew_Vec3D_3 (lua_State * L)
{
	Vec3D * pU = Vec3D_(L, 2), * pV = Vec3D_(L, 3), * pW = Vec3D_(L, 4);

	Matrix mtx;

	mtx._11 = pU->x, mtx._12 = pU->y, mtx._13 = pU->z, mtx._14 = 0.0f;
	mtx._21 = pV->x, mtx._22 = pV->y, mtx._23 = pV->z, mtx._24 = 0.0f;
	mtx._31 = pW->x, mtx._32 = pW->y, mtx._33 = pW->z, mtx._34 = 0.0f;
	mtx._41 =  0.0f, mtx._42 =  0.0f, mtx._43 =  0.0f, mtx._44 = 1.0f;

	*Matrix_(L, 1) = mtx;

	return 0;
}

static int MatrixNew_Vec3D_4 (lua_State * L)
{
	Vec3D * pU = Vec3D_(L, 2), * pV = Vec3D_(L, 3), * pW = Vec3D_(L, 4), * pT = Vec3D_(L, 5);

	Matrix mtx;

	mtx._11 = pU->x, mtx._12 = pV->x, mtx._13 = pW->x, mtx._14 = pT->x;
	mtx._21 = pU->y, mtx._22 = pV->y, mtx._23 = pW->y, mtx._24 = pT->y;
	mtx._31 = pU->z, mtx._32 = pV->z, mtx._33 = pW->z, mtx._34 = pT->z;
	mtx._41 =  0.0f, mtx._42 =  0.0f, mtx._43 =  0.0f, mtx._44 =  1.0f;

	*Matrix_(L, 1) = mtx;

	return 0;
}

static int MatrixNew_M (lua_State * L)
{
	*Matrix_(L, 1) = *Matrix_(L, 2, "new Matrix");

	return 0;
}

//
// RMatrix new
//
static int RMatrix_New (lua_State * L)
{
	*(Matrix**)UD(L, 1) = Matrix_(L, 2, "new RMatrix");

	return 0;
}

///
/// Matrix utilities
///
static int _Matrix (lua_State * L)
{
	return GenericNew(L, "Matrix", 4);
}

/// @brief Defines the matrix class
/// @param L Lua state
static void define_Matrix (lua_State * L)
{
	// Install matrix coordinate members.
	Member_Reg members[16];

	#define MtxEntry(e) members[(e / 10 - 1) * 4 + (e % 10) - 1].Set(offsetof(Matrix, _##e), "_"#e, Member_Reg::eFloat)

	MtxEntry(11);	MtxEntry(12);	MtxEntry(13);	MtxEntry(14);
	MtxEntry(21);	MtxEntry(22);	MtxEntry(23);	MtxEntry(24);
	MtxEntry(31);	MtxEntry(32);	MtxEntry(33);	MtxEntry(34);
	MtxEntry(41);	MtxEntry(42);	MtxEntry(43);	MtxEntry(44);

	#undef MtxEntry

	MembersBind(L, 0, 0, members, 16, eThis, 0, false);	// __index, __newindex

	// Supply an overloaded new.
	Overload MatrixCons(L, 5);	// __index, __newindex, G
		MatrixCons.AddDef(MatrixNew_Default, "Matrix", "nil", "nil", "nil", "nil");
		MatrixCons.AddDef(MatrixNew_Vec3D_3, "Matrix", "Vec3D", "Vec3D", "Vec3D", "nil");
		MatrixCons.AddDef(MatrixNew_Vec3D_4, "Matrix", "Vec3D", "Vec3D", "Vec3D", "Vec3D");
		MatrixCons.AddDef(MatrixNew_M, "Matrix", "userdata", "nil", "nil", "nil");
		MatrixCons.AddDef(MatrixNew_M, "Matrix", "Matrix", "nil", "nil", "nil");

	// Define the matrix.
	luaL_reg funcs[] = {
		{ "__eq", Matrix_eq },
		{ "__add", Matrix_add },
		{ "__sub", Matrix_sub },
		{ "__mul", Matrix_mul },
		{ "__div", Matrix_div },
		{ "__unm", Matrix_unm },
		{ "__len", Matrix_len },
		{ "Dup", Matrix_Dup },
		{ 0, 0 }
	};

	char const * closures[] = { "__index", "__newindex", 0 };

	class_Define(L, "Matrix", funcs, closures, 0, sizeof(Matrix));	// stack clear

	// Install reference vector coordinate members.
	MembersBind(L, 0, 0, members, 16, ePointerTo, 0, false);// __index, __newindex

	// Define the reference matrix.
	class_Define(L, "RMatrix", funcs, closures, RMatrix_New, "Matrix", sizeof(Matrix*));// stack clear

	// Install a helper function.
	lua_register(L, "Matrix", _Matrix);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% QUATERNION %%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

/// @brief Defines the quaternion class
/// @param L Lua state
static void define_Quaternion (lua_State * L)
{

}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% VEC3D %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

//
// Vec3D/RVec3D utlities
//
template<> char const * Lua::_typeT<Vec3D> (void)
{
	return "Vec3D";
}

template<> char const * Lua::_rtypeT<Vec3D> (void)
{
	return "RVec3D";
}

Vec3D * Lua::Vec3D_ (lua_State * L, int index, char const * what)
{
	return _pT<Vec3D>(L, index, what);
}

static int Vec3D_new (lua_State * L, Vec3D & v)
{
	return _copyT(L, v);
}

//
// Vec3D/RVec3D metamethods
//
static int Vec3D_eq (lua_State * L)
{
	return _eqT<Vec3D>(L);
}

static int Vec3D_add (lua_State * L)
{
	return _addT<Vec3D>(L);
}

static int Vec3D_sub (lua_State * L)
{
	return _subT<Vec3D>(L);
}

static int Vec3D_mul (lua_State * L)
{
	if (!_mulT<Vec3D>(L))	// One numerical argument -> Vector scaling
	{
		Vec3D * pV1 = Vec3D_(L, 1, "* (V . W)"), * pV2 = Vec3D_(L, 2, "* (V . W)");

		lua_pushnumber(L, Vec3DDot(pV1, pV2));	// v1 . v2
	}

	return 1;
}

static int Vec3D_div (lua_State * L)
{
	return _divT<Vec3D>(L);
}

static int Vec3D_pow (lua_State * L)
{
	Vec3D * pV1 = Vec3D_(L, 1, "^"), * pV2 = Vec3D_(L, 2, "^");

	Vec3D cv;	Vec3DCross(pV1, pV2, &cv);

	return Vec3D_new(L, cv);
}

static int Vec3D_unm (lua_State * L)
{
	return _unmT<Vec3D>(L);
}

static int Vec3D_len (lua_State * L)
{
	Vec3D * pV = Vec3D_(L, 1);

	lua_pushnumber(L, Vec3DLen(pV));

	return 1;
}

//
// Vec3D/RVec3D methods
//
static int Vec3D_Dup (lua_State * L)
{
	return Vec3D_new(L, *Vec3D_(L, 1));
}

static int Vec3D_LenSQXZ (lua_State * L)
{
	Vec3D * pV = Vec3D_(L, 1);

	lua_pushnumber(L, pV->x * pV->x + pV->z * pV->z);

	return 1;
}

static int Vec3D_LenXZ (lua_State * L)
{
	Vec3D * pV = Vec3D_(L, 1);

	lua_pushnumber(L, sqrtf(pV->x * pV->x + pV->z * pV->z));

	return 1;
}

static int Vec3D_NormalXZ (lua_State * L)
{
	Vec3D * pV = Vec3D_(L, 1);

	return Vec3D_new(L, Vec3D(pV->x, 0.0f, pV->z) / sqrtf(pV->x * pV->x + pV->z * pV->z));
}

static int Vec3D_ScaleToXZ (lua_State * L)
{
	Vec3D * pV = Vec3D_(L, 1);

	return Vec3D_new(L, Vec3D(pV->x, 0.0f, pV->z) * F(L, 2) / sqrtf(pV->x * pV->x + pV->z * pV->z));
}

static int Vec3D_XZ (lua_State * L)
{
	Vec3D * pV = Vec3D_(L, 1);

	return Vec3D_new(L, Vec3D(pV->x, 0.0f, pV->z));
}

//
// Vec3D new
//
static int Vec3DNew_Default (lua_State * L)
{
	new (UD(L, 1)) Vec3D;

	return 0;
}

static int Vec3DNew_Float3 (lua_State * L)
{
	*Vec3D_(L, 1) = Vec3D(F(L, 2), F(L, 3), F(L, 4));

	return 0;
}

static int Vec3DNew_V (lua_State * L)
{
	*Vec3D_(L, 1) = *Vec3D_(L, 2, "new Vec3D");

	return 0;
}

//
// RVec3D new
//
static int RVec3D_New (lua_State * L)
{
	*(Vec3D**)UD(L, 1) = Vec3D_(L, 2, "new RVec3D");

	return 0;
}

///
/// Vec3D utilities
///
static int _Vec3D (lua_State * L)
{
	return GenericNew(L, "Vec3D", 3);
}

/// @brief Defines the vector classes
/// @param L Lua state
static void define_Vector (lua_State * L)
{
	// Install vector coordinate members.
	Member_Reg members[3];

	members[0].Set(offsetof(Vec3D, x), "x", Member_Reg::eFloat);
	members[1].Set(offsetof(Vec3D, y), "y", Member_Reg::eFloat);
	members[2].Set(offsetof(Vec3D, z), "z", Member_Reg::eFloat);

	MembersBind(L, 0, 0, members, 3, eThis, 0, false);// __index, __newindex

	// Supply an overloaded new.
	Overload Vec3DCons(L, 4);	// __index, __newindex, G
		Vec3DCons.AddDef(Vec3DNew_Default, "Vec3D", "nil", "nil", "nil");
		Vec3DCons.AddDef(Vec3DNew_Float3, "Vec3D", "number", "number", "number");
		Vec3DCons.AddDef(Vec3DNew_V, "Vec3D", "userdata", "nil", "nil");
		Vec3DCons.AddDef(Vec3DNew_V, "Vec3D", "Vec3D", "nil", "nil");

	// Define the vector.
	luaL_reg funcs[] = {
		{ "__eq", Vec3D_eq },
		{ "__add", Vec3D_add },
		{ "__sub", Vec3D_sub },
		{ "__mul", Vec3D_mul },
		{ "__div", Vec3D_div },
		{ "__pow", Vec3D_pow },
		{ "__unm", Vec3D_unm },
		{ "__len", Vec3D_len },
		{ "Dup", Vec3D_Dup },
		{ "LenSQXZ", Vec3D_LenSQXZ },
		{ "LenXZ", Vec3D_LenXZ },
		{ "NormalXZ", Vec3D_NormalXZ },
		{ "ScaleToXZ", Vec3D_ScaleToXZ },
		{ "XZ", Vec3D_XZ },
		{ 0, 0 }
	};

	char const * closures[] = { "__index", "__newindex", 0 };

	class_Define(L, "Vec3D", funcs, closures, 0, sizeof(Vec3D));// stack clear

	// Install reference vector coordinate members.
	MembersBind(L, 0, 0, members, 3, ePointerTo, 0, false);	// __index, __newindex

	// Define the reference vector.
	class_Define(L, "RVec3D", funcs, closures, RVec3D_New, "Vec3D", sizeof(Vec3D*));// stack clear

	// Install a helper function.
	lua_register(L, "Vec3D", _Vec3D);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%% NUMERICAL %%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static bool _BQF (float a, float b, float c, float & t)
{
	if (fabsf(a) >= 1e-6f)
	{
		float term = b * b - a * c;

		if (term >= 0.0f)
		{
			float discriminant = sqrtf(term), t = (-b - discriminant) / a;

			if (t < 0.0f) t = (-b + discriminant) / a;

			return t >= 0.0f;
		}
	}

	return false;
}

static int BQF (lua_State * L)
{
	float t;

	if (_BQF(F(L, 1), F(L, 2), F(L, 3), t))
	{
		lua_pushnumber(L, t);	// t

		return 1;
	}

	return 0;
}

static int Partition (lua_State * L)
{
	Uint dim = uI(L, 1), count = uI(L, 2) + 1, offset = 0;

	lua_createtable(L, count, 0);	// {}

	for (Uint index = 1; index <= count; ++index)
	{
		Uint oldo = offset;

		offset = index * dim / count;

		lua_pushinteger(L, offset - oldo);	// { ... }, d
		lua_rawseti(L, 1, index);	// { ..., d }
	}

	return 1;
}

static int SpheresIntersect (lua_State * L)
{
	// Compute the coefficients of the sphere collision equation, which is a binary quadratic
	// form. Check whether the spheres will intersect in this time step.
	Vec3D center[2], vel[2];

	float radius[2];

	for (int index = 1; index <= 2; ++index)
	{
		lua_getfield(L, index, "center");	// s1, s2, maxT, center
		lua_getfield(L, index, "vel");	// s1, s2, maxT, center, vel
		lua_getfield(L, index, "radius");	// s1, s2, maxT, center, vel, radius

		center[index - 1] = *Vec3D_(L, -3);
		vel[index - 1] = *Vec3D_(L, -2);
		radius[index - 1] = F(L, -1);

		lua_pop(L, 3);	// s1, s2, maxT
	}

	float rSum = radius[1] + radius[2], t;

	Vec3D dC = center[1] - center[2], dV = vel[1] - vel[2];

	if (_BQF(Vec3DLenSQ(&dV), Vec3DDot(&dV, &dC), Vec3DLenSQ(&dC) - rSum * rSum, t) && t < F(L, 3))
	{
		lua_pushnumber(L, t);	// s1, s2, maxT, t

		// Compute the vector between the new centers, following it to the collision point.
		// Normalize it to acquire the collision normal.
		dC = radius[1] / rSum * (dC + t * dV);

		class_New(L, "Vec3D", "u", &(center[1] + t * vel[1] - dC));	// s1, s2, maxT, t, (x y z)
		class_New(L, "Vec3D", "u", &(dC / radius[1]));	// s1, s2, maxT, t, (x y z), (nx ny nz)

		return 3;
	}

	return 0;
}

/// @brief Binds math operation utilities to the Lua scripting system
void LuaI::open_mathops (lua_State * L)
{
	// Load math operations.
	static const luaL_reg MathopsFuncs[] = {
		{ "BQF", BQF },
		{ "Partition", Partition },
		{ "SpheresIntersect", SpheresIntersect },
		{ 0, 0 }
	};

	luaL_register(L, "mathops", MathopsFuncs);
	lua_pop(L, 1);

	// Define math classes.
	define_Complex(L);
	define_Matrix(L);
	define_Quaternion(L);
	define_Vector(L);
}