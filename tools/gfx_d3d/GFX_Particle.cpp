#include "gfx_i.h"

#include "GFX\gfx_math.h"

#include "STD\cfg.h"

//Based on Cipher Engine

//particle system flags

// true if the rgb colour of the particles should fade with time
#define PS_FLAG_FADERGB		0x00000001

// true if the alpha should fade with time
#define PS_FLAG_FADEALPHA	0x00000002

// true to draw each particle as a streak (from old pos to new pos)
#define PS_FLAG_MOTIONBLUR	0x00000004

// true if particles should always head away from the emitter (really for area emitters)
#define PS_FLAG_OUTDIR		0x00000008

// true if particles start at outer bound and move inward towards origin (really for area emitters)
// you cannot set this with OUTDIR
#define PS_FLAG_INDIR		0x00000010

//constants
#define MAX_VERT_BATCH	1024
#define MAX_ACTIVE_PS	32

enum
{
	EMITTER_AREA_POINT,
	EMITTER_AREA_SPHERE,
	EMITTER_AREA_BOX
};

enum
{
	COLLISION_IGNORE,
	COLLISION_DIE,
	COLLISION_BOUNCE,
	COLLISION_STOP,
	COLLISION_REPEL
};

typedef struct particle_s
{
	s32		age;				// birth time in ms
	Vec3D	pos;
	Vec3D	vel;
} particle_t;

typedef struct psColour_s {
	u8 r,g,b,a;
} psColour_t;

struct psPosClr {
	Vec3D	pos;
	u32		clr;
};

typedef struct particlesystem_s
{
	//tCHAR	name[MAXCHARBUFF];				// name of the system

	hEFFECT fx;								//the fx used
	hTXT	txt;							//if fx is NULL, we use this with default fx

	FLAGS	flag;			// particle system flags

	f32		emitter_rate;	// number of particles a second to emit
	f32		emitter_life;	// time that the emitter lives for (ms)
	u8		emitter_area;	// point, sphere or box
	f32		area_radius;	// radius of sphere emitters
	Vec3D	area_size;		// size of box emitters

	u8		collide_world;	// what to do when a collision with the world is detected
	u8		collide_entity;	// What to do when a collision with an entity is detected.
	f32		bouncelow;
	f32		bouncehigh;
	
	Vec3D	acceleration;	// rate and direction of particle acceleration
	Vec3D   direction;		// direction particles go in
	f32		spread;			// variation on the direction (0-1). 0=direction, 1=random
	f32		minspeed;		// low end of speed 
	f32	    maxspeed;		// high end of initial speed
	f32		drag;			// drag factor	
	f32		spinrate;		// radians over life
	f32		life;			// how long do particles live for (ms)
	f32		size;			// size of particles at birth
	f32		growby;			// At death, particles will be size+growby across
	f32		motionblurlag;	// The time lag for motion blur
	psColour_t colour;
} particlesystem_t;


typedef struct active_ps_s
{
	s32		id;
	u8		updated;		// true if this system has been updated since the last clear
	Vec3D	bbmin;			// lower corner of particles bounding box
	Vec3D	bbmax;			// upper corner of particles bounding box
	s16		max_particles;	// the maximum number of particles this ps can generate
	s16		num_particles;	// the current number of particles in the system
	s16		num_created;	// how many have we actually created so far
	f32		time;			// The age of the particle system (in ms)
	f32		frametime;		// the duration of the current frame

	Vec3D   direction;		// direction particles go in

	gfxParInfo info;		// info that is updated frame to frame
	Vec3D	oldemitterpos;
	Vec3D	emitteroffset;

	particlesystem_t* ps;	// the particle system this is an instance of
	particle_t*	particles;	// the particles used by this system

	psPosClr		*verts;
	//vec3_t*		xyz;
	//colour_t*	colour;
} active_ps_t;


typedef struct symbol_s
{
	tCHAR* sym;
	void (*Func)(particlesystem_t* ps, const tCHAR *tok);
} symbol_t;


// All the particle systems we know about
typedef map<wstring, particlesystem_t *>::iterator psIt;

map<wstring, particlesystem_t *> ps_registered;


// the file we are currently processing
//static tchar_t*				ps_tokenbuffer;

// The list of currently active particle systems
typedef map<s32, active_ps_t *>::iterator apsIt;

map<s32, active_ps_t *> ps_active;
static s32				ps_next_active_id = 1;

//the collision function for all particles
PARCOLLFUNC		  ps_collF=0;

// The geometry block that we load the particles 
// into before sending it to the renderer
// each particle needs 4 verts and 4 index entries
#define PS_VTXBUFF_USAGE (GFXUSAGE_WRITEONLY | GFXUSAGE_DYNAMIC)//D3DUSAGE_DYNAMIC//D3DUSAGE_SOFTWAREPROCESSING//
#define PS_VTXBUFF_POOL	 GFXPOOL_DEFAULT//GFXPOOL_MANAGED//

#define PS_TXTBUFF_USAGE (GFXUSAGE_WRITEONLY)
#define PS_TXTBUFF_POOL	 GFXPOOL_MANAGED

#define PS_INDBUFF_USAGE (GFXUSAGE_WRITEONLY)
#define PS_INDBUFF_POOL	 GFXPOOL_MANAGED

GFXVTXBUFF			   *ps_vtx;		//only holds the vertices and color (stream 0)
//GFXVTXBUFF			   *ps_txt;		//only holds texture coordinates (stream 1)

GFXINDBUFF			   *ps_indexlist;	//the index buffer (we render primitives in fan)

hEFFECT					ps_effect=0;		//the default effect

s8 ps_fxbuff[] = 
"bool transparent = true;"\
"texture BaseTex : TEX0;"\
"float4x4 TransMtx : PROJECTION;"\
"struct VS_OUTPUT"\
"{"\
"float4 Pos : POSITION;"\
"float4 Diff : COLOR0;"\
"float2 Tex : TEXCOORD0;"\
"};"\
"VS_OUTPUT VS(float3 Pos : POSITION, float4 clr : COLOR0, float2 Tex : TEXCOORD0) {"\
"VS_OUTPUT Out = (VS_OUTPUT)0;"\
"Out.Pos  = mul(float4(Pos, 1), TransMtx);"\
"Out.Diff = clr;"\
"Out.Tex  = Tex; return Out; }"\
"sampler Sampler = sampler_state {"\
"Texture = (BaseTex);"\
"MipFilter = LINEAR;"\
"MinFilter = LINEAR;"\
"MagFilter = LINEAR; };"\
"technique TVertexShaderOnly {"\
"pass P0 {"\
"cullmode = none;"\
"fogenable=false;"\
"ZWRITEENABLE = false;"\
"Sampler[0] = (Sampler);"\
"ColorOp[0] = MODULATE;"\
"ColorArg1[0] = TEXTURE;"\
"ColorArg2[0] = DIFFUSE;"\
"AlphaOp[0] = MODULATE;"\
"AlphaArg1[0] = TEXTURE;"\
"AlphaArg2[0] = DIFFUSE;"\
"ColorOp[1] = DISABLE;"\
"AlphaOp[1] = DISABLE;"\
"VertexShader = compile vs_1_1 VS();"\
"PixelShader = NULL; } }";

/////////////////////////////////////
// Name:	ParSysInit
// Purpose:	initializes the vertex
//			buffers and indices and
//			default FX
// Output:	stuff initialized
// Return:	TRUE if success
/////////////////////////////////////
u32 ParSysInit()
{
	//create the vertex buffer
	ps_vtx = GFXCreateVtxBuff(sizeof(psPosClr), MAX_VERT_BATCH, PS_VTXBUFF_USAGE, 0, PS_VTXBUFF_POOL);

	//create the texture pos buffer
	GFXAppendVtxBuff(ps_vtx, sizeof(Vec2D), MAX_VERT_BATCH, PS_TXTBUFF_USAGE, 0, PS_TXTBUFF_POOL);

	//set the vertex declaration
	//now let's set the vertex declaration
	D3DVERTEXELEMENT9 Declaration[4];
    D3DVERTEXELEMENT9 End = D3DDECL_END();

	Declaration[0].Stream = 0;
	Declaration[0].Offset = 0;
	Declaration[0].Type = D3DDECLTYPE_FLOAT3;
	Declaration[0].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[0].Usage = D3DDECLUSAGE_POSITION;
	Declaration[0].UsageIndex = 0;

	Declaration[1].Stream = 0;
	Declaration[1].Offset = sizeof(f32)*3;
	Declaration[1].Type = D3DDECLTYPE_D3DCOLOR;
	Declaration[1].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[1].Usage = D3DDECLUSAGE_COLOR;
	Declaration[1].UsageIndex = 0;

	Declaration[2].Stream = 1;
	Declaration[2].Offset = 0;
	Declaration[2].Type = D3DDECLTYPE_FLOAT2;
	Declaration[2].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[2].Usage = D3DDECLUSAGE_TEXCOORD;
	Declaration[2].UsageIndex = 0;

	Declaration[3] = End;

	VtxSetDecl(ps_vtx, Declaration);

	//create the index buffer
	GFXCreateIndBuff(sizeof(u16)*(MAX_VERT_BATCH/4*6), PS_INDBUFF_USAGE, 16, PS_INDBUFF_POOL, &ps_indexlist);

	//fill up the index and texture positions
	Vec2D *pTxtPos;
	u16	  *pInd;

	VtxLock(ps_vtx, 1, 0,0, (void**)&pTxtPos, 0);
	IndLock(ps_indexlist, 0,0, (void**)&pInd, 0);

	u32 n=0;
	for(u32 i = 0; i < MAX_VERT_BATCH; i+=4)
	{
		pTxtPos[i+0].x = 0;
		pTxtPos[i+0].y = 0;

		pTxtPos[i+1].x = 1;
		pTxtPos[i+1].y = 0;

		pTxtPos[i+2].x = 1;
		pTxtPos[i+2].y = 1;

		pTxtPos[i+3].x = 0;
		pTxtPos[i+3].y = 1;

		// index list
		pInd[n++] = 0 + i;
		pInd[n++] = 1 + i;
		pInd[n++] = 2 + i;
		pInd[n++] = 0 + i;
		pInd[n++] = 2 + i;
		pInd[n++] = 3 + i;
	}

	VtxUnlock(ps_vtx, 1);
	IndUnlock(ps_indexlist);

	//create the default FX
	ps_effect = EffectCreate(L"PS_DefaultFX", ps_fxbuff);

	return TRUE;
}

/////////////////////////////////////
// Name:	ParSysDestroy
// Purpose:	destroy all buffers and
//			default FX
// Output:	all buffers destroyed and FX
// Return:	none
/////////////////////////////////////
void ParSysDestroy()
{
	if(ps_vtx)
	{ GFXDestroyVtxBuff(ps_vtx); ps_vtx = 0; }

	SAFE_RELEASE(ps_indexlist);

	if(ps_effect)
	{ SAFE_RELEASE(ps_effect); }
}

/////////////////////////////////////
// Name:	ParSysSetCollisionFunc
// Purpose:	set the collision function
// Output:	collision function set
// Return:	none
/////////////////////////////////////
void F_API ParSysSetCollisionFunc(PARCOLLFUNC func)
{
	ps_collF=func;
}

/*
==========================
ps_ParseShader

Reads the name of the shader to use on the particles
==========================
*/
void ps_ParseShader(particlesystem_t* ps, const tCHAR *tok)
{
	hFILE fp;
	wstring path;

	//try fx
	path = tok;
	path += L".fx";

	fp = FSOpen(path.c_str(), L"rb");
	if(fp)
	{
		FSClose(fp);
		ps->fx = EffectLoad(path.c_str());
		return;
	}

	//try .jpg
	path = tok;
	path += L".jpg";

	fp = FSOpen(path.c_str(), L"rb");
	if(fp)
	{
		FSClose(fp);
		ps->txt = TextureLoad(path.c_str(), 0, 0);
		return;
	}

	//try .tga
	path = tok;
	path += L".tga";

	fp = FSOpen(path.c_str(), L"rb");
	if(fp)
	{
		FSClose(fp);
		ps->txt = TextureLoad(path.c_str(), 0, 0);
		return;
	}
}





/*
==========================
ps_ParseEmitterRate

Read the rate at which particles are born
==========================
*/
void ps_ParseEmitterRate(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	
	ps->emitter_rate = (f32)wcstod(tok, &strStop);

	// clamp to sane values
	if (ps->emitter_rate<1)		ps->emitter_rate=1;
	if (ps->emitter_rate>20000)	ps->emitter_rate=20000;
}






/*
==========================
ps_ParseEmitterLife

Read in how long the emitter will generate particles for
==========================
*/
void ps_ParseEmitterLife(particlesystem_t* ps, const tCHAR *tok)
{	
	if (wcsicmp(tok, L"infinite") == 0)
		ps->emitter_life = 0;
	else
	{
		tCHAR *strStop;
		ps->emitter_life = (f32) (wcstod(tok, &strStop) * 1000);
	}

	// bring the life into a valid range
	if (ps->emitter_life<0)
		ps->emitter_life = 0;
}





/*
==========================
ps_ParseEmitterArea

Read in the area over which particles are created in
==========================
*/
void ps_ParseEmitterArea(particlesystem_t* ps, const tCHAR *tok)
{
	Vec3D v;
	tCHAR type[MAXCHARBUFF];

	swscanf(tok, L"%s %f %f %f", type, &v.x, &v.y, &v.z);

	if (wcsicmp(type, L"point") == 0)
	{
		// this is the default, but support in anyway
		ps->emitter_area = EMITTER_AREA_POINT;
	}
	else if (wcsicmp(type, L"sphere") == 0)
	{
		ps->area_radius = v.x;
		if (ps->area_radius<0.01f)
			ps->area_radius = 0.01f;

		// set the type to a sphere
		ps->emitter_area = EMITTER_AREA_SPHERE;
	}
	else if (wcsicmp(type, L"box") == 0)
	{
		ps->area_size = v;
		
		// internally, we use a value that represents the distance from the 
		// center of the box to the edge. Script writers prefer the dimensions to 
		// be the total width, so we divide by 2 here.
		ps->area_size *= 0.5f;

		// clamp the size to something resonable
		if (ps->area_size.x<0.01f)	ps->area_size.x = 0.01f;
		if (ps->area_size.y<0.01f)	ps->area_size.y = 0.01f;
		if (ps->area_size.z<0.01f)	ps->area_size.z = 0.01f;

		// Set the emitter type
		ps->emitter_area = EMITTER_AREA_BOX;
	}
}





/*
==========================
ps_ParseSpinRate

Read in the rate at which particles spin
==========================
*/
void ps_ParseSpinRate(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->spinrate = TO_RAD((f32)wcstod(tok, &strStop));
}




/*
==========================
ps_ParseSize

Read in the initial size of particles
==========================
*/
void ps_ParseSize(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->size = (f32)wcstod(tok, &strStop);
	if (ps->size<0)		ps->size=0;
	if (ps->size>1000)	ps->size=1000;
}




/*
==========================
ps_ParseGrowby

Read in how much particles grow or shrink by
==========================
*/
void ps_ParseGrowby(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->growby = (f32)wcstod(tok, &strStop);
	if (ps->growby<-1000)	ps->growby = -1000;
	if (ps->growby>1000)		ps->growby = 1000;
}




/*
==========================
ps_ParseAcceleration

Read in the acceleration particles will experience
==========================
*/
void ps_ParseAcceleration(particlesystem_t* ps, const tCHAR *tok)
{
	swscanf(tok, L"%f %f %f", &ps->acceleration.x, &ps->acceleration.y, &ps->acceleration.z);
}




/*
==========================
ps_ParseFadeAlpha

Remember to fade the alpha channel as particles age
==========================
*/
void ps_ParseFadeAlpha(particlesystem_t* ps, const tCHAR *tok)
{
	SETFLAG(ps->flag, PS_FLAG_FADEALPHA);
}





/*
==========================
ps_ParseFadeRGB

Remember to fade the rgb values as the particle ages
==========================
*/
void ps_ParseFadeRGB(particlesystem_t* ps, const tCHAR *tok)
{
	SETFLAG(ps->flag, PS_FLAG_FADERGB);
}




/*
==========================
ps_ParseColour

Read in the colour new particles should be
==========================
*/
void ps_ParseColour(particlesystem_t* ps, const tCHAR *tok)
{
	Vec4D c;

	// read in the rgba values
	swscanf(tok, L"%f %f %f %f", &c.x, &c.y, &c.z, &c.w);

	//clamp values
	if(c.x<0) c.x = 0; else if(c.x>1) c.x = 1;
	if(c.y<0) c.y = 0; else if(c.y>1) c.y = 1;
	if(c.z<0) c.z = 0; else if(c.z>1) c.z = 1;
	if(c.w<0) c.w = 0; else if(c.w>1) c.w = 1;

	ps->colour.r = (u8) (c.x * 255);
	ps->colour.g = (u8) (c.y * 255);
	ps->colour.b = (u8) (c.z * 255);
	ps->colour.a = (u8) (c.w * 255);
}






/*
==========================
ps_ParseLife

Read in how long each particle will live for
==========================
*/
void ps_ParseLife(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->life = (f32) (wcstod(tok, &strStop) * 1000);
	if (ps->life<1)	ps->life = 1;
}







/*
==========================
ps_ParseDirection

Read in the initial direction that particles will travel in
when they are created
==========================
*/
void ps_ParseDirection(particlesystem_t* ps, const tCHAR *tok)
{
	
	// read the direction

	// Direction can be "out" to get particles to just go away from the
	// emitter. This is really only used in combination with area emitters
	if (wcsicmp(tok, L"out") == 0)
	{
		SETFLAG(ps->flag, PS_FLAG_OUTDIR);
		ps->direction.x = 0;
		ps->direction.y = 1;
		ps->direction.z = 0;
	}
	else if(wcsicmp(tok, L"in") == 0)
	{
		SETFLAG(ps->flag, PS_FLAG_INDIR);
		ps->direction.x = 0;
		ps->direction.y = 1;
		ps->direction.z = 0;
	}
	else
	{
		// otherwise, it is a direction vector
		swscanf(tok, L"%f %f %f", &ps->direction.x, &ps->direction.y, &ps->direction.z);

		// normalise the vector
		if(Vec3DNormal(&ps->direction, &ps->direction)<0.001f)
		{
			ps->direction.x = 0;
			ps->direction.y = 1;
			ps->direction.z = 0;
		}
	}
}






/*
==========================
ps_ParseSpread

Read in how much variation there is on the initial direction
0 is no variation, 1 is totaly random
==========================
*/
void ps_ParseSpread(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->spread = (f32)wcstod(tok, &strStop);

	// clamp it into range
	if (ps->spread<0)	ps->spread = 0;
	if (ps->spread>1)	ps->spread = 1;
}






/*
==========================
ps_ParseMinSpeed

Read in the minimum speed of new particles
==========================
*/
void ps_ParseMinSpeed(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->minspeed = (f32)wcstod(tok, &strStop);
	
	// keep the speed sensible
	if (ps->minspeed<0)	
		ps->minspeed = 0;
}






/*
==========================
ps_ParseMaxSpeed

Read in the max speed of new particles
==========================
*/
void ps_ParseMaxSpeed(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->maxspeed = (f32)wcstod(tok, &strStop);
	
	// keep the speed sensible
	if (ps->maxspeed<0)	
		ps->maxspeed = 0;
}






/*
==========================
ps_ParseMotionBlur

Remember to motion blur particles
==========================
*/
void ps_ParseMotionBlur(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;
	ps->motionblurlag = (f32)wcstod(tok, &strStop);
	if (ps->motionblurlag<0) 
		ps->motionblurlag=0;

	SETFLAG(ps->flag, PS_FLAG_MOTIONBLUR);
}





/*
==========================
ps_ParseDrag

Set the drag factor for particles (0 by default)
0 = no drag
1 = total drag (stops particle instantly, regardless of speed)
==========================
*/
void ps_ParseDrag(particlesystem_t* ps, const tCHAR *tok)
{
	tCHAR *strStop;

	// read the drag factor
	ps->drag = (f32)wcstod(tok, &strStop);

	// clamp it
	if (ps->drag<0)	ps->drag = 0;
}




/*
==========================
ps_ParseCollision

Read in the type of collision detection to use for
each particles motion
==========================
*/
void ps_ParseCollision(particlesystem_t* ps, const tCHAR *tok)
{
	//world bounce 0.400 0.800
	tCHAR colStr[MAXCHARBUFF], typStr[MAXCHARBUFF];

	u8 world, entity;
	s32 collide;

	// get the bouncy-ness as well
	swscanf(tok, L"%s %s %f %f", colStr, typStr, &ps->bouncelow, &ps->bouncehigh);
	
	// Is it a world or entity collision setting
	world = entity = FALSE;
	if (wcsicmp(colStr, L"world") == 0)
		world = TRUE;
	else if (wcsicmp(colStr, L"entity") == 0)
		entity = TRUE;

	// read the type of collision
	collide = COLLISION_IGNORE;
	if (wcsicmp(typStr, L"ignore") == 0)
		collide = COLLISION_IGNORE;
	else if (wcsicmp(typStr, L"stop") == 0)
		collide = COLLISION_STOP;
	else if (wcsicmp(typStr, L"die") == 0)
		collide = COLLISION_DIE;
	else if (wcsicmp(typStr, L"repel") == 0)
		collide = COLLISION_REPEL;
	else if (wcsicmp(typStr, L"bounce") == 0)
	{	
		// Clamp to sane values
		if (ps->bouncelow<0)	ps->bouncelow = 0;
		if (ps->bouncelow>5)	ps->bouncelow = 5;
		if (ps->bouncehigh<ps->bouncelow)	ps->bouncehigh = ps->bouncelow;
		if (ps->bouncehigh>5)	ps->bouncehigh = 5;		

		// note the collision type
		collide = COLLISION_BOUNCE;
	}

	// Valid arguments
	if (world && collide==COLLISION_REPEL)
		collide = COLLISION_IGNORE;

	// Set the world setting
	if (world)
		ps->collide_world = collide;
	
	// Set the entity setting
	if (entity)
		ps->collide_entity = collide;
}

/*
==========================
ps_PostParseFixup

Once we have finished parsing a particle system we can
make some final adjustments and cache some values
==========================
*/
void ps_PostParseFixup(particlesystem_t* ps)
{
	// the spin rate we store is the number of radians the
	// particle will turn though during its life.
	// we can not be of the particles life until here
	ps->spinrate *= 0.001f * ps->life;

	// ensure that max > min
	if (ps->maxspeed<ps->minspeed)
		ps->maxspeed = ps->minspeed;
}

static symbol_t ps_keywords[] =
{
	{L"shader",			ps_ParseShader		},
	{L"emitter_rate",	ps_ParseEmitterRate	},
	{L"emitter_life",	ps_ParseEmitterLife	},
	{L"emitter_area",	ps_ParseEmitterArea	},
	{L"spin_rate",		ps_ParseSpinRate	},
	{L"size",			ps_ParseSize		},
	{L"growby",			ps_ParseGrowby		},
	{L"acceleration",	ps_ParseAcceleration},
	{L"fade_alpha",		ps_ParseFadeAlpha	},
	{L"fade_rgb",		ps_ParseFadeRGB		},
	{L"colour",			ps_ParseColour		},
	{L"life",			ps_ParseLife		},
	{L"direction",		ps_ParseDirection	},
	{L"spread",			ps_ParseSpread		},
	{L"minspeed",		ps_ParseMinSpeed	},
	{L"maxspeed",		ps_ParseMaxSpeed	},
	{L"motionblur",		ps_ParseMotionBlur	},
	{L"collision",		ps_ParseCollision	},
	{L"collide",		ps_ParseCollision	},
	{L"drag",			ps_ParseDrag	},
	{NULL, NULL}
};

/*
==========================
ps_ParseChunk

read in all of ps's data with chunk
note: this is all the chunks inside {...}
==========================
*/
u32 ps_ParseChunk(particlesystem_t* ps, hSHADERF shdF, u32 secInd)
{
	const tCHAR *str, *param;

	u32 numVar = shdF->SectionGetVarNum(secInd);

	for(s32 i = 0; i < numVar; i++)
	{
		str = shdF->VarGetName(secInd, i);
		param = shdF->VarGetParam(secInd, i);

		//identify the token
		for(symbol_t *pSym = ps_keywords; pSym->sym != NULL; pSym++)
		{
			if(wcsicmp(str, pSym->sym) == 0)
			{
				// we got a hit, so call the function that parses this statement
				pSym->Func(ps, param);
				break;
			}
		}
	}

	ps_PostParseFixup(ps);

	return TRUE;

	// syntax error if we reach here...
	//ri.con_Printf(_T("Bad token in particle system script : %ls\n"), tok);
	//return rfalse;
}

/////////////////////////////////////
// Name:	ParSysExist
// Purpose:	check to see if given
//			particle system exists
// Output:	none
// Return:	TRUE if so
/////////////////////////////////////
u8 F_API ParSysExist(const tCHAR *name)
{
	//check in string list
	psIt it = ps_registered.find(name);

	return it != ps_registered.end();
}

static void _ParSysDestroy(particlesystem_t *ps)
{
	if(ps)
	{
		SAFE_RELEASE(ps->fx);
		SAFE_RELEASE(ps->txt);

		MemFree(ps);
	}
}

static particlesystem_t *_ParSysGet(const tCHAR *name)
{
	if(ParSysExist(name))
		return ps_registered[name];

	return 0;
}

//this will destroy old content
static void _ParSysSet(const tCHAR *name, particlesystem_t *ps)
{
	if(ParSysExist(name))
		_ParSysDestroy(ps_registered[name]);

	ps_registered[name] = ps;
}

//create a particle system with default settings
static particlesystem_t *_ParSysCreate()
{
	particlesystem_t *ps = (particlesystem_t *)MemAlloc(sizeof(particlesystem_t));

	if(ps)
	{
		// initialise to defaults
		ps->emitter_rate = 1;
		ps->emitter_life = 1000;
		ps->emitter_area = EMITTER_AREA_POINT;
		ps->collide_world = COLLISION_IGNORE;
		ps->collide_entity = COLLISION_IGNORE;
		ps->size = 0.1f;
		ps->life = 1000;

		ps->acceleration.x = ps->acceleration.y = ps->acceleration.z = 0;
		ps->direction.x = 0; ps->direction.y = 1; ps->direction.z = 0;

		// default to white
		ps->colour.r = 255;
		ps->colour.g = 255;
		ps->colour.b = 255;
		ps->colour.a = 255;
	}

	return ps;
}

/////////////////////////////////////
// Name:	ParSysLoad
// Purpose:	load particle systems
//			the file may contain several
//			particle system definitions
// Output:	system loaded
// Return:	TRUE if success
/////////////////////////////////////
u32 F_API ParSysLoad(const tCHAR *filename)
{
	u32 ret = TRUE;

	particlesystem_t *ps;

	u32 numSec;

	hSHADERF shdF = ShaderFileLoad(filename);

	if(shdF)
	{
		numSec = shdF->SectionGetNum();

		for(u32 i = 0; i < numSec; i++)
		{
			//create a new default particle system
			ps = _ParSysCreate();

			//parse the chunk with the particle system
			ps_ParseChunk(ps, shdF, i);

			//this may replace an existing particle system
			_ParSysSet(shdF->SectionGetName(i), ps);
		}

		shdF->Release();
	}

	return ret;
}

/////////////////////////////////////
// Name:	ParSysShutdown
// Purpose:	clear out all active
//			particles and remove all
//			particle systems
// Output:	everything cleared out
// Return:	none
/////////////////////////////////////
void F_API ParSysShutdown()
{
	//clear out all active particles
	ParSysClear();
	ParSysClear();

	//delete all particle systems
	for(psIt iPS = ps_registered.begin(); iPS != ps_registered.end(); iPS++)
		_ParSysDestroy((*iPS).second);

	ps_registered.clear();
}

/*
==========================
ps_FindActive

Searches the list of active particle systems for the instance handle specified
==========================
*/
active_ps_t* ps_FindActive(s32 id)
{
	active_ps_t* ps = 0;

	apsIt it = ps_active.find(id);

	if(it != ps_active.end())
	{
		ps = ps_active[id];
	}
	
	return ps;
}

/*
==========================
ps_CreateParticle

Creates a new particle in the slot specified
==========================
*/
void ps_CreateParticle(active_ps_t* ps, s32 i)
{
	particle_t* p;
	Vec3D v, outdir;
	f32 r;
	s32 tries;

	// Find the particle we are updating
	p = &ps->particles[i];

	// create a new particle in this slot
	// give an age somewhere in this frame
	p->age = RandRngInt(1, (s32)ps->frametime);

	// Pick the starting point for the particle
	switch (ps->ps->emitter_area)
	{
	case EMITTER_AREA_POINT:
		// position it along the line between the previous emitter position and the current emitter position
		p->pos = ps->oldemitterpos + RandRngFloat(0, 1)*ps->emitteroffset;

		// out/in direction isn't really meant for point emitters, so
		// just copy the default direction in this case
		if (TESTFLAGS(ps->ps->flag, PS_FLAG_OUTDIR) || TESTFLAGS(ps->ps->flag, PS_FLAG_INDIR))
			outdir = ps->direction;

		break;

	case EMITTER_AREA_SPHERE:
		// pick a random point inside the sphere around the emitter.
		r = ps->ps->area_radius;
		tries = 0;
		do {
			tries++;
			outdir.x = RandRngFloat(-r, r);
			outdir.y = RandRngFloat(-r, r);
			outdir.z = RandRngFloat(-r, r);
			if (tries>4)
				outdir *= 0.707f;
		} while (Vec3DDot(&outdir, &outdir) > r*r);

		// move relative to the emitter
		if(TESTFLAGS(ps->ps->flag, PS_FLAG_INDIR))
		{
			Vec3D inDir; Vec3DNormal(&outdir, &inDir);

			//push position out
			p->pos = ps->info.emitter_pos + inDir*r;

			outdir = -outdir;
		}
		else
			p->pos = outdir + ps->info.emitter_pos;

		break;

	case EMITTER_AREA_BOX:
		// pick a random point in the box
		outdir.x = RandRngFloat(-ps->ps->area_size.x, ps->ps->area_size.x);
		outdir.y = RandRngFloat(-ps->ps->area_size.y, ps->ps->area_size.y);
		outdir.z = RandRngFloat(-ps->ps->area_size.z, ps->ps->area_size.z);

		p->pos = outdir + ps->info.emitter_pos;

		if(TESTFLAGS(ps->ps->flag, PS_FLAG_INDIR))
			outdir = -outdir;

		break;
	}
		
	// figure out the velocity of the particle
	if(TESTFLAGS(ps->ps->flag, PS_FLAG_OUTDIR) || TESTFLAGS(ps->ps->flag, PS_FLAG_INDIR))
		v = outdir*(1.0f - ps->ps->spread);
	else
		v = ps->direction*(1.0f - ps->ps->spread);

	// mix in some randomness
	if (ps->ps->spread>0.01f)
	{
		Vec3D rand_dir(RandRngFloat(-1,1), RandRngFloat(-1,1), RandRngFloat(-1,1));
//		v = ps->direction*(1.0f - ps->ps->spread);
		v += (ps->ps->spread)*rand_dir;
	}

	// adjust it into the velocity range
	Vec3DNormal(&v, &v);
	p->vel = RandRngFloat(ps->ps->minspeed, ps->ps->maxspeed)*v;

	// increse the count of particles
	ps->num_particles++;
}

/*
==========================
ps_CollideParticle

Attempts to collide the particle with the environment
==========================
*/
void ps_CollideParticle(active_ps_t* ps, particle_t* p, Vec3D & newpos)
{
	f32 t;
	Vec3D impact, planenormal;
	
	Vec3D I, R;
	f32 mag, coef;

	if(!ps_collF)
		return;

	// try and move it through the world, and return if we hit nothing
	ps_collF(p->pos, newpos, FALSE, &impact, &planenormal, &t);
	
	if (t==1)
		return;

	// Decide how to react
	switch (ps->ps->collide_world)
	{
	case COLLISION_DIE:
		p->age = 0;
		break;

	case COLLISION_BOUNCE:
		// It hit something, so bounce
		// find the reflected ray
		I = p->vel;
		mag = Vec3DNormal(&I, &I);
		R = I + (-2.0f*Vec3DDot(&I, &planenormal))*planenormal;

		// Decide how much bounce to put in the particles.
		coef = RandRngFloat(ps->ps->bouncelow, ps->ps->bouncehigh);
		p->vel = (mag*coef)*R;

		// Update the new position to be the impact point
		// nudged out a little to avoid repeat collisions
		newpos = impact + 0.1f*planenormal;
		break;

	case COLLISION_STOP:
		newpos = impact + 0.1f*planenormal;
		p->vel.x = p->vel.y = p->vel.z = 0;
		// also need to make the particle system so that it does not accelerate any more
		break;
	}
}

/*
==========================
ps_Update

Updates a particle system (moves the particles, kills off
old particles, creates new particles etc.)

==========================
*/
u8 ps_Update(active_ps_t* ps, gfxParInfo* info)
{
	s16 i, to_create, to_update;
	particle_t* particle;
	Vec3D newpos;
	u8 collide;
	f32 dt;

	// upate any changing info
	ps->oldemitterpos = ps->info.emitter_pos;
	if (info)
		ps->info = *info;

	// This is a brand new particle system
	if (ps->time==0)
		ps->oldemitterpos = ps->info.emitter_pos;

	// age the particle system
	ps->frametime = g_time*1000.0f;
	ps->time += ps->frametime;
	dt = g_time;

	// figure out the difference in the emitter position
	ps->emitteroffset = ps->info.emitter_pos - ps->oldemitterpos;

	// figure out how many new particles to create this frame
	to_create = ((s32)(ps->time * ps->ps->emitter_rate) / 1000) - ps->num_created;
	ps->num_created += to_create;

	// If the emitter is disabled, don't make new particles
	if (ps->info.disable)
		to_create = 0;

	// if the emitter has died off, don't create new particles
	// (infinite emitters never die off)
	if ((ps->time > ps->ps->emitter_life) && (ps->ps->emitter_life!=0))
		to_create = 0;

	// Do we want to do collision tests?
	collide = FALSE;
	if ((ps->ps->collide_entity!=COLLISION_IGNORE) || (ps->ps->collide_world!=COLLISION_IGNORE))
		collide = TRUE;

	// figure out how many particles need updating
	to_update = ps->num_particles;

	// Loop over the particles, updating, killing and creating them
	// we stop as soon as we have performed all the required tasks
	particle=&ps->particles[0];
	for (i=0; i<ps->max_particles; i++, particle++)
	{
		// bail out if we have done everything
		if ((to_create==0) && (to_update==0))
			break;

		// age the particle and kill off old ones
		if (particle->age>0)
		{
			// This is one of the active ones
			// we keep track of how many of these we have process
			// so we can bail out of this loop as soon as everything is done
			to_update--;

			// age it
			particle->age += (s32)ps->frametime;
			if (particle->age > (s32)ps->ps->life)
			{
				particle->age = 0;
				ps->num_particles--;
			}
		}

		// If we are still alive, update it
		if (particle->age>0)
		{
			// accelerate the particle
			particle->vel += dt*ps->ps->acceleration;

			// add in some drag
			if (ps->ps->drag>0)
				particle->vel += (-ps->ps->drag*dt)*particle->vel;

			// move the particle
			newpos = particle->pos + dt*particle->vel;

			if (collide)
				ps_CollideParticle(ps, particle, newpos);

			// write the position back to the particle
			particle->pos = newpos;
		}

		// If we want to make new particles, and this slot is free, use it
		if (to_create && (particle->age == 0))
		{
			// create the particle and keep track 
			// of how many more need to be created
			ps_CreateParticle(ps, i);
			to_create--;
		}
	}

	// if there are some particles still alive, keep going
	if (ps->num_particles>0)
		return TRUE;

	// If the emitter is infite, keep it alive
	if (ps->ps->emitter_life==0)
		return TRUE;

	// If the emitter is still alive, keep going
	if (ps->time < ps->ps->emitter_life)
		return TRUE;

	// There are no particles, and the emitter is dead. time to stop
	return FALSE;
}

/*
==========================
ps_GenerateColour

Generates the colour data for each vertex for the particle system
If the particle system is not fading the colours in any way, this does nothing
and uses the original values in the colour array (set to white on creation)
==========================
*/
void ps_GenerateColour(active_ps_t* ps)
{
	s16 i, c;
	u8 fade_alpha, fade_rgb;
	psColour_t rgba;
	s32 age;
	f32 fade, r, g, b, a;

	// default to particle colour
	rgba = ps->ps->colour;

	// get these values locally
	fade_alpha = TESTFLAGS(ps->ps->flag, PS_FLAG_FADEALPHA);
	fade_rgb = TESTFLAGS(ps->ps->flag, PS_FLAG_FADERGB);
	r = (f32) rgba.r;
	g = (f32) rgba.g;
	b = (f32) rgba.b;
	a = (f32) rgba.a;

	// walk the list of particles, setting the rgba for each vertex (4 per particle)
	c = 0;
	for (i=0; i<ps->max_particles && c < MAX_VERT_BATCH; i++, c+=4)
	{
		// skip particles that are not in use
		age = ps->particles[i].age;
		if (age==0)
			continue;
		
		// figure out how far though its life this particle is
		fade = 1.0f - (((f32)age) / ps->ps->life);
		
		// Generate the colour
		if (fade_alpha)	rgba.a = (byte) (fade * a);
		if (fade_rgb)
		{
			rgba.r = (byte) (fade * r);
			rgba.g = (byte) (fade * g);
			rgba.b = (byte) (fade * b);
		}

		// fill in the 4 colours
		ps->verts[c].clr   = GFXCOLOR_RGBA(rgba.r,rgba.g,rgba.b,rgba.a);
		ps->verts[c+1].clr = ps->verts[c].clr;
		ps->verts[c+2].clr = ps->verts[c].clr;
		ps->verts[c+3].clr = ps->verts[c].clr;
	}
}

/*
==========================
ps_GenerateVerts

Generate the vertex data for the active particles.
==========================
*/
s16 ps_GenerateVerts(active_ps_t* ps)
{
	s16 v, i;
	f32 xx,xy,yx,yy;
	f32 s, c, a;
	f32 life, size, t, age;
	particle_t* particle;
	Vec3D pos, oldpos, t_oldpos;
	f32 dx, dy, len;
	u8 bbox_started;

	// we have not started the bounding box
	bbox_started = FALSE;

	// transform the particles to eye space, and generate 
	// the geoemtry there (hence identity to modelview)
	v = 0;
	for (i=0; i<ps->max_particles; i++, v+=4)
	{
		// skip particles that are not in use
		particle = &ps->particles[i];
		if (particle->age==0)
			continue;

		// figure out how far though its life this particle is
		life = (f32) particle->age / ps->ps->life;
		size = ps->ps->size + life*ps->ps->growby;

		// transform the position to eye space
		Vec3DTransformCoord(&particle->pos, &g_mtxView[FXMTX_NORMAL], &pos);

		// Update the bounding rect of the particle system
		if (bbox_started)
		{
			if (particle->pos.x < ps->bbmin.x)	ps->bbmin.x = particle->pos.x;
			if (particle->pos.y < ps->bbmin.y)	ps->bbmin.y = particle->pos.y;
			if (particle->pos.z < ps->bbmin.z)	ps->bbmin.z = particle->pos.z;
			
			if (particle->pos.x > ps->bbmax.x)	ps->bbmax.x = particle->pos.x;
			if (particle->pos.y > ps->bbmax.y)	ps->bbmax.y = particle->pos.y;
			if (particle->pos.z > ps->bbmax.z)	ps->bbmax.z = particle->pos.z;
		}
		else
		{
			// this is the first point to be added to the bounding box, so set the
			// bounding box to this particles position
			ps->bbmin = particle->pos;
			ps->bbmax = particle->pos;
			bbox_started = TRUE;
		}		

		// decide how to generate the quads
		if(TESTFLAGS(ps->ps->flag, PS_FLAG_MOTIONBLUR))
		{
			// find the old positin and transform it to eye space as well
			t = ps->ps->motionblurlag;
			age = (f32)particle->age / 1000.0f;
			if (age<t)
				t = age;
			t = -t;

			t_oldpos.x = particle->pos.x + particle->vel.x*t + 0.5f * ps->ps->acceleration.x*t*t;
			t_oldpos.y = particle->pos.y + particle->vel.y*t + 0.5f * ps->ps->acceleration.y*t*t;
			t_oldpos.z = particle->pos.z + particle->vel.z*t + 0.5f * ps->ps->acceleration.z*t*t;

			Vec3DTransformCoord(&t_oldpos, &g_mtxView[FXMTX_NORMAL], &oldpos);

			// figure out the direction and length of the vector in eye space
			dx = pos.x - oldpos.x;
			dy = pos.y - oldpos.y;
			len = dx*dx + dy*dy;

			//If the particle is coming directly towards or away from the camera, skip it
			if (len<0.001f)
				len = 0.001f;

			// rescale the vector so it is size units longnormalise the 2D vector
			len = sqrtf(len);
			dx = dx * size / len;
			dy = dy * size / len;

			// We now build the quad around the two end points
			ps->verts[v].pos.x = pos.x+dy;
			ps->verts[v].pos.y = pos.y-dx;
			ps->verts[v].pos.z = pos.z;

			ps->verts[v+1].pos.x = pos.x-dy;
			ps->verts[v+1].pos.y = pos.y+dx;
			ps->verts[v+1].pos.z = pos.z;

			ps->verts[v+2].pos.x = oldpos.x-dy;
			ps->verts[v+2].pos.y = oldpos.y+dx;
			ps->verts[v+2].pos.z = oldpos.z;

			ps->verts[v+3].pos.x = oldpos.x+dy;
			ps->verts[v+3].pos.y = oldpos.y-dx;
			ps->verts[v+3].pos.z = oldpos.z;
		}
		else
		{
			if (ps->ps->spinrate==0)
			{
				// standard non-rotating particle
				ps->verts[v].pos.x = pos.x-size;
				ps->verts[v].pos.y = pos.y-size;
				ps->verts[v].pos.z = pos.z;

				ps->verts[v+1].pos.x = pos.x+size;
				ps->verts[v+1].pos.y = pos.y-size;
				ps->verts[v+1].pos.z = pos.z;

				ps->verts[v+2].pos.x = pos.x+size;
				ps->verts[v+2].pos.y = pos.y+size;
				ps->verts[v+2].pos.z = pos.z;

				ps->verts[v+3].pos.x = pos.x-size;
				ps->verts[v+3].pos.y = pos.y+size;
				ps->verts[v+3].pos.z = pos.z;
			}
			else
			{
				// Spinning particle
				// figure out the angle (make half the particle spin the other way)
				a = life * ps->ps->spinrate;
				if (i&1) a = -a;
				s = sinf(a);
				c = cosf(a);
				
				// work out some offsets
				xx = size * c;
				xy = size * s;
				yx = size * -s;
				yy = size * c;
				
				// generate the verts
				ps->verts[v].pos.x = pos.x-(xx+yx);
				ps->verts[v].pos.y = pos.y-(xy+yy);
				ps->verts[v].pos.z = pos.z;

				ps->verts[v+1].pos.x = pos.x+(xx-yx);
				ps->verts[v+1].pos.y = pos.y+(xy-yy);
				ps->verts[v+1].pos.z = pos.z;

				ps->verts[v+2].pos.x = pos.x+(xx+yx);
				ps->verts[v+2].pos.y = pos.y+(xy+yy);
				ps->verts[v+2].pos.z = pos.z;

				ps->verts[v+3].pos.x = pos.x+(yx-xx);
				ps->verts[v+3].pos.y = pos.y+(yy-xy);
				ps->verts[v+3].pos.z = pos.z;
			}
		}
	}

	// grow the bounding box by the max size of a particle
	size = ps->ps->size + ps->ps->growby;

	//put it in object space
	ps->bbmin.x -= size + ps->info.emitter_pos.x;
	ps->bbmin.y -= size + ps->info.emitter_pos.y;
	ps->bbmin.z -= size + ps->info.emitter_pos.z;
	ps->bbmax.x += size - ps->info.emitter_pos.x;
	ps->bbmax.y += size - ps->info.emitter_pos.y;
	ps->bbmax.z += size - ps->info.emitter_pos.z;
	
	return v;
}

void ParFXCallback(hIEFFECT effect, u32 wParam, s32 lParam)
{
	s16 ind = ((wParam&0xffff0000)>>16);
	s16 numverts = (wParam&0x0000ffff);

	active_ps_t *ps = (active_ps_t *)lParam;
	psPosClr	*vtx;

	if(VtxLock(ps_vtx, 0, 0, numverts*sizeof(psPosClr), (void**)&vtx, D3DLOCK_DISCARD))
	{
		memcpy(vtx, &ps->verts[ind], sizeof(psPosClr)*numverts);
		VtxUnlock(ps_vtx, 0);
	}

	if(ps->ps->txt)
		effect->SetBaseTexture(0, ps->ps->txt);
}

/////////////////////////////////////
// Name:	ParSysRender
// Purpose:	add given particle to
//			the scene for rendering
// Output:	scene stuff
// Return:	none
/////////////////////////////////////
void F_API ParSysRender(s32 ID)
{
	active_ps_t* ps;

	// Find the particle system to draw
	ps = ps_FindActive(ID);
	if ((!ps) || (!ps->particles))
		return;

	// If there are no particles to draw, don't bother
	if (ps->num_particles==0)		
		return;

	//make sure this particle is within view-frustrum
	if(FrustrumCheckBox(ps->bbmin+ps->info.emitter_pos, ps->bbmax+ps->info.emitter_pos))
	{
		ps_GenerateColour(ps);
		s16 numverts = ps_GenerateVerts(ps);
		s16 i = 0;
		u32 param;

		SceneGeometry g={0};
		g.fx = (hIEFFECT)(ps->ps->fx ? ps->ps->fx : ps_effect);
		g.pVtx = ps_vtx;
		g.pInd = ps_indexlist;
		g.primType = GFXPT_TRIANGLELIST;
		g.flag = SCENE_SINGLEMTX;
		g.fxCB = ParFXCallback;
		g.lParam = (s32)ps;

		//calculate camera distance
		//Vec3D vCamObj(ps->info.emitter_pos.x-g_eyePt.x, 
		//	ps->info.emitter_pos.y-g_eyePt.y, 
		//	ps->info.emitter_pos.z-g_eyePt.z);

		g.camDist = CameraGetBoundFarDistSq(ps->bbmin+ps->info.emitter_pos, 
			ps->bbmax+ps->info.emitter_pos, 0);//Vec3DDot(&vCamObj,&vCamObj);

		//add 1024 vertices at a time...
		while(numverts > 0)
		{
			s16 thisBlock = numverts > MAX_VERT_BATCH ? MAX_VERT_BATCH : numverts;

			hEFFECT fx = ps->ps->fx ? ps->ps->fx : ps_effect;

			g.numVtx = thisBlock;
			g.numFaces = (thisBlock>>2)<<1;
			g.wParam = (thisBlock&0x0000ffff) | ((i&0x0000ffff)<<16); 

			GFXSceneAdd(g);

			numverts -= thisBlock;
			i += thisBlock;
		}
	}
}

static void _ParSysActiveDestroy(active_ps_t *ps)
{
	if(ps)
	{
		if(ps->particles) MemFree(ps->particles);
		if(ps->verts) MemFree(ps->verts);
		MemFree(ps);
	}
}

/////////////////////////////////////
// Name:	ParSysClear
// Purpose:	
/*
Examines the list of active particle systems. Any particle systems
that have not been updated since the last call to r_ClearParticleSystems()
will be destroyed. This means that as soon as the client code forgets
about a particle system (either on purpose or though a logic error) we
are able to clean it up. By refusing to mark particle systems with no
more active particles as updated, we are able to clean up all the expired
systems as well, even if the caller ignores the return values and continues
to request updates on dead systems.
This function should be called once a frame by the client code to ensure
this behaviour.
*/
// Output:	clear off certain particles
// Return:	none
/////////////////////////////////////
void F_API ParSysClear()
{
	active_ps_t* ps;

	// loop over all the active particle systems
	apsIt itNext;
	for(apsIt it = ps_active.begin(); it != ps_active.end(); it = itNext)
	{
		itNext = it; itNext++;

		ps = (*it).second;

		// If it has been updated since last time, reset it and continue
		if(ps->updated)
		{
			ps->updated = FALSE;
			continue;
		}

		//this guy will DIE!
		_ParSysActiveDestroy(ps);
		ps_active.erase(it);
	}
}

/////////////////////////////////////
// Name:	ParSysUpdate
// Purpose:	
/*
exported function that allows you to update an active
particle system. Returns rtrue is the particle system is
still alive and wants further processing, or rfalse if the
particle system has been killed off. It is safe to forget
all your local state about a particle system when this function
returns false.
*/
// Output:	update given particle
// Return:	FALSE if particle is dead
/////////////////////////////////////
u8 F_API ParSysUpdate(s32 id, gfxParInfo* info)
{
	active_ps_t* ps;
	
	// Looks for the particle system specified and updates the particles in it
	ps = ps_FindActive(id);
	
	// Did we find it?
	if (!ps)
		return FALSE;
	
	// If "updated" is true (we have already updated it), then return true
	if (ps->updated)
		return TRUE;
	
	// We are out of reason to skip this one, so update the particle system
	ps->updated = ps_Update(ps, info);
	return ps->updated;
}

/////////////////////////////////////
// Name:	ParSysStart
// Purpose:	
/*
Creates a new instance of a particle system of the type specified.
The type is the name of the particle system.
You should call ParSysUpdate before the next call to
ParSysClear or the particle system will be deleted.
*/
// Output:	new active particle
// Return:	the particle ID (0 for failure)
/////////////////////////////////////
s32 F_API ParSysStart(const tCHAR *type)
{
	particlesystem_t* parent;
	active_ps_t* ps;
	s16 maxparticles;
	
	// Find the type of particle system requested
	if (!type || type[0] == 0)
		return 0;
	
	// Is there a particle system in this slot

	// oh, go on then, you can have one
	parent = _ParSysGet(type);

	if (parent==NULL)
		return 0;
	
	// figure out how many particles there will be at this systems peak
	// this is min(particlelife, emitterlife) * rate
	// life times are stored in ms, rate is in particles per second, hence the conversion (0.001f)
	if ((parent->emitter_life!=0) && (parent->emitter_life < parent->life))
		maxparticles = (s16)((parent->emitter_life * parent->emitter_rate) / 1000);
	else
		maxparticles = (s16)((parent->life * parent->emitter_rate) / 1000);

	// If we are not going to see any particles, we may as well not bother
	if (maxparticles<1)
		return 0;	
	
	// Allocate a new instance of this particle system and clear it
	ps = (active_ps_t*)MemAlloc(sizeof(active_ps_t));
	
	// init the system with some defaults
	ps->updated = TRUE;
	ps->ps = parent;
	ps->max_particles = maxparticles;

	//copy direction, this can be changed by the user
	ps->direction = parent->direction;

	// allocate and clear the particles
	ps->particles = (particle_t*)MemAlloc(sizeof(particle_t) * maxparticles);

	// allocate and clear vertex and colour arrays
	ps->verts = (psPosClr*)MemAlloc(sizeof(psPosClr)*maxparticles*4);
	
	// Fill the colour array with white

	// note the id
	ps->id = ps_next_active_id++;

	ps_active[ps->id] = ps;
	
	return ps->id;
}

/////////////////////////////////////
// Name:	ParSysGetBound
// Purpose:	get the current particle
//			bound box (worldspace)
// Output:	pMin/Max set
// Return:	none
/////////////////////////////////////
void F_API ParSysGetBound(s32 id, Vec3D *pMin, Vec3D *pMax)
{
	active_ps_t* ps;
	
	// Looks for the particle system specified and updates the particles in it
	ps = ps_FindActive(id);
	
	// Did we find it?
	if (ps)
	{
		*pMin = ps->bbmin;
		*pMax = ps->bbmax;
	}
}

/////////////////////////////////////
// Name:	ParSysSetDir
// Purpose:	set the direction of given
//			active particle
// Output:	direction set
// Return:	none
/////////////////////////////////////
void F_API ParSysSetDir(s32 id, const Vec3D & dir)
{
	active_ps_t* ps;
	
	// Looks for the particle system specified and updates the particles in it
	ps = ps_FindActive(id);
	
	// Did we find it?
	if (ps)
	{
		ps->direction = dir;
	}
}