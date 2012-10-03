#include "cal3d/cal3d.h"
#include "gfx_i.h"

#include "GFX\gfx_math.h"

IgfxObject::IgfxObject(hIMDL mdl)
: m_ref(0), m_flag(0), m_mdl(mdl), m_alpha(1.0f), m_pCalModel(0), m_pObjMats(0),
  m_wrldTrans(0,0,0), m_wrldScl(1,1,1), m_parent(0), m_parentBoneID(-1)
{
	if(!m_mdl) return;

	memset(m_lights, 0, sizeof(m_lights));

	m_mdl->AddRef();

	CalCoreModel *pCalCoreModel = m_mdl->GetCalCoreModel();

	//create Cal Model instance (if we are using Cal3D model)
	if(pCalCoreModel)
	{
		m_pCalModel = new CalModel;

		if(!m_pCalModel)
		{ LogMsg(LOG_FILE, L"OBJCreate", L"Unable to allocate Cal3D Model"); return; }

		if(!m_pCalModel->create(pCalCoreModel))
		{ 
			tCHAR errStr[MAXCHARBUFF]; mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF);
			LogMsg(LOG_FILE, L"OBJCreate", errStr); 
			return; 
		}

		// attach all meshes to the model
		s32 meshId;
		for(meshId = 0; meshId < pCalCoreModel->getCoreMeshCount(); meshId++)
		{
			m_pCalModel->attachMesh(meshId);
		}

		// set the material set of the whole model
		m_pCalModel->setMaterialSet(0);

		m_pCalModel->update(0);
	}

	//create object materials and derive them from model
	u32 numMat = m_mdl->GetNumMaterial();

	if(numMat > 0)
	{
		m_pObjMats = (gfxObjMat *)MemAlloc(sizeof(gfxObjMat)*numMat);
		if(!m_pObjMats)
		{ LogMsg(LOG_FILE, L"OBJCreate", L"Unable to allocate Object Materials"); return; }
	}

	ResetMaterial();

	MtxIdentity(&m_axis);
}

IgfxObject::~IgfxObject()
{
	if(m_pCalModel)
		delete m_pCalModel;

	if(m_pObjMats && m_mdl)
	{
		u32 numMat = m_mdl->GetNumMaterial();
		for(u32 i = 0; i < numMat; i++)
		{
			SAFE_RELEASE(m_pObjMats[i].txt);
			SAFE_RELEASE(m_pObjMats[i].fx);
		}

		MemFree(m_pObjMats);
	}

	for(list<hOBJ>::iterator cObj = m_objNode.begin(); cObj != m_objNode.end(); cObj++)
	{
		((hIOBJ)(*cObj))->m_parent = 0;

		(*cObj)->Release();
	}

	SAFE_RELEASE(m_mdl);
}

void IgfxObject::AddRef()
{
	m_ref++;
}

s32 IgfxObject::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Name:	OBJCreate
// Purpose:	create object with given
//			model
// Output:	new object created
// Return:	new object
/////////////////////////////////////
hOBJ F_API OBJCreate(hMDL mdl)
{
	hOBJ newOBJ = (hOBJ)new IgfxObject((hIMDL)mdl);

	if(!newOBJ)
	{ LogMsg(LOG_FILE, L"OBJCreate", L"Unable to allocate object"); return 0; }

	return newOBJ;
}

/////////////////////////////////////
// Purpose:	translate given object
// Output:	obj trans.
// Return:	none
/////////////////////////////////////
void IgfxObject::Translate(const Vec3D & trans, u8 bAdd)
{
	if(bAdd)
		m_wrldTrans += trans;
	else
		m_wrldTrans = trans;
}

/////////////////////////////////////
// Purpose:	get obj's trans.
// Output:	pOut set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetTranslate(Vec3D *pOut)
{
	(*pOut) = m_wrldTrans;
}

/////////////////////////////////////
// Purpose:	scale given object
// Output:	obj scale
// Return:	none
/////////////////////////////////////
void IgfxObject::Scale(const Vec3D & scale, u8 bAdd)
{
	if(bAdd)
		m_wrldScl += scale;
	else
		m_wrldScl = scale;
}

/////////////////////////////////////
// Purpose:	get obj's scale
// Output:	pOut set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetScale(Vec3D *pOut)
{
	(*pOut) = m_wrldScl;
}

/////////////////////////////////////
// Purpose:	set the obj's axis (essentially
//			the matrix of obj's orientation)
//			NOTE: usu. treated as a 3x3 matrix.
//				  upperleft portion.
// Output:	axis set
// Return:	none
/////////////////////////////////////
void IgfxObject::SetAxisMtx(const Matrix *mtx)
{
	m_axis = *mtx;
}

/////////////////////////////////////
// Purpose:	get the obj's axis in matrix
//			form.  (essentially 3x3)
// Output:	mtxOut set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetAxisMtx(Matrix *mtxOut)
{
	*mtxOut = m_axis;
}

void IgfxObject::_ComputeWrldMtx()
{	
	m_wrldMtx[FXMTX_NORMAL] = m_mdl->GetMtx();

	m_wrldMtx[FXMTX_NORMAL]._11 *= m_wrldScl.x;
	m_wrldMtx[FXMTX_NORMAL]._22 *= m_wrldScl.y;
	m_wrldMtx[FXMTX_NORMAL]._33 *= m_wrldScl.z;
	
	m_wrldMtx[FXMTX_NORMAL] *= m_axis;

	m_wrldMtx[FXMTX_NORMAL]._41 = m_wrldTrans.x;
	m_wrldMtx[FXMTX_NORMAL]._42 = m_wrldTrans.y;
	m_wrldMtx[FXMTX_NORMAL]._43 = m_wrldTrans.z;
}

/////////////////////////////////////
// Purpose:	calculate the object's
//			world matrix to pMtx
// Output:	pMtx set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetWorldMtx(Matrix *pMtx)
{
	_ComputeWrldMtx();

	*pMtx = m_wrldMtx[FXMTX_NORMAL];
}

/////////////////////////////////////
// Purpose:	interpolates the weight 
//			of an animation cycle to 
//			a new value in a given 
//			amount of time. If the 
//			specified animation cycle 
//			is not active yet, it is 
//			activated.
//
//			weight = The weight to 
//					 interpolate the 
//					 animation cycle to.  
//			delay = The time in seconds 
//					until the new weight 
//					should be reached. 
// Output:	animation set
// Return:	TRUE if success
/////////////////////////////////////
u8 IgfxObject::MixerBlendCycle(s32 animID, f32 weight, f32 delay)
{
	if(!m_pCalModel)
		return FALSE;

	return m_pCalModel->getMixer()->blendCycle(animID, weight, delay);
}

/////////////////////////////////////
// Purpose:	fades an animation cycle 
//			out in a given amount of 
//			time.
//
//			delay = The time in seconds 
//					until the the 
//					animation cycle is 
//					completely removed. 
// Output:	animation set
// Return:	TRUE if success
/////////////////////////////////////
u8 IgfxObject::MixerClearCycle(s32 animID, f32 delay)
{
	if(!m_pCalModel)
		return FALSE;

	return m_pCalModel->getMixer()->clearCycle(animID, delay);
}

/////////////////////////////////////
// Purpose:	executes an animation action
//
//			delayIn = The time in seconds 
//					  until the animation 
//					  action reaches the 
//					  full weight from the 
//					  beginning of its 
//					  execution.  
//			delayOut = The time in seconds 
//					   in which the animation 
//					   action reaches zero 
//					   weight at the end 
//					   of its execution. 
//			bAutolock = pause at the end?
//					   default: FALSE
// Output:	animation set
// Return:	TRUE if success
/////////////////////////////////////
u8 IgfxObject::MixerExecuteAction(s32 animID, f32 delayIn, f32 delayOut, f32 weight, u8 bAutolock)
{
	if(!m_pCalModel)
		return FALSE;

	return m_pCalModel->getMixer()->executeAction(animID, delayIn, delayOut, weight, bAutolock ? true : false);
}

/////////////////////////////////////
// Purpose:	get the duration of the
//			mixer.
// Output:	none
// Return:	the duration
/////////////////////////////////////
f32 IgfxObject::MixerGetDuration()
{
	return m_pCalModel->getMixer()->getAnimationDuration();
}

/////////////////////////////////////
// Purpose:	get the current time of
//			the mixer
// Output:	none
// Return:	
/////////////////////////////////////
f32 IgfxObject::MixerGetTime()
{
	return m_pCalModel->getMixer()->getAnimationTime();
}

/////////////////////////////////////
// Purpose:	get the animation time
//			scale.
// Output:	none
// Return:	the anim. scale
/////////////////////////////////////
f32 IgfxObject::MixerGetScale()
{
	return m_pCalModel->getMixer()->getTimeFactor();
}

/////////////////////////////////////
// Purpose:	set the animation time
//			scale. (usu.: 1)
// Output:	sets the speed of animation
// Return:	none
/////////////////////////////////////
void IgfxObject::MixerSetScale(f32 s)
{
	m_pCalModel->getMixer()->setTimeFactor(s);
}

/////////////////////////////////////
// Purpose:	get the given anim's
//			duration
// Output:	none
// Return:	the duration
/////////////////////////////////////
f32 IgfxObject::AnimGetDuration(s32 animID)
{
	class CalCoreModel *pCalCoreModel = m_mdl->GetCalCoreModel();

	if(!pCalCoreModel)
		return 0;

	CalCoreAnimation *pAnim = pCalCoreModel->getCoreAnimation(animID);

	if(pAnim)
		return pAnim->getDuration();

	return 0;
}

/////////////////////////////////////
// Purpose:	get the current time of
//			given animation
// Output:	none
// Return:	current time
/////////////////////////////////////
f32 IgfxObject::AnimGetTime(s32 animID)
{
	if(!m_pCalModel || m_pCalModel->getMixer()->m_vectorAnimation.size() <= animID)
		return 0;

	return m_pCalModel->getMixer()->m_vectorAnimation[animID]->getTime();
}

/////////////////////////////////////
// Purpose:	get the animation time
//			scale.
// Output:	none
// Return:	the anim. scale
/////////////////////////////////////
f32 IgfxObject::AnimGetScale(s32 animID)
{
	if(!m_pCalModel || m_pCalModel->getMixer()->m_vectorAnimation.size() <= animID)
		return 0;

	return m_pCalModel->getMixer()->m_vectorAnimation[animID]->getTimeFactor();
}

/////////////////////////////////////
// Purpose:	set the animation time
//			scale. (usu.: 1)
// Output:	sets the speed of animation
// Return:	none
/////////////////////////////////////
void IgfxObject::AnimSetScale(s32 animID, f32 s)
{
	if(!m_pCalModel || m_pCalModel->getMixer()->m_vectorAnimation.size() <= animID)
		return;

	m_pCalModel->getMixer()->m_vectorAnimation[animID]->setTimeFactor(s);
}

/////////////////////////////////////
// Purpose:	get the model this obj
//			uses. DO NOT DELETE mdl
// Output:	none
// Return:	the model
/////////////////////////////////////
hMDL IgfxObject::GetModel()
{
	return (hMDL)m_mdl;
}

/////////////////////////////////////
// Purpose:	get the relative rotation
//			of given boneID
// Output:	pQ set
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetRotate(s32 boneID, Quaternion *pQ)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalQuaternion cQuat = pBone->getRotation();

		pQ->x = cQuat.x;
		pQ->y = cQuat.y;
		pQ->z = cQuat.z;
		pQ->w = cQuat.w;
	}
}

/////////////////////////////////////
// Purpose:	get the absolute rotation
//			of given boneID
// Output:	pQ set
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetRotateAbs(s32 boneID, Quaternion *pQ)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalQuaternion cQuat = pBone->getRotationAbsolute();

		pQ->x = cQuat.x;
		pQ->y = cQuat.y;
		pQ->z = cQuat.z;
		pQ->w = cQuat.w;
	}
}

/////////////////////////////////////
// Purpose:	get the rotation
//			to bring a point into the 
//			bone instance space
// Output:	pQ set
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetRotateBoneSpace(s32 boneID, Quaternion *pQ)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalQuaternion cQuat = pBone->getRotationBoneSpace();

		pQ->x = cQuat.x;
		pQ->y = cQuat.y;
		pQ->z = cQuat.z;
		pQ->w = cQuat.w;
	}
}

/////////////////////////////////////
// Purpose:	get the relative translation
//			of given boneID
// Output:	pLoc set
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetTrans(s32 boneID, Vec3D *pLoc)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalVector cVec = pBone->getTranslation();

		pLoc->x = cVec.x;
		pLoc->y = cVec.y;
		pLoc->z = cVec.z;
	}
}

/////////////////////////////////////
// Purpose:	get the relative translation
//			of given boneID
// Output:	pLoc set
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetTransAbs(s32 boneID, Vec3D *pLoc)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalVector cVec = pBone->getTranslationAbsolute();

		pLoc->x = cVec.x;
		pLoc->y = cVec.y;
		pLoc->z = cVec.z;
	}
}

/////////////////////////////////////
// Purpose:	get the translation
//			to bring a point into the 
//			bone instance space
//			of given boneID
// Output:	pLoc set
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetTransBoneSpace(s32 boneID, Vec3D *pLoc)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalVector cVec = pBone->getTranslationBoneSpace();

		pLoc->x = cVec.x;
		pLoc->y = cVec.y;
		pLoc->z = cVec.z;
	}
}

/////////////////////////////////////
// Purpose:	get the given bone's
//			bone space transformation
// Output:	pMtx filled
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneGetTransformMtx(s32 boneID, Matrix *pMtx)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);
		CalMatrix cMtx = pBone->getTransformMatrix();

		pMtx->_11 = cMtx.dxdx; pMtx->_21 = cMtx.dydx; pMtx->_31 = cMtx.dzdx; pMtx->_41 = 0;
		pMtx->_12 = cMtx.dxdy; pMtx->_22 = cMtx.dydy; pMtx->_32 = cMtx.dzdy; pMtx->_42 = 0;
		pMtx->_13 = cMtx.dxdz; pMtx->_23 = cMtx.dydz; pMtx->_33 = cMtx.dzdz; pMtx->_43 = 0;
		pMtx->_14 = 0;         pMtx->_24 = 0;		  pMtx->_34 = 0;         pMtx->_44 = 1;
	}
}

/////////////////////////////////////
// Purpose:	set the relative translation
//			of the given bone
// Output:	bone moved
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneSetTrans(s32 boneID, const Vec3D & loc)
{
	if(m_pCalModel)
	{
		Vec3D trans; BoneGetTrans(boneID, &trans);

		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);

		if(pBone)
		{
			pBone->setTranslation(CalVector(trans.x+loc.x, trans.y+loc.y, trans.z+loc.z));
			pBone->calculateState();
		}
	}
}

/////////////////////////////////////
// Purpose:	set the relative rotation
//			of the given bone
// Output:	bone rotated
// Return:	none
/////////////////////////////////////
void IgfxObject::BoneSetRotate(s32 boneID, const Quaternion & q)
{
	if(m_pCalModel)
	{
		Quaternion quat; BoneGetRotate(boneID, &quat);

		quat *= q;

		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		CalBone *pBone = pSkel->getBone(boneID);

		if(pBone)
		{
			pBone->setRotation(CalQuaternion(quat.x, quat.y, quat.z, quat.w));
			pBone->calculateState();
		}
	}
}

/////////////////////////////////////
// Purpose:	get the object's axis
//			aligned bounding box
//			(model space)
// Output:	pMin and pMax set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetAABB(Vec3D *pMin, Vec3D *pMax)
{
	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		pSkel->getBoneBoundingBox((f32*)(*pMin), (f32*)(*pMax));
	}
	else
	{
		gfxMesh *pMesh = m_mdl->GetXMesh();

		if(pMesh)
		{
			*pMin = pMesh->mins;
			*pMax = pMesh->maxs;
		}
	}
}

/////////////////////////////////////
// Purpose:	get the object's axis
//			aligned bounding box
//			(world space)
// Output:	pMin and pMax set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetAABBWorld(Vec3D *pMin, Vec3D *pMax)
{
	Vec3D sMin, sMax;

	if(m_pCalModel)
	{
		CalSkeleton *pSkel = m_pCalModel->getSkeleton();
		pSkel->getBoneBoundingBox((f32*)(*pMin), (f32*)(*pMax));
	}
	else
	{
		gfxMesh *pMesh = m_mdl->GetXMesh();

		if(pMesh)
		{
			*pMin = pMesh->mins;
			*pMax = pMesh->maxs;
		}
	}

	//set to world
	_ComputeWrldMtx();

	Vec3DTransformCoord(pMin, &m_wrldMtx[FXMTX_NORMAL], &sMin);
	Vec3DTransformCoord(pMax, &m_wrldMtx[FXMTX_NORMAL], &sMax);

	//set the AABB again
	if(sMin.x < sMax.x) { pMin->x = sMin.x; pMax->x = sMax.x; }
	else { pMax->x = sMin.x; pMin->x = sMax.x; }

	if(sMin.y < sMax.y) { pMin->y = sMin.y; pMax->y = sMax.y; }
	else { pMax->y = sMin.y; pMin->y = sMax.y; }

	if(sMin.z < sMax.z) { pMin->z = sMin.z; pMax->z = sMax.z; }
	else { pMax->z = sMin.z; pMin->z = sMax.z; }
}

/////////////////////////////////////
// Purpose:	update given obj.
// Output:	obj update
// Return:	none
/////////////////////////////////////
void IgfxObject::Update()
{
	if(m_pCalModel)
		m_pCalModel->update(g_time);

	//update children
	for(list<hOBJ>::iterator cObj = m_objNode.begin(); cObj != m_objNode.end(); cObj++)
		(*cObj)->Update();
}

/////////////////////////////////////
// Purpose:	checks to see if obj
//			is attached to a parent
//			(is child)
// Output:	none
// Return:	TRUE
/////////////////////////////////////
u8 IgfxObject::IsAttached()
{
	return m_parent ? TRUE : FALSE;
}

/////////////////////////////////////
// Purpose:	detach object from its
//			parent.
// Output:	obj detached
// Return:	none
/////////////////////////////////////
void IgfxObject::RemoveFromParent()
{
	if(m_parent)
	{
		m_parent->m_objNode.remove(this);
		m_parent = 0;
		
		if(m_ref > 0)
			m_ref--; //hopefully we only decrement the ref...
	}
}

/////////////////////////////////////
// Purpose:	detach child from given
//			obj.
// Output:	child detached
// Return:	none
/////////////////////////////////////
void IgfxObject::RemoveChild(hOBJ child)
{
	if(this == ((hIOBJ)child)->m_parent)
		child->RemoveFromParent();
}

/////////////////////////////////////
// Purpose:	add a child to given obj,
//			this will detach the child
//			to prev. parent first.
// Output:	new child added
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxObject::AddChild(hOBJ child, s32 parentJointInd)
{
	//make sure this child is removed from previous parent, if there is one
	child->RemoveFromParent();

	m_objNode.push_back(child);

	((hIOBJ)child)->m_parent = this;
	((hIOBJ)child)->m_parentBoneID = parentJointInd;
	
	child->AddRef();

	return TRUE;
}

/////////////////////////////////////
// Purpose:	set the object alpha channel
//			value ranges from 0~1
// Output:	main diffuse alpha set
// Return:	none
/////////////////////////////////////
void IgfxObject::SetAlpha(f32 alphaVal)
{
	m_alpha = alphaVal;
}

/////////////////////////////////////
// Purpose:	reset the obj's materials
// Output:	set obj's material from
///			its model.
// Return:	none
/////////////////////////////////////
void IgfxObject::ResetMaterial()
{
	u32 numMat = m_mdl->GetNumMaterial();

	for(u32 i = 0; i < numMat; i++)
	{
		SetTxt(i, m_mdl->GetTxt(i, 0));
		SetEffect(i, m_mdl->GetEffect(i));
		m_mdl->GetMaterial(i, &m_pObjMats[i].mat);
	}
}

/////////////////////////////////////
// Purpose:	get the FX of given obj
//			and material index
// Output:	none
// Return:	the FX
/////////////////////////////////////
hEFFECT IgfxObject::GetEffect(u32 ind)
{
	if(ind < m_mdl->GetNumMaterial())
		return m_pObjMats[ind].fx;

	return 0;
}

/////////////////////////////////////
// Purpose:	set the obj's FX
// Output:	this will add ref to fx
// Return:	none
/////////////////////////////////////
void IgfxObject::SetEffect(u32 ind, hEFFECT fx)
{
	if(ind < m_mdl->GetNumMaterial())
	{
		SAFE_RELEASE(m_pObjMats[ind].fx);
		m_pObjMats[ind].fx = fx;

		if(fx) fx->AddRef();
	}
}

/////////////////////////////////////
// Purpose:	get the texture of given
//			obj, material index and map
//			index.
// Output:	
// Return:	
/////////////////////////////////////
hTXT IgfxObject::GetTxt(u32 ind)
{
	if(ind < m_mdl->GetNumMaterial())
		return m_pObjMats[ind].txt;

	return 0;
}

/////////////////////////////////////
// Purpose:	set texture of given obj,
//			material index and map index
// Output:	txt ref add
// Return:	none
/////////////////////////////////////
void IgfxObject::SetTxt(u32 ind, hTXT txt)
{
	if(ind < m_mdl->GetNumMaterial())
	{
		SAFE_RELEASE(m_pObjMats[ind].txt);
		m_pObjMats[ind].txt = txt;

		if(txt) txt->AddRef();
	}
}

/////////////////////////////////////
// Purpose:	get the obj's material
// Output:	pMtrl set
// Return:	none
/////////////////////////////////////
void IgfxObject::GetMaterial(u32 ind, gfxMaterial *pMtrl)
{
	if(ind < m_mdl->GetNumMaterial() && pMtrl)
		*pMtrl = m_pObjMats[ind].mat;
}

/////////////////////////////////////
// Purpose:	set the model's material
// Output:	material set
// Return:	none
/////////////////////////////////////
void IgfxObject::SetMaterial(u32 ind, const gfxMaterial *pMtrl)
{
	if(ind < m_mdl->GetNumMaterial() && pMtrl)
		m_pObjMats[ind].mat = *pMtrl;
}

/////////////////////////////////////
// Purpose:	enable/disable shadow
//			volume for given object
// Output:	set/clear shadow flag
// Return:	none
/////////////////////////////////////
void IgfxObject::EnableShadow(u8 bYes)
{
	if(bYes)
		SETFLAG(m_flag, OBJECT_SHADOW);
	else
		CLEARFLAG(m_flag, OBJECT_SHADOW);
}

/////////////////////////////////////
// Purpose:	enable/disable shadow
//			receiving
// Output:	set/clear shadow flag
// Return:	none
/////////////////////////////////////
void IgfxObject::ReceiveShadow(u8 bYes)
{
	if(bYes)
		CLEARFLAG(m_flag, OBJECT_NORECSHADOW);
	else
		SETFLAG(m_flag, OBJECT_NORECSHADOW);
}