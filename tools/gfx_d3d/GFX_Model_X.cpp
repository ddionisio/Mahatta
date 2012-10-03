#include "gfx_i.h"

#include "GFX\gfx_math.h"

#define XMDL_NOSHADOW	(L"none")

//This is for loading X files, we do not support skinned meshes (only static)

u8 g_bnoShadow=FALSE;

//get extra information
void IgfxModel::_XLoadTrans(const tCHAR *filename)
{
	g_bnoShadow=FALSE;

	Vec3D trans(0,0,0), rot(0,0,0), scale(1,1,1);

	tCHAR transFile[MAXCHARBUFF];
	wcscpy(transFile, filename);
	GetExtension(transFile)[0] = 0;
	wcscat(transFile, L".ini");

	hFILE fp = FSOpen(transFile, L"rt");

	tCHAR buff[MAXCHARBUFF];

	if(fp)
	{
		while(!FSEOF(fp))
		{
			if(ParserReadWordFile(fp, buff, MAXCHARBUFF, '=') == '=')
			{
				tCHAR buff2[MAXCHARBUFF];
				ParserReadWordFile(fp, buff2, MAXCHARBUFF, 0);

				if(wcsicmp(buff, L"trans") == 0)
				{
					swscanf(buff2, L"%f,%f,%f", &trans.x, &trans.y, &trans.z);
				}
				else if(wcsicmp(buff, L"rotate") == 0)
				{
					swscanf(buff2, L"%f,%f,%f", &rot.x, &rot.y, &rot.z);

					rot.x = TO_RAD(rot.x);
					rot.y = TO_RAD(rot.y);
					rot.z = TO_RAD(rot.z);
				}
				else if(wcsicmp(buff, L"scale") == 0)
				{
					swscanf(buff2, L"%f,%f,%f", &scale.x, &scale.y, &scale.z);
				}
				else if(wcsicmp(buff, L"shadow") == 0)
				{
					if(wcsicmp(buff2, XMDL_NOSHADOW) == 0)
					{
						g_bnoShadow = TRUE;
					}
					else
					{
						tCHAR rPath[MAXCHARBUFF], meshPath[MAXCHARBUFF];

						wcscpy(rPath, filename);
						GetFilePart(rPath)[0] = 0;
						wcscat(rPath, buff2);
						
						FSGetPath(rPath, meshPath, MAXCHARBUFF);
						
						LPD3DXBUFFER pAdjacencyBuffer = NULL;
						LPD3DXMESH mesh = NULL;

						if(SUCCEEDED(D3DXLoadMeshFromX(meshPath, D3DXMESH_SYSTEMMEM, g_p3DDevice, 
							&pAdjacencyBuffer, NULL, NULL, NULL, &mesh)))
						{
							LPD3DXMESH shMesh;

							mesh->OptimizeInplace(
								D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
								(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL);

							//clone this sucker
							mesh->CloneMeshFVF(mesh->GetOptions(), GFXFVF_XYZ, g_p3DDevice, 
								&shMesh);

							m_pMesh->shMesh = (ID3DXBaseMesh*)shMesh;
						}

						SAFE_RELEASE(mesh);
						SAFE_RELEASE(pAdjacencyBuffer);
					}
				}
				else if(wcsicmp(buff, L"LOD_Begin") == 0)
				{
					m_lodSDist  = (f32) _wtof(buff2);
					m_lodSDist *= m_lodSDist;
				}
				else if(wcsicmp(buff, L"LOD_Distance") == 0)
				{
					m_lodDist = (f32) _wtof(buff2);
					m_lodDist *= m_lodDist;
				}
				else if(wcsicmp(buff, L"LOD_ShadowDistance") == 0)
				{
					m_shadDist = (f32) _wtof(buff2);
					m_shadDist *= m_shadDist;
				}
			}
		}

		FSClose(fp);
	}

	//set model mtx
	Matrix rotMtx, scaleMtx;
	
	MtxScale(&scale, &scaleMtx);
	MtxYawPitchRoll(rot.y, rot.x, rot.z, &rotMtx);

	m_mdlMtx = rotMtx*scaleMtx;

	m_mdlMtx._41 = trans.x;
	m_mdlMtx._42 = trans.y;
	m_mdlMtx._43 = trans.z;
}

/////////////////////////////////////
// Purpose:	load the X file to model mesh
// Output:	pMesh set in mdl
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxModel::LoadX(const tCHAR *filename)
{
	u32 ret = TRUE;

	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
    void*      pVertices = NULL;
    LPD3DXMESH pSysMesh=0, pTempMesh=0;
    HRESULT    hr;

	u32				numMaterials;
    LPD3DXBUFFER    pbufMaterials=0;    // contains both the materials data and the filename strings

	LPD3DXBUFFER    pAdjacencyBuffer = NULL;

	D3DXMATERIAL		*pMats;
	
	Vec3D			mins, maxs;

	//let's allocate the mesh
	m_pMesh = (gfxMesh*)MemAlloc(sizeof(gfxMesh));

	if(!m_pMesh)
	{ LogMsg(LOG_FILE, L"MDLLoadX", L"Unable to allocate pMesh"); return FALSE; }

	//set model transformation
	_XLoadTrans(filename);

	//set texture dir
	tCHAR buff[MAXCHARBUFF]; wcscpy(buff, filename);
	GetFilePart(buff)[0] = 0;
	wstring filePathTXT = buff;

	//load the mesh
	//D3DXLoadMeshFromXInMemory
	hFILE fp = FSOpen(filename, L"rb");

	if(fp)
	{
		if(FSIsArchive(fp))
		{
			hr = D3DXLoadMeshFromXInMemory((void*)FSBufferGetPtr(fp), FSBufferGetSize(fp), 
			D3DXMESH_MANAGED, g_p3DDevice, &pAdjacencyBuffer, &pbufMaterials, NULL, 
			(DWORD*)&numMaterials, &pSysMesh );

			FSClose(fp);
		}
		else
		{
			FSClose(fp);

			tCHAR filepath[MAXCHARBUFF];
			FSGetPath(filename, filepath, MAXCHARBUFF);

			hr = D3DXLoadMeshFromX(filepath, D3DXMESH_MANAGED, g_p3DDevice, 
				&pAdjacencyBuffer, &pbufMaterials, NULL, (DWORD*)&numMaterials, &pSysMesh );
		}
	}

    if(GFXCheckError(hr, L"MDLLoadX"))
	{ LogMsg(LOG_FILE, L"MDLLoadX", L"Error Loading X files"); ret = FALSE; goto DONE_XLOAD; }

	if(numMaterials == 0)
    { LogMsg(LOG_FILE, L"MDLLoadX", L"No Materials"); ret = FALSE; goto DONE_XLOAD; }

	// Lock the vertex buffer, to generate a simple bounding box
    hr = pSysMesh->GetVertexBuffer(&pVB);
    if(FAILED(hr))
	{ ret = FALSE; goto DONE_XLOAD; }

    hr = pVB->Lock(0, 0, &pVertices, 0);
    if(FAILED(hr))
    { ret = FALSE; goto DONE_XLOAD; }

	hr = D3DXComputeBoundingBox((LPD3DXVECTOR3)pVertices, pSysMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(pSysMesh->GetFVF()), (D3DXVECTOR3 *)&mins, (D3DXVECTOR3 *)&maxs);

	if(FAILED(hr))
	{ pVB->Unlock(); ret = FALSE; goto DONE_XLOAD; }

	pVB->Unlock();

	pMats = (D3DXMATERIAL*)pbufMaterials->GetBufferPointer();

	m_pMesh->mins = mins;
	m_pMesh->maxs = maxs;

	//allocate subsets
	m_pMesh->numSubset = numMaterials;
	m_pMesh->subset = (gfxMeshSub *)MemAlloc(sizeof(gfxMeshSub)*numMaterials);

	tCHAR txtPath[MAXCHARBUFF];

	u8 bUseTangent=FALSE, bUseBiNormal=FALSE;

	for(u32 i = 0; i < numMaterials; i++)
	{
		//copy material
		memcpy(&m_pMesh->subset[i].mat, &pMats[i].MatD3D, sizeof(pMats[i].MatD3D));

		if(pMats[i].pTextureFilename)
		{
			wcscpy(txtPath, filePathTXT.c_str());
			mbstowcs(buff, pMats[i].pTextureFilename, MAXCHARBUFF);
			wcscat(txtPath, buff);

			if(wcscmp(GetExtension(txtPath), L".fx") == 0)
			{
				m_pMesh->subset[i].fx = EffectLoad(txtPath); 

				FLAGS fxFlag = ((hIEFFECT)m_pMesh->subset[i].fx)->GetFlag();

				//check for tangent and binormal
				if(TESTFLAGS(fxFlag, EFFECT_TANGENT))
					bUseTangent = TRUE;
				if(TESTFLAGS(fxFlag, EFFECT_BINORMAL))
					bUseBiNormal = TRUE;
			}
			else
			{
				//load the texture
				m_pMesh->subset[i].txt = TextureLoad(txtPath, 0, 0);
			}
		}
	}

	//do we need to make a shadow buffer (if we don't have a shadow mesh)
	if(!m_pMesh->shMesh && !g_bnoShadow)
	{
		LPD3DXMESH shMesh;

		pSysMesh->CloneMeshFVF(pSysMesh->GetOptions(), GFXFVF_XYZ, g_p3DDevice, 
			&shMesh);

		m_pMesh->shMesh = (ID3DXBaseMesh*)shMesh;
	}

	//resize the shadow buffer if we have to
	if(m_pMesh->shMesh)
		GFXShadowInit(m_pMesh->shMesh->GetNumVertices()*8, m_pMesh->shMesh->GetNumVertices()*2, 0);

	// Make sure there are normals, which are required for the tesselation
    // enhancement
    if(!(pSysMesh->GetFVF() & D3DFVF_NORMAL))
    {
        hr = pSysMesh->CloneMeshFVF(pSysMesh->GetOptions(), pSysMesh->GetFVF() | D3DFVF_NORMAL, 
			g_p3DDevice, &pTempMesh);

        if(FAILED(hr))
		{ ret = FALSE; goto DONE_XLOAD; }

        D3DXComputeNormals(pTempMesh, NULL);

        SAFE_RELEASE(pSysMesh);
        pSysMesh = pTempMesh;
		pTempMesh = 0;
    }

	m_fvf = pSysMesh->GetFVF();

	//do we need to put tangent and binormal to the mesh?
	if(bUseTangent || bUseBiNormal)
	{
		D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
        D3DVERTEXELEMENT9 End = D3DDECL_END();
        s32 iElem;
     
        pSysMesh->GetDeclaration(Declaration);
     
        for(iElem=0; Declaration[iElem].Stream != End.Stream; iElem++)
        {   
            if(Declaration[iElem].Usage == D3DDECLUSAGE_TANGENT)
				SETFLAG(m_flag, MODEL_TANGENT);
			if(Declaration[iElem].Usage == D3DDECLUSAGE_BINORMAL)
                SETFLAG(m_flag, MODEL_BINORMAL);
			if(TESTFLAGS(m_flag, EFFECT_TANGENT) && TESTFLAGS(m_flag, EFFECT_BINORMAL))
				break;
        }
     
        // Update Mesh Semantics if changed
        if(!TESTFLAGS(m_flag, EFFECT_TANGENT) || !TESTFLAGS(m_flag, EFFECT_BINORMAL))
        {
			if(!TESTFLAGS(m_flag, EFFECT_TANGENT) && bUseTangent)
			{
				Declaration[iElem].Stream = 0;
				Declaration[iElem].Offset = (WORD)pSysMesh->GetNumBytesPerVertex();
				Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
				Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
				Declaration[iElem].Usage = D3DDECLUSAGE_TANGENT;
				Declaration[iElem].UsageIndex = 0;
				
				iElem++;
				Declaration[iElem] = End;
				
				SETFLAG(m_flag, MODEL_TANGENT);
			}

			if(!TESTFLAGS(m_flag, EFFECT_BINORMAL) && bUseBiNormal)
			{
				Declaration[iElem].Stream = 0;
				Declaration[iElem].Offset = (WORD)pSysMesh->GetNumBytesPerVertex() + (sizeof(f32)*3);
				Declaration[iElem].Type = D3DDECLTYPE_FLOAT3;
				Declaration[iElem].Method = D3DDECLMETHOD_DEFAULT;
				Declaration[iElem].Usage = D3DDECLUSAGE_BINORMAL;
				Declaration[iElem].UsageIndex = 0;

				iElem++;
				Declaration[iElem] = End;

				SETFLAG(m_flag, MODEL_BINORMAL);
			}

            hr = pSysMesh->CloneMesh(D3DXMESH_MANAGED, Declaration, g_p3DDevice, &pTempMesh );
            if(SUCCEEDED(hr))
            {
                SAFE_RELEASE(pSysMesh);
                pSysMesh = pTempMesh;
				pTempMesh = 0;

				hr = D3DXComputeTangent(pSysMesh, 0, bUseTangent ? 0 : D3DX_DEFAULT, 
					bUseBiNormal ? 0 : D3DX_DEFAULT, TRUE, NULL);
            }
        }
	}

	//set the mesh, if we have progressive mesh, we should make them...
	if(m_lodDist > 0)
	{
		DWORD        iPMesh;

		LPD3DXPMESH  pPMesh=0;

		// perform simple cleansing operations on mesh
		if( SUCCEEDED( hr = D3DXCleanMesh( pSysMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), &pTempMesh, 
											(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
		{
			SAFE_RELEASE(pSysMesh);
			pSysMesh = pTempMesh;
			pTempMesh = 0;
		}
		
		//  Perform a weld to try and remove excess vertices
		//    Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
		D3DXWELDEPSILONS Epsilons;
		memset(&Epsilons, 0, sizeof(D3DXWELDEPSILONS));
		hr = D3DXWeldVertices( pSysMesh, 0, &Epsilons, 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL );

		// verify validity of mesh for simplification
		if( FAILED( hr = D3DXValidMesh( pSysMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL ) ) )
		{
			goto LOD_END;
		}

		hr = D3DXGeneratePMesh(pSysMesh, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), 
			NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &pPMesh);
		
		if(SUCCEEDED(hr))
		{
			DWORD		 cVerticesPerMesh;

			DWORD        cVerticesMin = pPMesh->GetMinVertices();
			DWORD        cVerticesMax = pPMesh->GetMaxVertices();

			cVerticesPerMesh = (cVerticesMax - cVerticesMin) / 10;

			m_pMesh->cPMeshes = max(1, (DWORD)ceil((cVerticesMax - cVerticesMin) / (f32)cVerticesPerMesh));
			m_pMesh->pPMeshes = (LPD3DXPMESH*)MemAlloc(sizeof(LPD3DXPMESH)*m_pMesh->cPMeshes);
			
			if(m_pMesh->pPMeshes)
			{
				D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];
				pPMesh->GetDeclaration(Declaration);

				// clone all the separate pmeshes
				for(iPMesh = 0; iPMesh < m_pMesh->cPMeshes; iPMesh++)
				{
					//D3DXMESH_MANAGED | D3DXMESH_VB_SHARE
					hr = pPMesh->ClonePMesh(pPMesh->GetOptions() | D3DXMESH_VB_SHARE, Declaration,  g_p3DDevice, 
						&m_pMesh->pPMeshes[iPMesh]);
					if(FAILED(hr))
						goto LOD_END;

					// trim to appropriate space
					hr = m_pMesh->pPMeshes[iPMesh]->TrimByVertices(cVerticesMin + cVerticesPerMesh * iPMesh, cVerticesMin + cVerticesPerMesh * (iPMesh+1), NULL, NULL);
					if (FAILED(hr))
						goto LOD_END;

					hr = m_pMesh->pPMeshes[iPMesh]->OptimizeBaseLOD(D3DXMESHOPT_VERTEXCACHE, NULL);
					if (FAILED(hr))
						goto LOD_END;
				}

				// set current to be maximum number of vertices
				m_pMesh->iPMeshCur = m_pMesh->cPMeshes - 1;
				hr = m_pMesh->pPMeshes[m_pMesh->iPMeshCur]->SetNumVertices(cVerticesMax);
			}
			else
				hr = E_OUTOFMEMORY;
		}
LOD_END:
		SAFE_RELEASE(pPMesh);

		if (FAILED(hr))
		{
			GFXCheckError(hr, m_name);

			if(m_pMesh->pPMeshes)
			{
				for (iPMesh = 0; iPMesh < m_pMesh->cPMeshes; iPMesh++)
					SAFE_RELEASE(m_pMesh->pPMeshes[iPMesh]);

				MemFree(m_pMesh->pPMeshes); m_pMesh->pPMeshes = 0;
			}

			m_lodDist = 0;
		}
	}
	

	//just make the normal non-LOD mesh
	if(!m_pMesh->pPMeshes)
	{
		m_pMesh->mesh = (ID3DXBaseMesh*)pSysMesh;
		m_pMesh->mesh->AddRef();
	}

DONE_XLOAD:
	SAFE_RELEASE(pVB);
    SAFE_RELEASE(pSysMesh);
	SAFE_RELEASE(pTempMesh);
    SAFE_RELEASE(pbufMaterials);
	SAFE_RELEASE(pAdjacencyBuffer);
	
	return ret;
}

u32 IgfxModel::XGetNumMat()
{
	return m_pMesh->numSubset;
}

hEFFECT IgfxModel::XGetEffect(u32 ind)
{
	if(ind < m_pMesh->numSubset)
		return m_pMesh->subset[ind].fx;
	
	return 0;
}

void IgfxModel::XSetEffect(u32 ind, hEFFECT fx)
{
	if(ind < m_pMesh->numSubset)
	{
		SAFE_RELEASE(m_pMesh->subset[ind].fx);
		m_pMesh->subset[ind].fx = fx;

		if(fx) fx->AddRef();
	}
}

hTXT IgfxModel::XGetTxt(u32 ind, u32 mapInd)
{
	if(ind < m_pMesh->numSubset && mapInd == 0)
		return m_pMesh->subset[ind].txt;

	return 0;
}

void IgfxModel::XSetTxt(u32 ind, u32 mapInd, hTXT txt)
{
	if(ind < m_pMesh->numSubset && mapInd == 0)
	{
		SAFE_RELEASE(m_pMesh->subset[ind].txt);
		m_pMesh->subset[ind].txt = txt;

		if(txt) txt->AddRef();
	}
}

void IgfxModel::XGetMtrl(u32 ind, gfxMaterial *pMtrl)
{
	if(ind < m_pMesh->numSubset && pMtrl)
		*pMtrl = m_pMesh->subset[ind].mat;
}

void IgfxModel::XSetMtrl(u32 ind, const gfxMaterial *pMtrl)
{
	if(ind < m_pMesh->numSubset && pMtrl)
		m_pMesh->subset[ind].mat = *pMtrl;
}

void IgfxModel::XDestroy()
{
	if(m_pMesh)
	{
		u32 i;

		if(m_pMesh->subset)
		{
			for(i = 0; i < m_pMesh->numSubset; i++)
			{
				SAFE_RELEASE(m_pMesh->subset[i].txt);
				SAFE_RELEASE(m_pMesh->subset[i].fx);
			}

			MemFree(m_pMesh->subset);
		}

		SAFE_RELEASE(m_pMesh->mesh);

		SAFE_RELEASE(m_pMesh->shMesh);

		//destroy progressive meshes
		if(m_pMesh->pPMeshes)
		{
			for(i = 0; i < m_pMesh->cPMeshes; i++)
				SAFE_RELEASE(m_pMesh->pPMeshes[i]);

			MemFree(m_pMesh->pPMeshes);
		}

		MemFree(m_pMesh);
	}
}

//compute and get the LOD mesh to display
ID3DXBaseMesh *IgfxModel::XGetLOD(f32 distCamSQ)
{
	ID3DXBaseMesh *retMesh=m_pMesh->mesh;

	if(m_pMesh->pPMeshes)
	{
		DWORD numVtx;

		if(distCamSQ < m_lodSDist)
			numVtx = m_pMesh->pPMeshes[m_pMesh->cPMeshes - 1]->GetMaxVertices();
		else
		{
			f32 t = 1.0f - (distCamSQ/(m_lodSDist+m_lodDist));
			if(t < 0) t = 0; else if(t > 1) t = 1;

			numVtx = (DWORD)(t*(f32)(m_pMesh->pPMeshes[m_pMesh->cPMeshes - 1]->GetMaxVertices()));
		}

		// if current pm valid for desired value, then set the number of vertices directly
		if ((numVtx >= m_pMesh->pPMeshes[m_pMesh->iPMeshCur]->GetMinVertices()) && (numVtx <= m_pMesh->pPMeshes[m_pMesh->iPMeshCur]->GetMaxVertices()))
		{
			m_pMesh->pPMeshes[m_pMesh->iPMeshCur]->SetNumVertices( numVtx );
		}
		else  // search for the right one
		{
			m_pMesh->iPMeshCur = m_pMesh->cPMeshes - 1;

			// look for the correct "bin" 
			while (m_pMesh->iPMeshCur > 0)
			{
				// if number of vertices is less than current max then we found one to fit
				if (numVtx >= m_pMesh->pPMeshes[m_pMesh->iPMeshCur]->GetMinVertices())
					break;

				m_pMesh->iPMeshCur -= 1;
			}

			// set the vertices on the newly selected mesh
			m_pMesh->pPMeshes[m_pMesh->iPMeshCur]->SetNumVertices( numVtx );
		}

		retMesh = (ID3DXBaseMesh *)m_pMesh->pPMeshes[m_pMesh->iPMeshCur];
	}

	return retMesh;
}