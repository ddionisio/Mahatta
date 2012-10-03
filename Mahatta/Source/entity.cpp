#include "engine_local.h"

#include "entity.h"

#include "world.h"

//The entity interface

IEntity::IEntity(const tCHAR *name, u32 type, const tCHAR *modelFile, const EntityVFTable &vfTable, void *initDat)
: Entity(), m_bWrldAttached(0), m_type(type), m_ref(0), m_lastCollided(0), m_bcollideWith(0), m_fx(0),
  m_shadType(ENTITY_SHADOW_NONE), m_shadTxt(0), m_vftbl(vfTable), m_pushVel(0,0,0)
{
	if(name)
		wcscpy(m_name, name);

	memset(&m_trace, 0, sizeof(m_trace)); m_trace.t = 1;

	if(modelFile)
	{
		if(modelFile[0] == '*')
		{
			s32 mdlInd; swscanf(modelFile, L"*%d", &mdlInd);
			SetMDLMap(WorldGetMap(), mdlInd, TRUE);
		}
		else
			SetMDL(modelFile);
	}

	memset(&m_entCull, 0, sizeof(m_entCull));

	//update gfxEntity
	if(WorldGetMap())
		WorldGetMap()->UpdateEnt(&m_entCull, m_loc+m_boundMin, m_loc+m_boundMax);

	if(m_vftbl.Init)
		m_vftbl.Init((hENTITY)this, initDat);
}

void IEntity::AddCollider(hIENTITY ent)
{
	//ent->AddRef();
	m_collideList.push_back(ent);
}

void IEntity::ClearCollideList()
{
	/*if(m_collideList.size() > 0)
	{
		for(EntIt i = m_collideList.begin(); i != m_collideList.end(); i++)
			(*i)->Release();
	}*/

	m_collideList.clear();
}

IEntity::~IEntity()
{
	//if(m_lastCollided) m_lastCollided->Release();

	ClearCollideList();

	if(m_vftbl.Destroy)
		m_vftbl.Destroy((hENTITY)this);

	if(m_fx) { m_fx->Release(); }

	SetShadowType(ENTITY_SHADOW_NONE);
}

void IEntity::AddRef()
{
	m_ref++;
}

s32 IEntity::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Name:	EntCreate
// Purpose:	create a new entity with
//			given name, userdata and
//			callback.
//			optional modelFile to load
//			model before calling ENTITYMSG_INIT
//			if this is "*<num>", then we get
//			the modelind from current map
//			(bounds are automatically set)
//			initDat is passed on
//			callback: ENTITYMSG_INIT
//			The type can be anything you
//			want.  This is later used
//			to sort from lowest type value
//			to highest (collision and such)
// Output:	new entity created
// Return:	the new entity
/////////////////////////////////////
hENTITY EntCreate(const tCHAR *name, u32 type, const tCHAR *modelFile, 
				  const EntityVFTable &vfTable, void *initDat)
{
	hENTITY newEnt = (hENTITY)new IEntity(name, type, modelFile, vfTable, initDat);

	if(!newEnt)
	{ LogMsg(LOG_FILE, L"EntityCreate", L"Unable to allocate new entity"); }

	return newEnt;
}

/////////////////////////////////////
// Purpose:	get the type you set this
//			entity when created.
// Output:	none
// Return:	the type
/////////////////////////////////////
u32 IEntity::GetType()
{
	return m_type;
}

/////////////////////////////////////
// Purpose:	set the entity model
//			this will re-create the
//			object with a new model
//			give a NULL filename to
//			just remove the entity's
//			object and model
// Output:	model set, obj recreated
// Return:	TRUE if success
/////////////////////////////////////
s32 IEntity::SetMDL(const tCHAR *filename)
{
	s32 ret = TRUE;

	//destroy object
	if(m_obj)
		m_obj->Release();

	if(filename)
	{
		hMDL mdl = g_GFX->MDLLoad(filename);

		if(mdl)
		{
			m_obj = g_GFX->OBJCreate(mdl);

			if(m_obj)
			{
				//set location
				m_obj->Translate(m_loc, FALSE);
			}
		}
	}

	return ret;
}

/////////////////////////////////////
// Purpose:	set the entity to a model
//			inside the given map
//			bSetBound if you want to
//			set the bound to map model
// Output:	model index set
// Return:	TRUE if success
/////////////////////////////////////
s32 IEntity::SetMDLMap(hQBSP qbsp, s32 mdlInd, u8 bSetBound)
{
	m_mapMdlID = mdlInd;

	if(m_mapMdlID != -1)
	{
		WorldGetMap()->ModelGetTranslate(m_mapMdlID, &m_loc);

		if(bSetBound)
		{
			WorldGetMap()->RayModelGetBBoxMin(m_mapMdlID, &m_boundMin);
			WorldGetMap()->RayModelGetBBoxMax(m_mapMdlID, &m_boundMax);

			m_boundMin -= m_loc;
			m_boundMax -= m_loc;

			m_boundRadius = WorldGetMap()->RayModelGetRadius(m_mapMdlID);
		}
	}

	return TRUE;
}

//Model and Object Interface Wrapper
//note: all EntMDL* and EntOBJ* only works
//on entities with objects (hOBJ) (not map models)

s32 IEntity::MDLGetAnimID(const tCHAR *name)
{
	if(m_obj)
	{
		hMDL mdl = m_obj->GetModel();

		if(mdl) return mdl->GetAnimID(name);
	}

	return -1;
}

s32 IEntity::MDLGetNumAnim()
{
	if(m_obj)
	{
		hMDL mdl = m_obj->GetModel();

		if(mdl) return mdl->GetNumAnim();
	}

	return 0;
}

s32 IEntity::MDLGetBoneID(const tCHAR *name)
{
	if(m_obj)
	{
		hMDL mdl = m_obj->GetModel();

		if(mdl) return mdl->GetBoneID(name);
	}

	return 0;
}

u32 IEntity::MDLGetNumMaterial()
{
	if(m_obj)
	{
		hMDL mdl = m_obj->GetModel();

		if(mdl) return mdl->GetNumMaterial();
	}

	return 0;
}

/////////////////////////////////////
// Purpose:	set entity Effect, this will
//			override the model's FXs
//			Set to NULL to restore model's
//			FXs
// Output:	object FX override
// Return:	none
/////////////////////////////////////
void IEntity::SetFX(hEFFECT fx)
{
	if(m_fx) { m_fx->Release(); m_fx = 0; }

	if(fx)
	{
		m_fx = fx;
		m_fx->AddRef();
	}
}

/////////////////////////////////////
// Purpose:	open the area portals
//			around this entity
//			Used for doors and such
// Output:	area open/close in map
// Return:	none
/////////////////////////////////////
void IEntity::SetAreaPortalState(u8 open)
{
	SetAreaPortalState(WorldGetMap(), open);
}

void IEntity::SetAreaPortalState(hQBSP qbsp, u8 open)
{
	if(qbsp)
		qbsp->EntSetAreaPortalState(&m_entCull, 
		m_loc+m_boundMin, m_loc+m_boundMax, open);
}

/////////////////////////////////////
// Purpose:	get the last trace from
//			collision
// Output:	none
// Return:	the trace
/////////////////////////////////////
void IEntity::GetLastTrace(gfxTrace *pTraceOut)
{
	memcpy(pTraceOut, &m_trace, sizeof(m_trace));
}

/////////////////////////////////////
// Purpose:	get the last entity that
//			collided to us from prev.
//			update.
// Output:	none
// Return:	the entity ID
/////////////////////////////////////
hENTITY IEntity::GetLastCollide()
{
	return (hENTITY)m_lastCollided;
}

///////////////////////////////////////
//Entity Movement Functions:

void IEntity::MoveDir(f32 s)
{
	m_vel += (s*m_spd*EngineGetTick())*m_dir;
}

void IEntity::MoveVec(const Vec3D & v, f32 s)
{
	m_vel += (s*m_spd*EngineGetTick())*v;
}

void IEntity::MoveStrafe(f32 s)
{
	//get the strafe vector
	Vec3D uVec(0,1,0);
	Vec3D sVec;
	Vec3DCross(&m_dir, &uVec, &sVec);
	g_GFX->Vec3DNormal(&sVec, &sVec);

	MoveVec(sVec, s);
}

#define SHAD_BLOB_NAME	(L"shadow_blob_texture")

void shadBlobTxtCB(hTXT txt)
{
	//now calculate circle stuff
	u8 *gooks, *gooksWalk;

	s32		xOff, yOff, Value, x, y, pitch;

	f64 TempValue;

	if(txt->Lock(0, 0, 0, &pitch, (void**)&gooks))
	{
		for (y = 0; y < txt->GetHeight(); y++)
		{
			gooksWalk = gooks + y*pitch;
			//BYTE *gooksWalker = &BOARDPOS(gooks, y, 0, Width);
			for (x = 0; x < txt->GetWidth(); x++)
			{
				xOff = x - txt->GetWidth() / 2;
				yOff = y - txt->GetHeight() / 2;
				TempValue = sqrtf(xOff * xOff + yOff * yOff) / (txt->GetWidth() / 2);
				// You can run TempValue through whatever transforms you want here.
				// Here I've chosen to make the solid center a bit larger by having
				// TempValue stay zero over a larger range while still hitting 1.0
				// at the same point.
				TempValue = (TempValue - 0.1) * (1.0 / 0.9);
				
				Value = 255 - (s32)(TempValue * 255 + 0.5);
				if (Value < 0)
					Value = 0;
				if (Value > 255)
					Value = 255;

				gooksWalk[3] = Value;
				gooksWalk[2] = 1;
				gooksWalk[1] = 1;
				gooksWalk[0] = 1;
				
				gooksWalk+=4;
			}
		}

		txt->Unlock(0);
	}
}

//set shadow type
void IEntity::SetShadowType(eEntShadow shadowType)
{
	m_shadType = shadowType;

	switch(shadowType)
	{
	case ENTITY_SHADOW_NONE:
		if(m_obj)
			m_obj->EnableShadow(FALSE);
		if(m_shadTxt) { m_shadTxt->Release(); m_shadTxt = 0; }
		break;

	case ENTITY_SHADOW_BLOB:
		if(m_obj)
			m_obj->EnableShadow(FALSE);
		if(!m_shadTxt)
		{
			m_shadTxt = g_GFX->TextureSearchName(SHAD_BLOB_NAME);

			if(!m_shadTxt)
				m_shadTxt = g_GFX->TextureCreate(SHAD_BLOB_NAME, 64,64, 1, 0, GFXFMT_A8R8G8B8, shadBlobTxtCB);
			else
				m_shadTxt->AddRef();
		}

		break;

	case ENTITY_SHADOW_VOLUME:
		if(m_obj)
			m_obj->EnableShadow(TRUE);
		if(m_shadTxt) { m_shadTxt->Release(); m_shadTxt = 0; }
		break;
	}
}

///////////////////////////////////////
//These are only used by the engine:

s32 IEntity::Render(hQBSP qbsp, const Vec3D & camLoc)
{
	//check for culling
	if(qbsp && qbsp->CullEnt(&m_entCull))
		return TRUE;

	//render
	//don't render entity object that is attached, 
	//it will be rendered by the parent object from another entity
	if(m_obj && !m_obj->IsAttached())
		m_obj->Display(m_fx, (void*)this);
	else if(qbsp && m_mapMdlID > 0)
		qbsp->ModelRender(m_mapMdlID, camLoc, m_shadType == ENTITY_SHADOW_VOLUME ? TRUE : FALSE);

	//render bounding box?
	if(EngineFlagCheck(ENGINE_FLAG_SHOWBBOX))
	{
		g_GFX->GFXRenderBoundBox(&(m_boundMin+m_loc), &(m_boundMax+m_loc), 0xffffffff);
	}

	//call callback
	if(m_vftbl.Render)
		m_vftbl.Render((hENTITY)this, camLoc);

	if(m_shadTxt)
	{
		f32 radius = fabsf(m_boundMax.x-m_boundMin.x);

		gfxTrace trace;
			
		Vec3D dst = m_loc; dst.y -= 1000.0f;

		qbsp->Collision(&m_boundMin, &m_boundMax, m_loc, dst, &trace, 1.0f);

		dst = (m_loc + trace.t*(dst-m_loc))+(trace.norm*0.1f);

		Matrix blobMtx(radius,0,0,dst.x,  0,0,radius,dst.y,  0,-radius,0,dst.z,  0,0,0,1);
		m_shadTxt->Blt3D(&blobMtx, 0);
	}

	return TRUE;
}

s32 IEntity::Update(hQBSP qbsp)
{
	static u8 bIsChild;

	bIsChild = FALSE;

	//////////////////////////////////////////////////////
	//Set public data stuff
	if(m_obj) 
	{ 
		bIsChild = m_obj->IsAttached();

		m_obj->Scale(m_scale, FALSE);

		//set the orientation based on direction
		Matrix axis;
		f32 rotY = g_GFX->Vec2DGetAngle(&Vec2D(m_dir.x, m_dir.z));

		if(m_dir.y != 0)
		{
			f32 rotX = g_GFX->Vec2DGetAngle(&Vec2D(1.0f, -m_dir.y));
			g_GFX->MtxYawPitchRoll(rotY, rotX+1.5707963267948966192313216916398f, 0, &axis);
		}
		else
			g_GFX->MtxRotateY(rotY, &axis);

		m_obj->SetAxisMtx(&axis);

		m_obj->Translate(m_loc, FALSE);
	}
	else if(qbsp && m_mapMdlID > 0)
	{
		qbsp->ModelTranslate(m_mapMdlID, m_loc, FALSE);
	}	

	//update gfxEntity
	//we don't have to update entity's cull info if its object is attached by a parent
	if(qbsp && !bIsChild)
	{
		qbsp->UpdateEnt(&m_entCull, m_loc+m_boundMin, m_loc+m_boundMax);
	}

	//call callback
	if(m_vftbl.Update)
		m_vftbl.Update((hENTITY)this);

	if(m_obj)
		m_obj->Update();

	return TRUE;
}