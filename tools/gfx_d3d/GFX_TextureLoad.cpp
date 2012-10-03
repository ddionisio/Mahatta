#include "gfx_i.h"

//for 2D textures: width/height is used
//for volume textures: width/height/depth is used
//for cube textures: width is used
//width,height,depth,miplvl can be D3DX_DEFAULT
//fmt can be D3DFMT_UNKNOWN
//clrKey = 0 for no color key
PGFXBASETEXTURE _TextureLoad(const tCHAR *filename, D3DXIMAGE_INFO *pImgInfOut, u32 width, u32 height, u32 depth,
							 u32 miplvl, u32 fmt, u32 clrKey)
{
	if(!pImgInfOut) return 0;

	PGFXBASETEXTURE pTxtOut = 0;

	HRESULT ret = E_FAIL;

	//are we using archive or not?
	hFILE fp = FSOpen(filename, L"rb");

	if(fp)
	{
		if(FSIsArchive(fp)) //load from memory
		{
			void *buffPtr = (void*)FSBufferGetPtr(fp);
			u32 buffS = FSBufferGetSize(fp);

			D3DXGetImageInfoFromFileInMemory(buffPtr, buffS, pImgInfOut);

			if(width == D3DX_DEFAULT) width = pImgInfOut->Width;
			if(height == D3DX_DEFAULT) height = pImgInfOut->Height;
			if(depth == D3DX_DEFAULT) depth = pImgInfOut->Depth;

			switch(pImgInfOut->ResourceType)
			{
			case D3DRTYPE_TEXTURE:
				{
					GFXValidateTxtSize(&width, &height);

					LPDIRECT3DTEXTURE9 txt2D;

					ret = D3DXCreateTextureFromFileInMemoryEx(g_p3DDevice, buffPtr, buffS,
							width, height, //width/height
							miplvl,				//mip level
							0,							
							(D3DFORMAT)fmt,				//pixel format (use file)
							D3DPOOL_MANAGED,			//D3DPOOL_DEFAULT
							D3DX_DEFAULT,				//filter         (we should let user do this...maybe...)
							//D3DX_FILTER_LINEAR,
							D3DX_DEFAULT,				//mip-map filter (we should let user do this...maybe...)
							clrKey,					//the color keying
							0,							//src info, whatever the hell that is
							0,							//palette info
							&txt2D);

					pTxtOut = txt2D;
				}
				break;

			case D3DRTYPE_VOLUMETEXTURE:
				{
					GFXValidateVolTxtSize(&width, &height, &depth);

					LPDIRECT3DVOLUMETEXTURE9 txtVol;
					
					ret = D3DXCreateVolumeTextureFromFileInMemoryEx( g_p3DDevice, buffPtr, buffS,
                    width, height, depth, miplvl, 0, (D3DFORMAT)fmt, D3DPOOL_MANAGED,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &txtVol );

					pTxtOut = txtVol;
				}
				break;

			case D3DRTYPE_CUBETEXTURE:
				{
					GFXValidateCubeTxtSize(&width);

					LPDIRECT3DCUBETEXTURE9 txtCube;

					LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
					ret = D3DXCreateCubeTextureFromFileInMemoryEx( g_p3DDevice, buffPtr, buffS,
						width, miplvl, 0, (D3DFORMAT)fmt, D3DPOOL_MANAGED,
						D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &txtCube );

					pTxtOut = txtCube;
				}
				break;
			}

			FSClose(fp);
		}
		else				//load from file
		{
			FSClose(fp);

			tCHAR filepath[MAXCHARBUFF];

			//get absolute path
			FSGetPath(filename, filepath, MAXCHARBUFF);

			D3DXGetImageInfoFromFile(filepath, pImgInfOut);

			if(width == D3DX_DEFAULT) width = pImgInfOut->Width;
			if(height == D3DX_DEFAULT) height = pImgInfOut->Height;
			if(depth == D3DX_DEFAULT) depth = pImgInfOut->Depth;

			switch(pImgInfOut->ResourceType)
			{
			case D3DRTYPE_TEXTURE:
				{
					GFXValidateTxtSize(&width, &height);

					LPDIRECT3DTEXTURE9 txt2D;

					ret = D3DXCreateTextureFromFileEx(g_p3DDevice, filepath,
							width, height, //width/height
							miplvl,				//mip level
							0,							
							(D3DFORMAT)fmt,				//pixel format (use file)
							D3DPOOL_MANAGED,			//D3DPOOL_DEFAULT
							D3DX_DEFAULT,				//filter         (we should let user do this...maybe...)
							//D3DX_FILTER_LINEAR,
							D3DX_DEFAULT,				//mip-map filter (we should let user do this...maybe...)
							clrKey,					//the color keying
							0,							//src info, whatever the hell that is
							0,							//palette info
							&txt2D);

					pTxtOut = txt2D;
				}
				break;

			case D3DRTYPE_VOLUMETEXTURE:
				{
					GFXValidateVolTxtSize(&width, &height, &depth);

					LPDIRECT3DVOLUMETEXTURE9 txtVol;
					
					ret = D3DXCreateVolumeTextureFromFileEx(g_p3DDevice, filepath,
                    width, height, depth, miplvl, 0, (D3DFORMAT)fmt, D3DPOOL_MANAGED,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &txtVol );

					pTxtOut = txtVol;
				}
				break;

			case D3DRTYPE_CUBETEXTURE:
				{
					GFXValidateCubeTxtSize(&width);

					LPDIRECT3DCUBETEXTURE9 txtCube;

					LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
					ret = D3DXCreateCubeTextureFromFileEx( g_p3DDevice, filepath,
						width, miplvl, 0, (D3DFORMAT)fmt, D3DPOOL_MANAGED,
						D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &txtCube );

					pTxtOut = txtCube;
				}
				break;
			}
		}
	}

	pImgInfOut->Width = width;
	pImgInfOut->Height = height;
	pImgInfOut->Depth = depth;

	return pTxtOut;
}