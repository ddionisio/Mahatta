#include "cal3d/cal3d.h"
#include "gfx_i.h"

#include "GFX\gfx_math.h"

static inline void _Cal3DSetVtxDecl(GFXVTXBUFF *vtxBuff, FLAGS flag)
{
	s16 ofs = 0, ind = 0;

	//now let's set the vertex declaration
	D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
    D3DVERTEXELEMENT9 End = D3DDECL_END();

	Declaration[ind].Stream = 0;
	Declaration[ind].Offset = ofs; ofs += sizeof(f32)*3;
	Declaration[ind].Type = D3DDECLTYPE_FLOAT3;
	Declaration[ind].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[ind].Usage = D3DDECLUSAGE_POSITION;
	Declaration[ind].UsageIndex = 0;
	ind++;

	Declaration[ind].Stream = 0;
	Declaration[ind].Offset = ofs; ofs += sizeof(f32)*3;
	Declaration[ind].Type = D3DDECLTYPE_FLOAT3;
	Declaration[ind].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[ind].Usage = D3DDECLUSAGE_NORMAL;
	Declaration[ind].UsageIndex = 0;
	ind++;

	Declaration[ind].Stream = 0;
	Declaration[ind].Offset = ofs; ofs += sizeof(f32)*2;
	Declaration[ind].Type = D3DDECLTYPE_FLOAT2;
	Declaration[ind].Method = D3DDECLMETHOD_DEFAULT;
	Declaration[ind].Usage = D3DDECLUSAGE_TEXCOORD;
	Declaration[ind].UsageIndex = 0;
	ind++;

	ofs = 0;

	if(TESTFLAGS(flag, MODEL_TANGENT))
	{
		Declaration[ind].Stream = 1;
		Declaration[ind].Offset = 0; ofs += sizeof(f32)*4;
		Declaration[ind].Type = D3DDECLTYPE_FLOAT4;
		Declaration[ind].Method = D3DDECLMETHOD_DEFAULT;
		Declaration[ind].Usage = D3DDECLUSAGE_TANGENT;
		Declaration[ind].UsageIndex = 0;
		ind++;

		//binormals are useless without tangents
		if(TESTFLAGS(flag, MODEL_BINORMAL))
		{
			Declaration[ind].Stream = 2;
			Declaration[ind].Offset = 0; ofs += sizeof(f32)*4;
			Declaration[ind].Type = D3DDECLTYPE_FLOAT4;
			Declaration[ind].Method = D3DDECLMETHOD_DEFAULT;
			Declaration[ind].Usage = D3DDECLUSAGE_BINORMAL;
			Declaration[ind].UsageIndex = 0;
			ind++;
		}
	}

	Declaration[ind] = End;

	VtxSetDecl(vtxBuff, Declaration);
}

u32 IgfxModel::_Cal3DCreateBuffer()
{
	if(m_pCalCoreModel == 0)
		return TRUE; //we assume pMesh

	//get the maximum count for vertex and index
	s32 maxInd=0, maxVert=0;

	s32 maxMeshInd=0,curMeshIndC;
	s32 maxMeshVert=0,curMeshVertC;

	for(s32 meshID = 0; meshID < m_pCalCoreModel->getCoreMeshCount(); meshID++)
	{
		// get the core mesh
		CalCoreMesh *pCoreMesh;
		pCoreMesh = m_pCalCoreModel->getCoreMesh(meshID);

		curMeshVertC = 0;
		curMeshIndC = 0;

		//get the maximum vertex and face count and the highest count you can get per submesh
		for(s32 subMeshID = 0; subMeshID < pCoreMesh->getCoreSubmeshCount(); subMeshID++)
		{
			CalCoreSubmesh *pCoreSubMesh;
			pCoreSubMesh = pCoreMesh->getCoreSubmesh(subMeshID);
			
			if(pCoreSubMesh->getFaceCount()*3 > maxInd)
				maxInd = pCoreSubMesh->getFaceCount()*3;

			if(pCoreSubMesh->getVertexCount() > maxVert)
				maxVert = pCoreSubMesh->getVertexCount();

			curMeshVertC += pCoreSubMesh->getVertexCount();
			curMeshIndC += pCoreSubMesh->getFaceCount()*3;
		}

		if(curMeshVertC > maxMeshVert)
			maxMeshVert = curMeshVertC;
		if(curMeshIndC > maxMeshInd)
			maxMeshInd = curMeshIndC;
	}

	if(maxVert > 0 && maxInd > 0)
	{
		//create the vertex buffer
		m_vtxBuff = GFXCreateVtxBuff(sizeof(f32)*8, maxVert, 
			GFXUSAGE_WRITEONLY|GFXUSAGE_DYNAMIC, m_fvf, GFXPOOL_DEFAULT);

		if(!m_vtxBuff)
		{ LogMsg(LOG_FILE, L"_MDLCal3DCreateBuffer: Error creating vertex buffer", m_name); return FALSE; }

		//create index buffer
		GFXCreateIndBuff(sizeof(CalIndex)*maxInd, GFXUSAGE_WRITEONLY|GFXUSAGE_DYNAMIC, 
			sizeof(CalIndex) == 2 ? 16 : 32, GFXPOOL_DEFAULT, &m_indBuff);

		if(!m_indBuff)
		{ LogMsg(LOG_FILE, L"_MDLCal3DCreateBuffer: Error creating index buffer", m_name); return FALSE; }

		gfxCal3D *pCal3DS = (gfxCal3D *)m_pCalCoreModel->getUserData();

		if(pCal3DS)
		{
			pCal3DS->numInd = maxMeshInd;
			pCal3DS->numVtx = maxMeshVert;

			pCal3DS->shadowVtx = (Vec3D*)MemAlloc(sizeof(Vec3D)*maxMeshVert);
			pCal3DS->shadowInd = (u8*)MemAlloc(sizeof(CalIndex)*maxMeshInd);

			//resize the shadow buffer if we have to
			GFXShadowInit(pCal3DS->numVtx*8, pCal3DS->numVtx*2, 0);
		}

		m_pCalCoreModel->setUserData((Cal::UserData)pCal3DS);
	}
	else
	{ LogMsg(LOG_FILE, L"_MDLCal3DCreateBuffer: No vertices or indices", m_name); return FALSE; }

	//u32 GFXAppendVtxBuff(GFXVTXBUFF *vtx, u32 size, u32 count, u32 usage, u32 FVF, GFXPOOL pool);

	//let's see if we want tangents and/or binormals
	if(TESTFLAGS(m_flag, MODEL_TANGENT))
	{
		//at least one of them are included

		//now let's append another stream to our vtx buffer
		//this is a non-FVF vertex buffer
		u32 vret = GFXAppendVtxBuff(m_vtxBuff, sizeof(f32)*4, maxVert, 
			GFXUSAGE_DYNAMIC, 0, GFXPOOL_DEFAULT);

		if(TESTFLAGS(m_flag, MODEL_BINORMAL))
		{
			vret = GFXAppendVtxBuff(m_vtxBuff, sizeof(f32)*4, maxVert, 
				GFXUSAGE_DYNAMIC, 0, GFXPOOL_DEFAULT);
		}

		if(!vret)
		{ LogMsg(LOG_FILE, L"_MDLCal3DCreateBuffer: Error creating tangent/binormal buffer", m_name); return FALSE; }

		_Cal3DSetVtxDecl(m_vtxBuff, m_flag);
	}

	return TRUE;
}

/////////////////////////////////////
// Purpose:	load cal3D model
// Output:	model loaded to given model
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxModel::LoadCal3D(const tCHAR *filename)
{
	//TODO: MAKE CAL3D USE WIDE CHAR!!!

	s32 id, matID=-1;

	//default values
	Vec3D	scale(1.0f,1.0f,1.0f);
	Vec3D	rot(0.0f,0.0f,0.0f);
	Vec3D	trans(0.0f,0.0f,0.0f);
	
	tCHAR errStr[MAXCHARBUFF];

	hFILE fp = FSOpen(filename, L"rt");

	if(!fp) { LogMsg(LOG_FILE, L"MDLLoadCal3D: Unable to load file", filename); return FALSE; }

	tCHAR dir[MAXCHARBUFF]; wcscpy(dir, filename);
	tCHAR *pDir = GetFilePart(dir); if(pDir) *pDir = 0;

	// create a core model instance
	m_pCalCoreModel = new CalCoreModel;

	if(!m_pCalCoreModel)
	{ LogMsg(LOG_FILE, L"MDLLoadCal3D: Unable to allocate CalCoreModel", filename); return FALSE; }

	char sFilename[MAXCHARBUFF]; wcstombs(sFilename, filename, MAXCHARBUFF);
	if(!m_pCalCoreModel->create(string(sFilename)))
	{
		mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF);
		LogMsg(LOG_FILE, filename, errStr);
		return FALSE;
	}

	//set the user data: shadow stuff
	//gfxCal3D *pCal3DS = 0;
	gfxCal3D *pCal3DS = (gfxCal3D *)MemAlloc(sizeof(gfxCal3D));

	if(pCal3DS)
		m_pCalCoreModel->setUserData((Cal::UserData)pCal3DS);
	else //something went wrong!!!
	{ LogMsg(LOG_FILE, L"MDLLoadCal3D: Unable to allocate gfxCal3D", filename); return FALSE; }

	pCal3DS->shadowID = -1;

	hFILE sfp;

	wstring wPath;
	tCHAR wAbsPath[MAXCHARBUFF];
	char sPath[MAXCHARBUFF];

	tCHAR lineS[DMAXCHARBUFF]={0}, *pLineS;

	//let's parse this crap
	//while(!FSEOF(fp))
	for(s32 line = 1; ; line++)
	{
		tCHAR varS[MAXCHARBUFF]={0};

		// stop if we reached the end of file
		if(FSEOF(fp)) break;

		//get line
		ParserReadStringFile(fp, lineS, DMAXCHARBUFF, 0, '\n');

		//skip comment if we find one
		pLineS = lineS;
		if(ParserSkipCharBuff(&pLineS, '#', FALSE) == RETCODE_SUCCESS)
			continue;

		//get variable and its data
		pLineS = lineS;

		if(ParserReadWordBuff(&pLineS, varS, MAXCHARBUFF, '=') == 0)
			continue; //we reached the last of the line...

		//determine what the variable is
		if(wcsicmp(varS, L"LOD_Distance") == 0)
		{
			m_lodDist = (f32)_wtof(pLineS);
			m_lodDist *= m_lodDist;
		}
		else if(wcsicmp(varS, L"LOD_ShadowDistance") == 0)
		{
			m_shadDist = (f32)_wtof(pLineS);
			m_shadDist *= m_shadDist;
		}
		else if(wcsicmp(varS, L"scale") == 0)
		{
			// set rendering scale factor
			scale.x = (f32)_wtof(pLineS);
			scale.y = scale.z = scale.x;
		}
		else if(wcsicmp(varS, L"scaleEX") == 0)
		{
			// set rendering scale factor
			swscanf(pLineS, L"%f,%f,%f", &scale.x, &scale.y, &scale.z);
		}
		else if(wcsicmp(varS, L"rotate") == 0)
		{
			// set rendering rotate factor
			swscanf(pLineS, L"%f,%f,%f", &rot.x, &rot.y, &rot.z);

			rot.x = TO_RAD(rot.x);
			rot.y = TO_RAD(rot.y);
			rot.z = TO_RAD(rot.z);
		}
		else if(wcsicmp(varS, L"trans") == 0)
		{
			// set rendering scale factor
			swscanf(pLineS, L"%f,%f,%f", &trans.x, &trans.y, &trans.z);
		}
		else if(wcsicmp(varS, L"skeleton") == 0) // load core skeleton
		{
			wPath = dir;
			wPath += pLineS;

			sfp = FSOpen(wPath.c_str(), L"rb");

			if(sfp)
			{
				if(FSIsArchive(sfp))
				{
					CalCoreSkeleton *pSkel = CalLoader::loadCoreSkeleton((void*)FSBufferGetPtr(sfp));

					FSClose(sfp);

					if(pSkel)
						m_pCalCoreModel->setCoreSkeleton(pSkel);
					else
					{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF);
					  LogMsg(LOG_FILE, filename, errStr); return FALSE; }
				}
				else
				{
					FSClose(sfp);

					FSGetPath(wPath.c_str(), wAbsPath, MAXCHARBUFF);
					wcstombs(sPath, wAbsPath, MAXCHARBUFF);

					if(!m_pCalCoreModel->loadCoreSkeleton(sPath))
					{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF);
					  LogMsg(LOG_FILE, filename, errStr); return FALSE; }
				}
			}
		}
		else if(wcsicmp(varS, L"animation") == 0)
		{
			id = -1;
			wPath = dir;
			wPath += pLineS;

			sfp = FSOpen(wPath.c_str(), L"rb");

			if(sfp)
			{
				if(FSIsArchive(sfp) && m_pCalCoreModel->getCoreSkeleton())
				{
					CalCoreAnimation *pAnim = CalLoader::loadCoreAnimation((void*)FSBufferGetPtr(sfp), 
						m_pCalCoreModel->getCoreSkeleton());

					FSClose(sfp);

					if(pAnim)
						id = m_pCalCoreModel->addCoreAnimation(pAnim);
				}
				else
				{
					FSClose(sfp);

					FSGetPath(wPath.c_str(), wAbsPath, MAXCHARBUFF);
					wcstombs(sPath, wAbsPath, MAXCHARBUFF);

					// load core animation
					id = m_pCalCoreModel->loadCoreAnimation(sPath);
				}
			}

			if(id == -1)
			{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF); LogMsg(LOG_FILE, filename, errStr); return FALSE; }

			//set helper name
			tCHAR wAnimName[MAXCHARBUFF]={0};
			char sAnimName[MAXCHARBUFF]={0};
			GetFilename(pLineS, wAnimName, MAXCHARBUFF);
			wcstombs(sAnimName, wAnimName, MAXCHARBUFF);

			m_pCalCoreModel->addAnimHelper(sAnimName, id);
		}
		else if(wcsicmp(varS, L"mesh") == 0)
		{
			wPath = dir;
			wPath += pLineS;

			sfp = FSOpen(wPath.c_str(), L"rb");

			if(sfp)
			{
				if(FSIsArchive(sfp))
				{
					CalCoreMesh *pMesh = CalLoader::loadCoreMesh((void*)FSBufferGetPtr(sfp));

					FSClose(sfp);

					if(pMesh)
						m_pCalCoreModel->addCoreMesh(pMesh);
					else
					{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF);
					  LogMsg(LOG_FILE, filename, errStr); return FALSE; }
				}
				else
				{
					FSClose(sfp);

					FSGetPath(wPath.c_str(), wAbsPath, MAXCHARBUFF);
					wcstombs(sPath, wAbsPath, MAXCHARBUFF);

					// load core mesh
					if( m_pCalCoreModel->loadCoreMesh(sPath) == -1)
					{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF); LogMsg(LOG_FILE, filename, errStr); return FALSE; }
				}
			}
		}//pCal3DS
		else if(wcsicmp(varS, L"meshShadow") == 0)
		{
			if(pCal3DS->shadowID >= 0)
			{
				//destroy this mesh???
				continue;
			}

			wPath = dir;
			wPath += pLineS;

			sfp = FSOpen(wPath.c_str(), L"rb");

			if(sfp)
			{
				if(FSIsArchive(sfp))
				{
					CalCoreMesh *pMesh = CalLoader::loadCoreMesh((void*)FSBufferGetPtr(sfp));

					if(pMesh)
						pCal3DS->shadowID = m_pCalCoreModel->addCoreMesh(pMesh);
					else
						pCal3DS->shadowID = -1;

					FSClose(sfp);
				}
				else
				{
					FSClose(sfp);

					FSGetPath(wPath.c_str(), wAbsPath, MAXCHARBUFF);
					wcstombs(sPath, wAbsPath, MAXCHARBUFF);

					// load core mesh
					pCal3DS->shadowID = m_pCalCoreModel->loadCoreMesh(sPath);
				}
			}
			
			if(pCal3DS->shadowID == -1)
			{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF); LogMsg(LOG_FILE, filename, errStr); return FALSE; }
		}
		else if(wcsicmp(varS, L"material") == 0)
		{
			wPath = dir;
			wPath += pLineS;

			sfp = FSOpen(wPath.c_str(), L"rb");

			if(sfp)
			{
				if(FSIsArchive(sfp))
				{
					CalCoreMaterial *pMat = CalLoader::loadCoreMaterial((void*)FSBufferGetPtr(sfp));

					if(pMat)
						matID = m_pCalCoreModel->addCoreMaterial(pMat);
					else
						matID = -1;

					FSClose(sfp);
				}
				else
				{
					FSClose(sfp);

					FSGetPath(wPath.c_str(), wAbsPath, MAXCHARBUFF);
					wcstombs(sPath, wAbsPath, MAXCHARBUFF);

					// load core material
					matID = m_pCalCoreModel->loadCoreMaterial(sPath);
				}
			}

			if(matID == -1)
			{ mbstowcs(errStr, CalError::getLastErrorText().c_str(), MAXCHARBUFF); LogMsg(LOG_FILE, filename, errStr); return FALSE; }

			// get the core material
			CalCoreMaterial *pCoreMaterial;
			pCoreMaterial = m_pCalCoreModel->getCoreMaterial(matID);

			// loop through all maps of the core material
			// and load each textures
			s32 mapId;
			for(mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
			{
				// get the filename of the texture
				string strFilename;
				strFilename = pCoreMaterial->getMapFilename(mapId);

				// load the texture from the file
				if(strFilename[0] != '\0')
				{
					tCHAR wTxtPath[MAXCHARBUFF]; mbstowcs(wTxtPath, strFilename.c_str(), MAXCHARBUFF);

					wPath = dir; wPath += wTxtPath;

					hTXT txt = TextureLoad(wPath.c_str(), FALSE, 0);
					
					// store the texture id in the user data of the map
					pCoreMaterial->setMapUserData(mapId, (Cal::UserData)txt);
				}
			}
		}
		//assume that previous key is material
		else if(wcsicmp(varS, L"materialFX") == 0 && matID != -1)
		{
			wPath = dir;
			wPath += pLineS;

			hEFFECT fx = EffectLoad(wPath.c_str());

			if(!fx)
			{ LogMsg(LOG_FILE, L"MDLLoadCal3D: Error loading FX", filename); return FALSE; }

			// get the core material and set FX as user data
			CalCoreMaterial *pCoreMaterial;
			pCoreMaterial = m_pCalCoreModel->getCoreMaterial(matID);
			pCoreMaterial->setUserData((Cal::UserData)fx);
		}
		else
		{ swprintf(lineS, L"MDLLoadCal3D: (%d): Invalid syntax.", line); LogMsg(LOG_FILE, lineS, filename); }
	}

	FSClose(fp);

	// make one material thread for each material
	// NOTE: this is not the right way to do it, but this viewer can't do the right
	// mapping without further information on the model etc.
	for(matID = 0; matID < m_pCalCoreModel->getCoreMaterialCount(); matID++)
	{
		// create the a material thread
		m_pCalCoreModel->createCoreMaterialThread(matID);

		// initialize the material thread
		m_pCalCoreModel->setCoreMaterialId(matID, 0, matID);
	}

	//prepare model space matrix
	Matrix rotMtx, scaleMtx;
	
	MtxScale(&scale, &scaleMtx);
	MtxYawPitchRoll(rot.y, rot.x, rot.z, &rotMtx);

	m_mdlMtx = rotMtx*scaleMtx;

	m_mdlMtx._41 = trans.x;
	m_mdlMtx._42 = trans.y;
	m_mdlMtx._43 = trans.z;

	//enable tangents for all submeshes, if we need them
	s32 meshId; 
	for(meshId = 0; meshId < m_pCalCoreModel->getCoreMeshCount(); meshId++) 
	{ 
		s32 submeshId; 
		for(submeshId = 0; submeshId < m_pCalCoreModel->getCoreMesh(meshId)->getCoreSubmeshCount() ; submeshId++) 
		{ 
			CalCoreSubmesh * Submesh = m_pCalCoreModel->getCoreMesh(meshId)->getCoreSubmesh(submeshId); 
			s32 matThreadId = Submesh->getCoreMaterialThreadId();
			CalCoreMaterial *pCoreMaterial= m_pCalCoreModel->getCoreMaterial(m_pCalCoreModel->getCoreMaterialId(matThreadId, 0));

			//pCoreMaterial->setUserData
			if(pCoreMaterial)
			{
				hEFFECT fx = (hEFFECT)pCoreMaterial->getUserData();

				//check to see if we want bumpmap
				if(fx)
				{
					FLAGS fxFlag = ((hIEFFECT)fx)->GetFlag();

					if(TESTFLAGS(fxFlag, EFFECT_TANGENT))
						SETFLAG(m_flag, MODEL_TANGENT);
					if(TESTFLAGS(fxFlag, EFFECT_BINORMAL))
						SETFLAG(m_flag, MODEL_BINORMAL);

					if(TESTFLAGS(m_flag, MODEL_TANGENT))
						Submesh->enableTangents(0,TRUE);
				}
			}
		} 
	}

	//create the buffers
	m_fvf = GFXFVF_XYZ | GFXFVF_NORMAL | GFXFVF_TEX1;

	return _Cal3DCreateBuffer();
}

u32 IgfxModel::Cal3DGetNumMat()
{
	return m_pCalCoreModel->getCoreMaterialCount();
}

hEFFECT IgfxModel::Cal3DGetEffect(u32 ind)
{
	if(ind < m_pCalCoreModel->getCoreMaterialCount())
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_pCalCoreModel->getCoreMaterial(ind);

		if(pCoreMaterial)
			return (hEFFECT)pCoreMaterial->getUserData();
	}

	return 0;
}

void IgfxModel::Cal3DSetEffect(u32 ind, hEFFECT fx)
{
	if(ind < m_pCalCoreModel->getCoreMaterialCount())
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_pCalCoreModel->getCoreMaterial(ind);

		if(pCoreMaterial)
		{
			if(pCoreMaterial->getUserData()) ((hEFFECT)pCoreMaterial->getUserData())->Release();

			pCoreMaterial->setUserData((Cal::UserData)fx);

			if(fx) fx->AddRef();
		}
	}
}

hTXT IgfxModel::Cal3DGetTxt(u32 ind, u32 mapInd)
{
	if(ind < m_pCalCoreModel->getCoreMaterialCount())
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_pCalCoreModel->getCoreMaterial(ind);

		if(pCoreMaterial && mapInd < pCoreMaterial->getMapCount())
			return (hTXT)pCoreMaterial->getMapUserData(mapInd);
	}

	return 0;
}

void IgfxModel::Cal3DSetTxt(u32 ind, u32 mapInd, hTXT txt)
{
	if(ind < m_pCalCoreModel->getCoreMaterialCount())
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_pCalCoreModel->getCoreMaterial(ind);

		if(pCoreMaterial && mapInd < pCoreMaterial->getMapCount())
		{
			pCoreMaterial->setMapUserData(mapInd, (Cal::UserData)txt);

			if(txt) txt->AddRef();
		}
	}
}

void IgfxModel::Cal3DGetMtrl(u32 ind, gfxMaterial *pMtrl)
{
	if(ind < m_pCalCoreModel->getCoreMaterialCount())
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_pCalCoreModel->getCoreMaterial(ind);

		if(pCoreMaterial && pMtrl)
		{
			CalCoreMaterial::Color clr;

			clr = pCoreMaterial->getDiffuseColor();

			pMtrl->Diffuse.x = clr.red/255.0f;
			pMtrl->Diffuse.y = clr.green/255.0f;
			pMtrl->Diffuse.z = clr.blue/255.0f;
			pMtrl->Diffuse.w = clr.alpha/255.0f;

			clr = pCoreMaterial->getAmbientColor();

			pMtrl->Ambient.x = clr.red/255.0f;
			pMtrl->Ambient.y = clr.green/255.0f;
			pMtrl->Ambient.z = clr.blue/255.0f;
			pMtrl->Ambient.w = clr.alpha/255.0f;

			clr = pCoreMaterial->getSpecularColor();

			pMtrl->Specular.x = clr.red/255.0f;
			pMtrl->Specular.y = clr.green/255.0f;
			pMtrl->Specular.z = clr.blue/255.0f;
			pMtrl->Specular.w = clr.alpha/255.0f;

			pMtrl->Power = pCoreMaterial->getShininess();

			pMtrl->Emissive.x=0.0f;pMtrl->Emissive.y=0.0f;pMtrl->Emissive.z=0.0f;pMtrl->Emissive.w=0.0f;
		}
	}
}

void IgfxModel::Cal3DSetMtrl(u32 ind, const gfxMaterial *pMtrl)
{
	if(ind < m_pCalCoreModel->getCoreMaterialCount())
	{
		CalCoreMaterial *pCoreMaterial;
		pCoreMaterial = m_pCalCoreModel->getCoreMaterial(ind);

		if(pCoreMaterial)
		{
			CalCoreMaterial::Color clr;

			clr.red = (u8)(pMtrl->Ambient.x*255.0f);
			clr.green = (u8)(pMtrl->Ambient.y*255.0f);
			clr.blue = (u8)(pMtrl->Ambient.z*255.0f);
			clr.alpha = (u8)(pMtrl->Ambient.w*255.0f);
			pCoreMaterial->setAmbientColor(clr);

			clr.red = (u8)(pMtrl->Diffuse.x*255.0f);
			clr.green = (u8)(pMtrl->Diffuse.y*255.0f);
			clr.blue = (u8)(pMtrl->Diffuse.z*255.0f);
			clr.alpha = (u8)(pMtrl->Diffuse.w*255.0f);
			pCoreMaterial->setDiffuseColor(clr);

			clr.red = (u8)(pMtrl->Specular.x*255.0f);
			clr.green = (u8)(pMtrl->Specular.y*255.0f);
			clr.blue = (u8)(pMtrl->Specular.z*255.0f);
			clr.alpha = (u8)(pMtrl->Specular.w*255.0f);
			pCoreMaterial->setSpecularColor(clr);

			pCoreMaterial->setShininess(pMtrl->Power);
		}
	}
}

void IgfxModel::Cal3DDestroy()
{
	if(m_pCalCoreModel)
	{
		// get the core material
		CalCoreMaterial *pCoreMaterial;

		for(s32 matID = 0; matID < m_pCalCoreModel->getCoreMaterialCount(); matID++)
		{
			pCoreMaterial = m_pCalCoreModel->getCoreMaterial(matID);

			// loop through all maps of the core material
			for(s32 mapId = 0; mapId < pCoreMaterial->getMapCount(); mapId++)
			{
				if(pCoreMaterial->getMapUserData(mapId))
					((hTXT)pCoreMaterial->getMapUserData(mapId))->Release();

			}

			//destroy fx
			if(pCoreMaterial->getUserData())
				((hEFFECT)pCoreMaterial->getUserData())->Release();
		}

		//destroy the shadow buffer
		gfxCal3D *shadowBuff = (gfxCal3D *)m_pCalCoreModel->getUserData();
		if(shadowBuff)
		{
			if(shadowBuff->shadowVtx)
				MemFree(shadowBuff->shadowVtx);

			if(shadowBuff->shadowInd)
				MemFree(shadowBuff->shadowInd);

			MemFree(shadowBuff);
		}

		delete m_pCalCoreModel;
	}
}