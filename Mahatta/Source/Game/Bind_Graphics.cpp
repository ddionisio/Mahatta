#include "game_lua.h"
#include "game_lua_i.h"
#include "game_local.h"

using namespace Lua;

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%% hFNT %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int GetStrSize (lua_State * L)
{
	iPoint size;	_hT<hFNT>(L, 1)->GetStrSize(WS_(L, 2)->c_str(), &size);

	lua_pushinteger(L, size.x);
	lua_pushinteger(L, size.y);

	return 2;
}

static int TextBox (lua_State * L)
{
	iRect rect;

	lua_getfield(L, 4, "bottom");	rect.bottom = uI(L, -1);
	lua_getfield(L, 4, "left");		rect.left = uI(L, -1);
	lua_getfield(L, 4, "right");	rect.right = uI(L, -1);
	lua_getfield(L, 4, "top");		rect.top = uI(L, -1);

	_hT<hFNT>(L, 1)->TextBox(WS_(L, 2)->c_str(), sI(L, 3), &rect, uI(L, 5), uI(L, 6));

	return 0;
}

//
// hFNT metamethods
//
static int hFNT_gc (lua_State * L)
{
	return _gcT<hFNT>(L);
}

//
// hFNT new
//
static int hFNT_New_WS (lua_State * L)
{
	*(hFNT*)UD(L, 1) = g_gi.FontCreate(WS_(L, 2)->c_str(), sI(L, 3), uI(L, 4));

	return 0;
}

static int hFNT_New_F (lua_State * L)
{
	return _refT<hFNT>(L);
}

///
/// hFNT utilities
///
static int _hFNT (lua_State * L)
{
	return GenericNew(L, "hFNT", 3);
}

/// @brief Defines the hFNT class
/// @param L Lua state
static void define_hFNT (lua_State * L)
{
	// Supply an overloaded new.
	Overload hFNT_Cons(L, 4);	// G
		hFNT_Cons.AddDef(hFNT_New_WS, "hFNT", "WS", "number", "number");
		hFNT_Cons.AddDef(hFNT_New_F, "hFNT", "userdata", "nil", "nil");
		hFNT_Cons.AddDef(hFNT_New_F, "hFNT", "hFNT", "nil", "nil");

	// Define the hFNT.
	luaL_reg funcs[] = {
		{ "GetStrSize", GetStrSize },
		{ "__gc", hFNT_gc },
		{ "TextBox", TextBox },
		{ 0, 0 }
	};
	
	class_Define(L, "hFNT", funcs, 0, sizeof(hFNT));

	// Install a helper function.
	lua_register(L, "hFNT", _hFNT);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%% hMDL %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

//
// hMDL metamethods
//
static int hMDL_gc (lua_State * L)
{
	return _gcT<hMDL>(L);
}

//
// hMDL new
//
static int hMDL_New_WS (lua_State * L)
{
	*(hMDL*)UD(L, 1) = g_gi.MDLLoad(WS_(L, 2)->c_str());

	return 0;
}

static int hMDL_New_M (lua_State * L)
{
	return _refT<hMDL>(L);
}

///
/// hMDL utilities
///
static int _hMDL (lua_State * L)
{
	return GenericNew(L, "hMDL", 1);
}

/// @brief Defines the hMDL class
/// @param L Lua state
static void define_hMDL (lua_State * L)
{
	// Supply an overloaded new.
	Overload hMDL_Cons(L, 2);	// G
		hMDL_Cons.AddDef(hMDL_New_WS, "hMDL", "WS");
		hMDL_Cons.AddDef(hMDL_New_M, "hMDL", "userdata");
		hMDL_Cons.AddDef(hMDL_New_M, "hMDL", "hMDL");

	// Define the hMDL.
	luaL_reg funcs[] = {
		{ "__gc", hMDL_gc },
		{ 0, 0 }
	};
	
	class_Define(L, "hMDL", funcs, 0, sizeof(hMDL));

	// Install a helper function.
	lua_register(L, "hMDL", _hMDL);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%% hOBJ %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int Display (lua_State * L)
{
	_hT<hOBJ>(L, 1)->Display(0, 0);	// TODO: support effects

	return 0;
}

static int MixerBlendCycle (lua_State * L)
{
	_hT<hOBJ>(L, 1)->MixerBlendCycle(sI(L, 2), F(L, 3), F(L, 4));

	return 0;
}

static int SetAxisMtx (lua_State * L)
{
	_hT<hOBJ>(L, 1)->SetAxisMtx(Matrix_(L, 2, "SetAxisMtx"));

	return 0;
}

static int Translate (lua_State * L)
{
	_hT<hOBJ>(L, 1)->Translate(*Vec3D_(L, 2, "Translate"), B(L, 3));

	return 0;
}

static int Update (lua_State * L)
{
	_hT<hOBJ>(L, 1)->Update();

	return 0;
}

//
// hOBJ metamethods
//
static int hOBJ_gc (lua_State * L)
{
	return _gcT<hOBJ>(L);
}

//
// hOBJ new
//
static int hOBJ_New_hMDL (lua_State * L)
{
	*(hOBJ*)UD(L, 1) = g_gi.OBJCreate(_hT<hMDL>(L, 2));

	return 0;
}

static int hOBJ_New_O (lua_State * L)
{
	return _refT<hOBJ>(L);
}

///
/// hOBJ utilities
///
static int _hOBJ (lua_State * L)
{
	return GenericNew(L, "hOBJ", 1);
}

/// @brief Defines the hOBJ class
/// @param L Lua state
static void define_hOBJ (lua_State * L)
{
	// Supply an overloaded new.
	Overload hOBJ_Cons(L, 2);	// G
		hOBJ_Cons.AddDef(hOBJ_New_hMDL, "hOBJ", "hMDL");
		hOBJ_Cons.AddDef(hOBJ_New_O, "hOBJ", "userdata");
		hOBJ_Cons.AddDef(hOBJ_New_O, "hOBJ", "hOBJ");

	// Define the object.
	luaL_reg funcs[] = {
		{ "Display", Display },
		{ "__gc", hOBJ_gc },
		{ "MixerBlendCycle", MixerBlendCycle },
		{ "SetAxisMtx", SetAxisMtx },
		{ "Translate", Translate },
		{ "Update", Update },
		{ 0, 0 }
	};
	
	class_Define(L, "hOBJ", funcs, 0, sizeof(hOBJ));

	// Install a helper function.
	lua_register(L, "hOBJ", _hOBJ);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%% hTXT %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static s32 GetS32 (lua_State * L, int index, char const * key)
{
	lua_getfield(L, index, key);

	s32 ret = sI(L, -1);

	lua_pop(L, 1);

	return ret;
}

static int StretchBlt (lua_State * L)
{
	hTXT pTxt = _hT<hTXT>(L, 1);

	// Pick the source region, defaulting to the entire texture. If a dimension is
	// degenerate, quit trivially.
	iRect srcRect;

	if (!lua_isnoneornil(L, 6))
	{
		srcRect.top = GetS32(L, 6, "top");
		srcRect.left = GetS32(L, 6, "left");
		srcRect.right = GetS32(L, 6, "right");
		srcRect.bottom = GetS32(L, 6, "bottom");

		if (srcRect.left >= srcRect.right || srcRect.top >= srcRect.bottom) return 0;
	}

	else
	{
		srcRect.left = srcRect.top = 0;
		srcRect.right = pTxt->GetWidth() - 1;
		srcRect.bottom = pTxt->GetHeight() - 1;
	}

	// By default, the valid output region is the entire screen.
	gfxMode mode;	g_gi.GFXGetMode(&mode);

	s32 dx = 0, dw = s32(mode.width);
	s32 dy = 0, dh = s32(mode.height);

	s32 x = sI(L, 2), y = sI(L, 3), w = sI(L, 4), h = sI(L, 5);

	// Get various blitting parameters. By default, apply no color modulation. If a
	// view rect was supplied, adjust the output region.
	gfxBlt blt = { 0 };

	blt.clr = ~0;

	if (!lua_isnoneornil(L, 7))
	{
		lua_getfield(L, 7, "clr");		blt.clr = lua_isnumber(L, -1) ? uI(L, -1) : ~0;
		lua_getfield(L, 7, "flags");	blt.flags = static_cast<u32>(lua_tointeger(L, -1));
		lua_getfield(L, 7, "rot");		blt.rot = lua_tonumber(L, -1);
		lua_getfield(L, 7, "rotOfsX");	blt.rotOfsX = lua_tonumber(L, -1);
		lua_getfield(L, 7, "rotOfsY");	blt.rotOfsY = lua_tonumber(L, -1);
		lua_getfield(L, 7, "dstRect");

		if (!lua_isnil(L, -1))
		{
			dx = GetS32(L, -1, "x");
			dy = GetS32(L, -1, "y");
			dw = GetS32(L, -1, "w");
			dh = GetS32(L, -1, "h");

			if (dw <= 0 || dh <= 0) return 0;
		}
	}

	else blt.clr = ~0;

	// Clip the texture to the destination region, adjusting the source region in the
	// process. Draw the texture if it lies within this region.
	s32 ex = x + w, ey = y + h;

	if (x < dx) srcRect.left += s32(dx - x), x = dx;
	if (ex > dx + dw) srcRect.right += s32(dx + dw - ex), ex = dx + dw;
	if (y < dy)	srcRect.top += s32(dy - y), y = dy;
	if (ey > dy + dh) srcRect.bottom += s32(dy + dh - ey), ey = dy + dh;

	if (ex > x && ey > y) pTxt->StretchBlt(f32(x), f32(y), f32(ex - x), f32(ey - y), &srcRect, &blt);

	return 0;
}

static int GetWidth (lua_State * L)
{
	lua_pushinteger(L, _hT<hTXT>(L, 1)->GetWidth());

	return 1;
}

static int GetHeight (lua_State * L)
{
	lua_pushinteger(L, _hT<hTXT>(L, 1)->GetHeight());

	return 1;
}

//
// hTXT metamethods
//
static int hTXT_gc (lua_State * L)
{
	return _gcT<hTXT>(L);
}

//
// hTXT new
//
static int hTXT_New_WS (lua_State * L)
{
	*(hTXT*)UD(L, 1) = g_gi.TextureLoad(WS_(L, 2)->c_str(), lua_toboolean(L, 3), u32(lua_tointeger(L, 4)));

	return 0;
}

static int hTXT_New_T (lua_State * L)
{
	return _refT<hTXT>(L);
}

///
/// hTXT utilities
///
static int _hTXT (lua_State * L)
{
	return GenericNew(L, "hTXT", 1);
}

/// @brief Defines the hTXT class
/// @param L Lua state
static void define_hTXT (lua_State * L)
{
	// Supply an overloaded new.
	Overload hTXT_Cons(L, 2);	// G
		hTXT_Cons.AddDef(hTXT_New_WS, "hTXT", "WS");
		hTXT_Cons.AddDef(hTXT_New_T, "hTXT", "userdata");
		hTXT_Cons.AddDef(hTXT_New_T, "hTXT", "hTXT");

	// Define the texture.
	luaL_reg funcs[] = {
		{ "GetWidth", GetWidth },
		{ "GetHeight", GetHeight },
		{ "__gc", hTXT_gc },
		{ "StretchBlt", StretchBlt },
		{ 0, 0 }
	};
	
	class_Define(L, "hTXT", funcs, 0, sizeof(hTXT));

	// Install a helper function.
	lua_register(L, "hTXT", _hTXT);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% hVIEW %%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int SetScene (lua_State * L)
{
	_hT<hVIEW>(L, 1)->SetScene();

	return 0;
}

//
// hVIEW metamethods
//
static int hVIEW_gc (lua_State * L)
{
	return _gcT<hVIEW>(L);
}

//
// hVIEW new
//
static int hVIEW_New_Vec3D_2 (lua_State * L)
{
	*(hVIEW*)UD(L, 1) = g_gi.ViewCreate(*Vec3D_(L, 2, "new hVIEW"), *Vec3D_(L, 3, "new hVIEW"));

	return 0;
}

static int hVIEW_New_V (lua_State * L)
{
	return _refT<hVIEW>(L);
}

///
/// hVIEW utilities
///
static int _hVIEW (lua_State * L)
{
	return GenericNew(L, "hVIEW", 2);
}

/// @brief Defines the hVIEW class
/// @param L Lua state
static void define_hVIEW (lua_State * L)
{
	// Supply an overloaded new.
	Overload hVIEW_Cons(L, 3);	// G
		hVIEW_Cons.AddDef(hVIEW_New_Vec3D_2, "hVIEW", "Vec3D", "Vec3D");
		hVIEW_Cons.AddDef(hVIEW_New_V, "hVIEW", "userdata", "nil");
		hVIEW_Cons.AddDef(hVIEW_New_V, "hVIEW", "hVIEW", "nil");

	// Define the hVIEW.
	luaL_reg funcs[] = {
		{ "__gc", hVIEW_gc },
		{ "SetScene", SetScene },
		{ 0, 0 }
	};
	
	class_Define(L, "hVIEW", funcs, 0, sizeof(hVIEW));

	// Install a helper function.
	lua_register(L, "hVIEW", _hVIEW);
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% GENERAL %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int Clear (lua_State * L)
{
	// arg 2: array of rects

	g_gi.GFXClear(uI(L, 1), 0, 0, uI(L, 3), F(L, 4), uI(L, 5));

	return 0;
}

static int DrawBox (lua_State * L)
{
	Vec2D verts[5];

	verts[0].x = verts[3].x = verts[4].x = F(L, 1);
	verts[0].y = verts[1].y = verts[4].y = F(L, 2);

	verts[1].x = verts[2].x = verts[0].x + F(L, 3) - 1.0f;
	verts[2].y = verts[3].y = verts[0].y + F(L, 4) - 1.0f;

	g_gi.LineDraw(verts, 5, uI(L, 5));

	return 0;
}

static int _RGB (lua_State * L)
{
	if (lua_toboolean(L, 4)) lua_pushinteger(L, GFXCOLOR_COLORVALUE(F(L, 1), F(L, 2), F(L, 3), 1.0f));

	else lua_pushinteger(L, GFXCOLOR_RGBA(U8(L, 1), U8(L, 1), U8(L, 3), 0xFF));

	return 1;
}

static int _RGBA (lua_State * L)
{
	if (lua_toboolean(L, 4)) lua_pushinteger(L, GFXCOLOR_COLORVALUE(F(L, 1), F(L, 2), F(L, 3), F(L, 4)));

	else lua_pushinteger(L, GFXCOLOR_RGBA(U8(L, 1), U8(L, 1), U8(L, 3), U8(L, 4)));

	return 1;
}

static int SceneClear (lua_State * L)
{
	g_gi.GFXSceneClear();

	return 0;
}

static int SceneRender (lua_State * L)
{
	g_gi.GFXSceneRender();

	return 0;
}

static int TextureBegin (lua_State * L)
{
	g_gi.TextureBegin();

	return 0;
}

static int TextureEnd (lua_State * L)
{
	g_gi.TextureEnd();

	return 0;
}

/*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%% GLOBALS %%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%-*/

static int _vw (lua_State * L)
{
	gfxMode mode;	g_gi.GFXGetMode(&mode);

	lua_pushinteger(L, mode.width);

	return 1;
}

static int _vh (lua_State * L)
{
	gfxMode mode;	g_gi.GFXGetMode(&mode);

	lua_pushinteger(L, mode.height);

	return 1;
}

/// @brief Binds graphics utilities to the Lua scripting system
void LuaI::open_graphics (lua_State * L)
{
	// Bind globals.
	Call(L, "BindGlobal", 0, "s f f", "vw", _vw, NOP);
	Call(L, "BindGlobal", 0, "s f f", "vh", _vh, NOP);

	// Define graphics classes.
	define_hFNT(L);
	define_hMDL(L);
	define_hOBJ(L);
	define_hTXT(L);
	define_hVIEW(L);

	// Install graphics routines.
	luaL_reg funcs[] = {
		{ "Clear", Clear },
		{ "DrawBox", DrawBox },
		{ "RGB", _RGB },
		{ "RGBA", _RGBA },
		{ "SceneClear", SceneClear },
		{ "SceneRender", SceneRender },
		{ "TextureBegin", TextureBegin },
		{ "TextureEnd", TextureEnd },
		{ 0, 0 }
	};

	luaL_register(L, "gfx", funcs);	// { ... }

	// Load constants.
	struct {
		char const * mName;	///< Constant name
		int mValue;	///< Constant value
	} consts[] = {
		{ "ClearTarget", GFXCLEAR_TARGET },
		{ "ClearZBuffer", GFXCLEAR_ZBUFFER },
		{ "ClearStencil", GFXCLEAR_STENCIL },
		{ "ClearAll", GFXCLEAR_ALL },
		{ "BltFlipH", GFXBLT_FLIPH },
		{ "BltFlipV", GFXBLT_FLIPV }
	};

	for (int index = 0; index < sizeof(consts) / sizeof(*consts); ++index)
	{
		lua_pushinteger(L, consts[index].mValue);	// { ... }, value
		lua_setfield(L, -2, consts[index].mName);	// { ..., name = value }
	}

	lua_pop(L, 1);
}