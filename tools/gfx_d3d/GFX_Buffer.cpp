#include "gfx_i.h"

/*
typedef struct _GFXVTX {
	IDirect3DVertexBuffer9 *vtx;
	u32						stride;	 //size of one vertex element in bytes
	u32						fvf;	 //fvf format
} GFXVTX;

typedef struct _GFXVTXBUFF {
	GFXVTX					*vtxBuff;	 //the vertex buffers
	u32						 numVtxBuff; //number of vertex buffer (each coresponding to a stream)

	PGFXVERTEXDECLARATION    decl;		 //vertex declaration for HLSL
} GFXVTXBUFF;
*/

void GFXDestroyVtxBuff(GFXVTXBUFF *vtx)
{
	if(vtx)
	{
		if(vtx->vtxBuff)
		{
			for(u32 i = 0; i < vtx->vtxBuff->size(); i++)
				SAFE_RELEASE((*vtx->vtxBuff)[i].vtx);

			delete vtx->vtxBuff;
		}

		SAFE_RELEASE(vtx->decl);

		MemFree(vtx);
	}
}

//add a new stream to the given buffer
u32 GFXAppendVtxBuff(GFXVTXBUFF *vtx, u32 size, u32 count, u32 usage, u32 FVF, GFXPOOL pool)
{
	GFXVTX vtxDat;

	HRESULT hr = g_p3DDevice->CreateVertexBuffer(size*count, usage, FVF, (D3DPOOL)pool, &vtxDat.vtx, 0);

	if(!GFXCheckError(hr, L"GFXCreateVtxBuff"))
	{
		vtxDat.stride = size;
		vtxDat.fvf = FVF;
		vtxDat.count = count;

		vtx->vtxBuff->push_back(vtxDat);
	}
	else
		return FALSE;

	return TRUE;
}

GFXVTXBUFF *GFXCreateVtxBuff(u32 size, u32 count, u32 usage, u32 FVF, GFXPOOL pool)
{
	GFXVTXBUFF *newVtx = (GFXVTXBUFF *)MemAlloc(sizeof(GFXVTXBUFF));

	if(!newVtx)
	{ LogMsg(LOG_FILE, L"GFXCreateVtxBuff", L"Unable to allocate new vertex"); return 0; }

	//create vtx buffer array
	newVtx->vtxBuff = new vector<GFXVTX>;

	if(!newVtx->vtxBuff)
	{ LogMsg(LOG_FILE, L"GFXCreateVtxBuff", L"Unable to allocate vertex buffer array"); return 0; }

	if(!GFXAppendVtxBuff(newVtx, size, count, usage, FVF, pool))
	{ GFXDestroyVtxBuff(newVtx); newVtx = 0; }

	return newVtx;
}

//size can only be 16 or 32, other values will default to 16
u32 GFXCreateIndBuff(u32 len, u32 usage, u32 size, GFXPOOL pool, GFXINDBUFF **indBuffOut)
{
	D3DFORMAT format = size == 32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16;

	HRESULT hr = g_p3DDevice->CreateIndexBuffer(len, usage, format, (D3DPOOL)pool, indBuffOut, 0);

	return !GFXCheckError(hr, L"GFXCreateIndBuff");
}

//Vertex

u32 VtxGetCount(GFXVTXBUFF *vtx, u32 ind)
{
	return (*vtx->vtxBuff)[ind].count;
}

u32 VtxGetStride(GFXVTXBUFF *vtx, u32 ind)
{
	return (*vtx->vtxBuff)[ind].stride;
}

//for entire buffer, set ofs and size to 0
u32 VtxLock(GFXVTXBUFF *vtx, u32 ind, u32 ofs, u32 size, void **dat, u32 flags)
{
	HRESULT hr = (*vtx->vtxBuff)[ind].vtx->Lock(ofs, size, dat, flags);

	return SUCCEEDED(hr);
}

void VtxUnlock(GFXVTXBUFF *vtx, u32 ind)
{
	(*vtx->vtxBuff)[ind].vtx->Unlock();
}

void VtxSetDecl(GFXVTXBUFF *vtx, D3DVERTEXELEMENT9 *pDecl)
{
	SAFE_RELEASE(vtx->decl);

	HRESULT hr = g_p3DDevice->CreateVertexDeclaration(pDecl, &vtx->decl);
}

//force to use the first stream with it's FVF
void VtxSetFVF(GFXVTXBUFF *vtx)
{
	g_p3DDevice->SetStreamSource(0, (*vtx->vtxBuff)[0].vtx, 0, (*vtx->vtxBuff)[0].stride);
	g_p3DDevice->SetFVF((*vtx->vtxBuff)[0].fvf);
}

//set the stream source, decl/fvf of given vtx
void VtxSet(GFXVTXBUFF *vtx)
{
	for(u32 i = 0; i < vtx->vtxBuff->size(); i++)
		g_p3DDevice->SetStreamSource(i, (*vtx->vtxBuff)[i].vtx, 0, (*vtx->vtxBuff)[i].stride);

	if(vtx->decl) //assuming we are using shaders
		g_p3DDevice->SetVertexDeclaration(vtx->decl);
	else //default to the first buffer's FVF
		g_p3DDevice->SetFVF((*vtx->vtxBuff)[0].fvf);
}

//you should call this after you're finish with the vtx buffer
void VtxUnset(GFXVTXBUFF *vtx)
{
	for(u32 i = 0; i < vtx->vtxBuff->size(); i++)
		g_p3DDevice->SetStreamSource(i, 0,0,0);
}

//Index

//for entire buffer, set ofs and size to 0
u32 IndLock(GFXINDBUFF *ind, u32 ofs, u32 size, void **dat, u32 flags)
{
	HRESULT hr = ind->Lock(ofs, size, dat, flags);

	return SUCCEEDED(hr);
}

void IndUnlock(GFXINDBUFF *ind)
{
	ind->Unlock();
}