#include "gfx_i.h"

#include "gfx_effect.h"

#include "GFX\gfx_math.h"

static inline D3DFORMAT _EffectGetFormat(const char *name)
{
	if(stricmp(name, "unknown") == 0)
		return D3DFMT_UNKNOWN;
	else if(stricmp(name, "r8g8b8") == 0)
		return D3DFMT_R8G8B8;
	else if(stricmp(name, "a8r8g8b8") == 0)
		return D3DFMT_A8R8G8B8;
	else if(stricmp(name, "x8r8g8b8") == 0)
		return D3DFMT_X8R8G8B8;
	else if(stricmp(name, "r5g6b5") == 0)
		return D3DFMT_R5G6B5;
	else if(stricmp(name, "x1r5g5b5") == 0)
		return D3DFMT_X1R5G5B5;
	else if(stricmp(name, "a1r5g5b5") == 0)
		return D3DFMT_A1R5G5B5;
	else if(stricmp(name, "a4r4g4b4") == 0)
		return D3DFMT_A4R4G4B4;
	else if(stricmp(name, "r3g3b2") == 0)
		return D3DFMT_R3G3B2;
	else if(stricmp(name, "a8") == 0)
		return D3DFMT_A8;
	else if(stricmp(name, "a8r3g3b2") == 0)
		return D3DFMT_A8R3G3B2;
	else if(stricmp(name, "x4r4g4b4") == 0)
		return D3DFMT_X4R4G4B4;
	else if(stricmp(name, "a2b10g10r10") == 0)
		return D3DFMT_A2B10G10R10;
	else if(stricmp(name, "a8b8g8r8") == 0)
		return D3DFMT_A8B8G8R8;
	else if(stricmp(name, "x8b8g8r8") == 0)
		return D3DFMT_X8B8G8R8;
	else if(stricmp(name, "g16r16") == 0)
		return D3DFMT_G16R16;
	else if(stricmp(name, "a2r10g10b10") == 0)
		return D3DFMT_A2R10G10B10;
	else if(stricmp(name, "a16b16g16r16") == 0)
		return D3DFMT_A16B16G16R16;

	else if(stricmp(name, "a8p8") == 0)
		return D3DFMT_A8P8;
	else if(stricmp(name, "p8") == 0)
		return D3DFMT_P8;

	else if(stricmp(name, "l8") == 0)
		return D3DFMT_L8;
	else if(stricmp(name, "a8l8") == 0)
		return D3DFMT_A8L8;
	else if(stricmp(name, "a4l4") == 0)
		return D3DFMT_A4L4;

	else if(stricmp(name, "v8u8") == 0)
		return D3DFMT_V8U8;
	else if(stricmp(name, "l6v5u5") == 0)
		return D3DFMT_L6V5U5;
	else if(stricmp(name, "x8l8v8u8") == 0)
		return D3DFMT_X8L8V8U8;
	else if(stricmp(name, "q8w8v8u8") == 0)
		return D3DFMT_Q8W8V8U8;
	else if(stricmp(name, "v16u16") == 0)
		return D3DFMT_V16U16;
    else if(stricmp(name, "a2w10v10u10") == 0)
		return D3DFMT_A2W10V10U10;

	else if(stricmp(name, "UYVY") == 0)
		return D3DFMT_UYVY;
	else if(stricmp(name, "R8G8_B8G8") == 0)
		return D3DFMT_R8G8_B8G8;
	else if(stricmp(name, "YUY2") == 0)
		return D3DFMT_YUY2;
	else if(stricmp(name, "G8R8_G8B8") == 0)
		return D3DFMT_G8R8_G8B8;
	else if(stricmp(name, "DXT1") == 0)
		return D3DFMT_DXT1;
	else if(stricmp(name, "DXT2") == 0)
		return D3DFMT_DXT2;
	else if(stricmp(name, "DXT3") == 0)
		return D3DFMT_DXT3;
	else if(stricmp(name, "DXT4") == 0)
		return D3DFMT_DXT4;
	else if(stricmp(name, "DXT5") == 0)
		return D3DFMT_DXT5;
    
	else if(stricmp(name, "D16_LOCKABLE") == 0)
		return D3DFMT_D16_LOCKABLE;
	else if(stricmp(name, "D32") == 0)
		return D3DFMT_D32;
	else if(stricmp(name, "D15S1") == 0)
		return D3DFMT_D15S1;
	else if(stricmp(name, "D24S8") == 0)
		return D3DFMT_D24S8;
	else if(stricmp(name, "D24X8") == 0)
		return D3DFMT_D24X8;
	else if(stricmp(name, "D24X4S4") == 0)
		return D3DFMT_D24X4S4;
	else if(stricmp(name, "D16") == 0)
		return D3DFMT_D16;

	else if(stricmp(name, "D32F_LOCKABLE") == 0)
		return D3DFMT_D32F_LOCKABLE;
	else if(stricmp(name, "D24FS8") == 0)
		return D3DFMT_D24FS8;
    
    else if(stricmp(name, "L16") == 0)
		return D3DFMT_L16;

/*
    D3DFMT_VERTEXDATA           =100,
    D3DFMT_INDEX16              =101,
    D3DFMT_INDEX32              =102,

    D3DFMT_Q16W16V16U16         =110,

    D3DFMT_MULTI2_ARGB8         = MAKEFOURCC('M','E','T','1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    D3DFMT_R16F                 = 111,
    D3DFMT_G16R16F              = 112,
    D3DFMT_A16B16G16R16F        = 113,

    // IEEE s23e8 formats (32-bits per channel)
    D3DFMT_R32F                 = 114,
    D3DFMT_G32R32F              = 115,
    D3DFMT_A32B32G32R32F        = 116,

    D3DFMT_CxV8U8               = 117,*/

	return D3DFMT_UNKNOWN;
}

static const CHAR *mtxBaseStr[FXMTX_MAX] = {"\0", "inverse", "transpose", 
	"inversetranspose"};
static const CHAR *mtxTypeStr[FXT_MAX] = {"world", "view", "projection", 
	"worldview", "viewprojection", "worldviewprojection"};

//parse effect parameters
u32 IgfxEffect::_ParseParam()
{
	HRESULT hr;

	D3DXEFFECT_DESC fxDesc;

	m_effect->GetDesc(&fxDesc);
	
	wstring strPath, strDir;

	if(TESTFLAGS(m_flag, EFFECT_FILE))
		_FXGetDir(m_name, strDir, TRUE);
	else
		strDir=L"\0";

	u32 iAnnot;

	D3DXPARAMETER_DESC ParamDesc;
    D3DXPARAMETER_DESC AnnotDesc;
    D3DXHANDLE hParam;
    D3DXHANDLE hAnnot;
    LPDIRECT3DBASETEXTURE9 pTex = NULL;

	D3DFORMAT renderFmt = D3DFMT_UNKNOWN;

    for(u32 iParam = 0; iParam < fxDesc.Parameters; iParam++)
	{
		tCHAR tName[MAXCHARBUFF];

		LPCSTR pstrName = NULL;
        LPCSTR pstrFunction = NULL;
        LPCSTR pstrTarget = NULL;
        LPCSTR pstrTextureType = NULL;
        s32 Width = D3DX_DEFAULT;
        s32 Height= D3DX_DEFAULT;
        s32 Depth = D3DX_DEFAULT;
		s32 MipLevels = 0;
		D3DFORMAT fmt = D3DFMT_UNKNOWN;
		Vec2D vpSize(1.0f,1.0f);

        hParam = m_effect->GetParameter ( NULL, iParam );
        m_effect->GetParameterDesc( hParam, &ParamDesc );

		//get all permutation of transformations (world,view,proj)
        if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_MATRIX_ROWS || ParamDesc.Class == D3DXPC_MATRIX_COLUMNS ) )
        {
			u8 bDoneMtxSemantic=FALSE;
			CHAR mtxStr[MAXCHARBUFF]={0};

			for(u32 mbi = 0; mbi < FXT_MAX; mbi++)
			{
				for(u32 mbj = 0; mbj < FXMTX_MAX; mbj++)
				{
					strcpy(mtxStr, mtxTypeStr[mbi]);
					strcat(mtxStr, mtxBaseStr[mbj]);

					if(stricmp(ParamDesc.Semantic, mtxStr) == 0)
					{
						m_mtx[mbi][mbj] = hParam;
						bDoneMtxSemantic = TRUE;
						break;
					}
				}

				if(bDoneMtxSemantic) break;
			}
        }

		//get vector handles
		//entering phat code...
        if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_VECTOR ))
        {
			LPCSTR pOBJStr = 0, pSpaceStr = 0;

			///////Get Annotations
			for(iAnnot = 0; iAnnot < ParamDesc.Annotations; iAnnot++ )
			{
				hAnnot = m_effect->GetAnnotation ( hParam, iAnnot );
				m_effect->GetParameterDesc( hAnnot, &AnnotDesc );

				if ( stricmp( AnnotDesc.Name, "object" ) == 0 )
					m_effect->GetString( hAnnot, &pOBJStr );
				else if ( stricmp( AnnotDesc.Name, "space" ) == 0 )
					m_effect->GetString( hAnnot, &pSpaceStr );
			}

            if( stricmp( ParamDesc.Semantic, "materialambient" ) == 0 )
				m_hMatAmbient = hParam;
            else if( stricmp( ParamDesc.Semantic, "materialdiffuse" ) == 0 )
				m_hMatDiffuse = hParam;
            else if( stricmp( ParamDesc.Semantic, "materialspecular" ) == 0 )
				m_hMatSpecular = hParam;
			else if( stricmp( ParamDesc.Semantic, "materialemissive" ) == 0 )
				m_hMatEmissive = hParam;
			else if( stricmp( ParamDesc.Semantic, "viewportpixelsize" ) == 0 )
				m_hVPPixelSize = hParam;
			else if( stricmp( ParamDesc.Semantic, "direction" ) == 0 )
			{
				if ( pOBJStr && stricmp( pOBJStr, "directionallight" ) == 0 )
					m_lDir.dir = hParam;
			}
			else if( stricmp( ParamDesc.Semantic, "position" ) == 0 )
			{
				if ( pOBJStr && stricmp( pOBJStr, "pointlight" ) == 0 )
					m_lPoint.pos = hParam;
			}
			else if( stricmp( ParamDesc.Semantic, "diffuse" ) == 0 )
			{
				if(pOBJStr)
				{
					if ( stricmp( pOBJStr, "directionallight" ) == 0 )
						m_lDir.Diffuse = hParam;
					else if ( stricmp( pOBJStr, "pointlight" ) == 0 )
						m_lPoint.Diffuse = hParam;
				}
				else if(pSpaceStr)
				{
					if ( stricmp( pOBJStr, "material" ) == 0 )
						m_hMatDiffuse = hParam;
				}
			}
			else if( stricmp( ParamDesc.Semantic, "ambient" ) == 0 )
			{
				if(pOBJStr)
				{
					if ( stricmp( pOBJStr, "directionallight" ) == 0 )
						m_lDir.Ambient = hParam;
					else if ( stricmp( pOBJStr, "pointlight" ) == 0 )
						m_lPoint.Ambient = hParam;
				}
				else if(pSpaceStr)
				{
					if ( stricmp( pOBJStr, "material" ) == 0 )
						m_hMatAmbient = hParam;
				}
			}
			else if( stricmp( ParamDesc.Semantic, "specular" ) == 0 )
			{
				if(pOBJStr)
				{
					if ( stricmp( pOBJStr, "directionallight" ) == 0 )
						m_lDir.Specular = hParam;
					else if ( stricmp( pOBJStr, "pointlight" ) == 0 )
						m_lPoint.Specular = hParam;
				}
				else if(pSpaceStr)
				{
					if ( stricmp( pOBJStr, "material" ) == 0 )
						m_hMatSpecular = hParam;
				}
			}
        }

        if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_SCALAR ))
        {
            if( stricmp( ParamDesc.Semantic, "materialpower" ) == 0 )
				m_hMatPower = hParam;
            else if( stricmp( ParamDesc.Semantic, "time" ) == 0 )
				m_hTime = hParam;
			else if( stricmp( ParamDesc.Semantic, "timeelapse" ) == 0 )
				m_hTimeElapse = hParam;
        }

		//texture stuff
		if( ParamDesc.Semantic != NULL && 
            ( ParamDesc.Class == D3DXPC_OBJECT ))
		{
			char bTxtStr[MAXCHARBUFF];
			//check for texture stuff
			for(u32 bI = 0; bI < MAXBASETXT; bI++)
			{
				sprintf(bTxtStr, "tex%d", bI);

				if( stricmp( ParamDesc.Semantic, bTxtStr ) == 0 )
				{
					//this is only for base texture 0
					if(bI == 0)
						SETFLAG(m_flag, EFFECT_BASETEX0);

					m_hBaseTxt[bI] = hParam;
					break;
				}
			}
		}

		for(iAnnot = 0; iAnnot < ParamDesc.Annotations; iAnnot++ )
        {
            hAnnot = m_effect->GetAnnotation ( hParam, iAnnot );
            m_effect->GetParameterDesc( hAnnot, &AnnotDesc );
            if( stricmp( AnnotDesc.Name, "name" ) == 0 || stricmp( AnnotDesc.Name, "resourcename" ) == 0)
                m_effect->GetString( hAnnot, &pstrName );
            else if ( stricmp( AnnotDesc.Name, "function" ) == 0 )
                m_effect->GetString( hAnnot, &pstrFunction );
            else if ( stricmp( AnnotDesc.Name, "target" ) == 0 )
                m_effect->GetString( hAnnot, &pstrTarget );
            else if ( stricmp( AnnotDesc.Name, "width" ) == 0 )
                m_effect->GetInt( hAnnot, (INT*)&Width );
            else if ( stricmp( AnnotDesc.Name, "height" ) == 0 )
                m_effect->GetInt( hAnnot, (INT*)&Height );
            else if ( stricmp( AnnotDesc.Name, "depth" ) == 0 )
                m_effect->GetInt( hAnnot, (INT*)&Depth );
			else if ( stricmp( AnnotDesc.Name, "miplevels" ) == 0 )
				m_effect->GetInt( hAnnot, (INT*)&MipLevels );
            else if( stricmp( AnnotDesc.Name, "type" ) == 0 || stricmp( AnnotDesc.Name, "texturetype" ) == 0 )
                m_effect->GetString( hAnnot, &pstrTextureType );
			else if( stricmp( AnnotDesc.Name, "format" ) == 0 )
			{
				LPCSTR fmtStr;
				m_effect->GetString( hAnnot, &fmtStr );
				fmt = _EffectGetFormat(fmtStr);
			}
			else if( stricmp( AnnotDesc.Name, "viewportdimensions" ) == 0 )
			{
				D3DXVECTOR4 vec;
				m_effect->GetVector( hAnnot, &vec);
				vpSize.x = vec.x;
				vpSize.y = vec.y;
			}
        }

		//This is for Render Targets
		if( m_pScn
			&& ParamDesc.Semantic != NULL 
			&& stricmp( ParamDesc.Semantic, "RENDERCOLORTARGET" ) == 0 )
		{
			fxRenderTxt rTxt;
			u32 w,h;

			if(Width != D3DX_DEFAULT && Height != D3DX_DEFAULT)
			{
				w = Width;
				h = Height;

				rTxt.vpSize.x = 0;
				rTxt.vpSize.y = 0;
			}
			else
			{
				w = (u32)(((f32)g_mode.width)*vpSize.x);
				h = (u32)(((f32)g_mode.height)*vpSize.y);

				rTxt.vpSize = vpSize;
			}

			if(fmt == D3DFMT_UNKNOWN)
				fmt = (D3DFORMAT)g_mode.fmt;

			//rTxt.txt
			if(FAILED(D3DXCreateTexture(g_p3DDevice, w, h, MipLevels, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &rTxt.txt)))
				continue;
			
			//copy the data information so we can use it later when reset the device
			mbstowcs(tName, ParamDesc.Name, MAXCHARBUFF);

			rTxt.name = tName;
			rTxt.mipLevel = MipLevels;

			rTxt.txt->GetSurfaceLevel(0, &rTxt.surf);
			rTxt.surf->GetDesc(&rTxt.desc);

			renderFmt = fmt;
			m_pScn->renderTxts.push_back(rTxt);

			//pTex = rTxt.txt;
			//m_effect->SetTexture( m_effect->GetParameter( NULL, iParam ), pTex );
            //SAFE_RELEASE( pTex );
		}
		//This is for Render Depth Surfaces
		else if( m_pScn
			&& ParamDesc.Semantic != NULL 
			&& stricmp( ParamDesc.Semantic, "RENDERDEPTHSTENCILTARGET" ) == 0 )
		{
			fxDepthSurf dSurf;

			//get some surface description from the main depth surface
			GFXSURFACE *pRSurf=0;
			g_p3DDevice->GetDepthStencilSurface(&pRSurf);
			pRSurf->GetDesc(&dSurf.desc);
			SAFE_RELEASE(pRSurf);

			u32 w,h;

			if(Width != D3DX_DEFAULT && Height != D3DX_DEFAULT)
			{
				w = Width;
				h = Height;

				dSurf.vpSize.x = 0;
				dSurf.vpSize.y = 0;
			}
			else
			{
				w = (u32)(((f32)g_mode.width)*vpSize.x);
				h = (u32)(((f32)g_mode.width)*vpSize.y);

				dSurf.vpSize = vpSize;
			}

			mbstowcs(tName, ParamDesc.Name, MAXCHARBUFF);

			dSurf.name = tName;

			if(renderFmt == D3DFMT_UNKNOWN)
				renderFmt=(D3DFORMAT)g_mode.fmt;

			if(fmt == D3DFMT_UNKNOWN)
				fmt=dSurf.desc.Format;

			if(FAILED(g_p3DDevice->CreateDepthStencilSurface(w, h, fmt, dSurf.desc.MultiSampleType,
				dSurf.desc.MultiSampleQuality, FALSE, &dSurf.surf, 0)))
				continue;

			//copy the data information so we can use it later when reset the device
			dSurf.surf->GetDesc(&dSurf.desc);
			
			m_pScn->depthSurfs.push_back(dSurf);
		}
		//This is for Loading Textures
		else if( pstrName != NULL )
        {
			mbstowcs(tName, pstrName, MAXCHARBUFF);

            pTex = NULL;
            
			strPath = strDir;
			strPath += L"\\";
			strPath += tName;

			D3DXIMAGE_INFO imgInf;

			//check to see if this is an animated texture
			tCHAR *pStr = GetExtension(strPath.c_str());
			if(pStr && wcsicmp(pStr, L".ini") == 0)
			{
				gfxFXAnimTxt newAnimTxt;
				newAnimTxt.var = m_effect->GetParameter( NULL, iParam );
				newAnimTxt.txt = TextureLoad(strPath.c_str(), 0, 0);

				m_pAnimTxts.push_back(newAnimTxt);

				pTex = 0;
			}
			else
			{
				pTex = _TextureLoad(strPath.c_str(), &imgInf, Width, Height, Depth, MipLevels, fmt, 0);
			}

            // Apply successfully loaded texture to effect
            if( pTex != NULL ) 
            {
				m_effect->SetTexture( m_effect->GetParameter( NULL, iParam ), pTex );
                SAFE_RELEASE( pTex );
            }
        }
		//This is for Generated Textures
		else if( pstrFunction != NULL )
        {
			LPD3DXBUFFER errors=0;

            LPD3DXBUFFER pTextureShader = NULL;

			tCHAR absPath[MAXCHARBUFF]; FSGetPath(m_name, absPath, MAXCHARBUFF);
			FXIncludeManager inc(m_name);
            			
            if( pstrTarget == NULL )
                pstrTarget = "tx_1_0";

			if(TESTFLAGS(m_flag, EFFECT_FILE))
				hr = D3DXCompileShaderFromFile(absPath, 0, &inc, pstrFunction, pstrTarget,
				0, &pTextureShader, &errors, NULL);
			else
			{
				hr = D3DXCompileShader((const char *)m_buff, (u32)strlen((const char *)m_buff), 0, &inc, pstrFunction, 
				pstrTarget, 0, &pTextureShader, &errors, NULL);
			}

			if(errors)
			{
				_FXCompilerErrorDump(m_name, errors);
				SAFE_RELEASE(errors);
			}

            if(SUCCEEDED(hr))
            {
                if( Width == D3DX_DEFAULT )
                    Width = 64;
                if( Height == D3DX_DEFAULT )
                    Height = 64;
                if( Depth == D3DX_DEFAULT )
                    Depth = 1;
				if( fmt == D3DX_DEFAULT )
					fmt = D3DFMT_A8R8G8B8;

                if (pstrTextureType == NULL || stricmp( pstrTextureType, "2D" ) == 0)
				{
					GFXValidateTxtSize((u32*)&Width, (u32*)&Height);

                    LPDIRECT3DTEXTURE9 p2DTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateTexture( g_p3DDevice, Width, Height, 
                        MipLevels, 0, fmt, D3DPOOL_MANAGED, &p2DTex) ) )
                    {
                        if( SUCCEEDED( hr = D3DXFillTextureTX( p2DTex, 
                            (DWORD *)pTextureShader->GetBufferPointer(), NULL, 0 ) ) )
                        {
                            pTex = p2DTex;
                        }
                    }
                }
				else if( stricmp( pstrTextureType, "volume" ) == 0 )
                {
					GFXValidateVolTxtSize((u32*)&Width, (u32*)&Height, (u32*)&Depth);

                    LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateVolumeTexture( g_p3DDevice, 
						Width, Height, Depth, MipLevels, 0, fmt, D3DPOOL_MANAGED, &pVolumeTex) ) )
                    {
                        if( SUCCEEDED( hr = D3DXFillVolumeTextureTX( pVolumeTex, 
                            (DWORD *)pTextureShader->GetBufferPointer(), NULL, 0 ) ) )
                        {
                            pTex = pVolumeTex;
                        }
                    }
                }
                else if( stricmp( pstrTextureType, "cube" ) == 0 )
                {
					GFXValidateCubeTxtSize((u32*)&Width);

                    LPDIRECT3DCUBETEXTURE9 pCubeTex = NULL;
                    if( SUCCEEDED( hr = D3DXCreateCubeTexture( g_p3DDevice, 
                        Width, MipLevels, 0, fmt, D3DPOOL_MANAGED, &pCubeTex) ) )
                    {
                        if( SUCCEEDED( hr = D3DXFillCubeTextureTX( pCubeTex,
                            (DWORD *)pTextureShader->GetBufferPointer(), NULL, 0 ) ) )
                        {
                            pTex = pCubeTex;
                        }
                    }
                }
                
				m_effect->SetTexture( m_effect->GetParameter( NULL, iParam ), pTex );
                SAFE_RELEASE(pTex);
                SAFE_RELEASE(pTextureShader);
            }
        }

		//other stuff?
	}

	/////////////////////////////////////////////////////
	//check for tangent and binormal semantics
	D3DXEFFECT_DESC EffectDesc;
    D3DXHANDLE hTechnique;
    D3DXTECHNIQUE_DESC TechniqueDesc;
    D3DXHANDLE hPass;
    D3DXPASS_DESC PassDesc;

	m_effect->GetDesc(&EffectDesc);
    for(u32 iTech = 0; iTech < EffectDesc.Techniques; iTech++)
    {
        hTechnique = m_effect->GetTechnique(iTech);
        m_effect->GetTechniqueDesc(hTechnique, &TechniqueDesc);
        for(u32 iPass = 0; iPass < TechniqueDesc.Passes; iPass++)
        {
            hPass = m_effect->GetPass(hTechnique, iPass);
            m_effect->GetPassDesc(hPass, &PassDesc);

			UINT NumVSSemanticsUsed;
            D3DXSEMANTIC pVSSemantics[MAXD3DDECLLENGTH];

            if( !PassDesc.pVertexShaderFunction ||
                FAILED( D3DXGetShaderInputSemantics( PassDesc.pVertexShaderFunction, pVSSemantics, &NumVSSemanticsUsed ) ) )
            {
                continue;
            }

            for( UINT iSem = 0; iSem < NumVSSemanticsUsed; iSem++ )
            {
                if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_TANGENT )
					SETFLAG(m_flag, EFFECT_TANGENT);
				if( pVSSemantics[iSem].Usage == D3DDECLUSAGE_BINORMAL )
					SETFLAG(m_flag, EFFECT_BINORMAL);

                if(TESTFLAGS(m_flag, EFFECT_TANGENT) && TESTFLAGS(m_flag, EFFECT_BINORMAL))
					goto DONETANGENTBINORMALCHECK;
            }
        }
    }
DONETANGENTBINORMALCHECK:
	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////
	//check for transparent flag
	BOOL bTransparent = FALSE;
	m_effect->GetBool("transparent", &bTransparent);

	if(bTransparent)
		SETFLAG(m_flag, EFFECT_TRANSPARENT);
	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////
	//get LOD distance
	if(GetParam(0, L"lod") != NULL)
	{
		GetValue("lod", &m_lodDist, sizeof(f32));
		m_lodDist *= m_lodDist;
	}
	else
		m_lodDist = 0;

	//set the LOD up, and get the levels
	if(m_lodDist > 0)
	{
		//format: string lod_lvl="blahh,blahh2,blahh3";
		const char *lodStr = NULL;
		tCHAR lodWStr[MAXCHARBUFF];

		m_effect->GetString("lod_lvl", &lodStr);

		mbstowcs(lodWStr, lodStr, MAXCHARBUFF);

		if(lodStr)
		{
			//get all LOD levels, the technique we set when we render
			tCHAR buff[MAXCHARBUFF];

			tCHAR *pBuff = (tCHAR*)lodWStr;
			
			while(*pBuff != 0)
			{
				GFXHANDLE tech=0;

				ParserReadWordBuff(&pBuff, buff, MAXCHARBUFF, ',');

				tech = GetTechnique(buff);

				if(tech)
				{
					//make sure this technique works!
					if(FAILED(m_effect->ValidateTechnique(tech)))
					{
						GFXHANDLE nTech=0;
						//try to find next technique, we will assume we have a
						//low quality tech. for this LOD
						m_effect->FindNextValidTechnique(tech, &nTech);

						if(nTech)
							m_pLodTech.push_back(nTech);
					}
					else
						m_pLodTech.push_back(tech);
				}
			}
		}
	}
	/////////////////////////////////////////////////////

	return TRUE;
}