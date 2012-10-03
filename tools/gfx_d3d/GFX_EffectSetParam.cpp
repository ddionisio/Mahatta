#include "gfx_i.h"

#include "GFX\gfx_math.h"

/////////////////////////////////////
// Purpose:	set the global params
//			ie. All common params
//			to vtx. buffers
//			(e.g.: projection mtx...etc.)
// Output:	effect params set
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxEffect::SetGlobalParams()
{
	u32 i;

	//FIXME!: optimize?

	//set view matrix
	for(i = 0; i < FXMTX_MAX; i++)
	{
		if(m_mtx[FXT_VIEW][i])
			m_effect->SetMatrix(m_mtx[FXT_VIEW][i], (D3DXMATRIX*)&g_mtxView[i]);
	}

	//set projectile matrix
	for(i = 0; i < FXMTX_MAX; i++)
	{
		if(m_mtx[FXT_PROJ][i])
			m_effect->SetMatrix(m_mtx[FXT_PROJ][i], (D3DXMATRIX*)&g_mtxProj[i]);
	}

	//set view*projectile matrix
	for(i = 0; i < FXMTX_MAX; i++)
	{
		if(m_mtx[FXT_VIEWPROJ][i])
			m_effect->SetMatrix(m_mtx[FXT_VIEWPROJ][i], (D3DXMATRIX*)&g_mtxViewProj[i]);
	}

	//set global timer
	if(m_hTime)
		m_effect->SetFloat(m_hTime, g_time);

	if(m_hTimeElapse)
		m_effect->SetFloat(m_hTimeElapse, g_timeElapse);

	//set viewport pixel size
	if(m_hVPPixelSize)
	{
		f32 vpS[2] = {(f32)g_mode.width, (f32)g_mode.height};
		m_effect->SetFloatArray(m_hVPPixelSize, vpS, 2);
	}

	//set animated textures
	for(i = 0; i < m_pAnimTxts.size(); i++)
	{
		if(m_pAnimTxts[i].var && m_pAnimTxts[i].txt)
			SetTexture(m_pAnimTxts[i].var, m_pAnimTxts[i].txt);
	}

	return TRUE;
}

void IgfxEffect::_SetLightParam(const gfxFXLH & lh, hLIGHT l)
{
	if(lh.dir)
	{
		D3DXVECTOR4 v(l->lDat.Direction.x, l->lDat.Direction.y, l->lDat.Direction.z, 0.0f);
		m_effect->SetVector(lh.dir, &v);
	}

	if(lh.pos)
	{
		D3DXVECTOR4 p(l->lDat.Position.x, l->lDat.Position.y, l->lDat.Position.z, 1.0f);
		m_effect->SetVector(lh.pos, &p);
	}

	if(lh.Diffuse)
		m_effect->SetVector(lh.Diffuse, (LPD3DXVECTOR4)&l->lDat.Diffuse);

	if(lh.Ambient)
		m_effect->SetVector(lh.Ambient, (LPD3DXVECTOR4)&l->lDat.Ambient);

	if(lh.Specular)
		m_effect->SetVector(lh.Specular, (LPD3DXVECTOR4)&l->lDat.Specular);
}

/////////////////////////////////////
// Purpose:	set params for effect
//			we are assuming a matrix array
//			of size FXMTX_MAX
//			if bSingleMtx, then there is
//			only the normal world mtx
// Output:	effect params set
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxEffect::SetParams(const Matrix * wrldMtx, u8 bSingleMtx, const gfxMaterial & mtrl, 
					const hLIGHT *pLights, void *userDat, EFFECTDATCB fxCB, u32 wParam, s32 lParam)
{
	u8 i;

	if(bSingleMtx)
	{
		//set world matrix
		if(m_mtx[FXT_WORLD][FXMTX_NORMAL])
			m_effect->SetMatrix(m_mtx[FXT_WORLD][FXMTX_NORMAL], (D3DXMATRIX*)wrldMtx);

		if(m_mtx[FXT_WRLDVIEW][FXMTX_NORMAL]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEW][FXMTX_NORMAL], (D3DXMATRIX*)&((*wrldMtx)*g_mtxView[FXMTX_NORMAL]));

		if(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_NORMAL]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_NORMAL], (D3DXMATRIX*)&((*wrldMtx)*g_mtxViewProj[FXMTX_NORMAL]));
	}
	else
	{
		//set world matrix
		for(i = 0; i < FXMTX_MAX; i++)
		{
			if(m_mtx[FXT_WORLD][i])
				m_effect->SetMatrix(m_mtx[FXT_WORLD][i], (D3DXMATRIX*)&wrldMtx[i]);
		}

		//set world*view matrix
		if(m_mtx[FXT_WRLDVIEW][FXMTX_NORMAL]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEW][FXMTX_NORMAL], (D3DXMATRIX*)&(wrldMtx[FXMTX_NORMAL]*g_mtxView[FXMTX_NORMAL]));
		if(m_mtx[FXT_WRLDVIEW][FXMTX_INVERSE]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEW][FXMTX_INVERSE], (D3DXMATRIX*)&(g_mtxView[FXMTX_INVERSE]*wrldMtx[FXMTX_INVERSE]));
		if(m_mtx[FXT_WRLDVIEW][FXMTX_TRANSPOSE]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEW][FXMTX_TRANSPOSE], (D3DXMATRIX*)&(g_mtxView[FXMTX_TRANSPOSE]*wrldMtx[FXMTX_TRANSPOSE]));
		if(m_mtx[FXT_WRLDVIEW][FXMTX_INVTRANSPOSE]) 
		{
			Matrix wrldviewInv(g_mtxView[FXMTX_INVERSE]*wrldMtx[FXMTX_INVERSE]);
			Matrix wrldviewInvT; MtxTranspose(&wrldviewInv, &wrldviewInvT);
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEW][FXMTX_INVTRANSPOSE], (D3DXMATRIX*)&wrldviewInvT);
		}

		//set world*view*projection matrix
		if(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_NORMAL]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_NORMAL], (D3DXMATRIX*)&(wrldMtx[FXMTX_NORMAL]*g_mtxViewProj[FXMTX_NORMAL]));
		if(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_INVERSE]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_INVERSE], (D3DXMATRIX*)&(g_mtxViewProj[FXMTX_INVERSE]*wrldMtx[FXMTX_INVERSE]));
		if(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_TRANSPOSE]) 
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_TRANSPOSE], (D3DXMATRIX*)&(g_mtxViewProj[FXMTX_TRANSPOSE]*wrldMtx[FXMTX_TRANSPOSE]));
		if(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_INVTRANSPOSE]) 
		{
			Matrix wrldviewprojInv(g_mtxViewProj[FXMTX_INVERSE]*wrldMtx[FXMTX_INVERSE]);
			Matrix wrldviewprojInvT; MtxTranspose(&wrldviewprojInv, &wrldviewprojInvT);
			m_effect->SetMatrix(m_mtx[FXT_WRLDVIEWPROJ][FXMTX_INVTRANSPOSE], (D3DXMATRIX*)&wrldviewprojInvT);
		}
	}

	//set lights (for now, just one point light and one directional light)
	if(pLights)
	{
		u8 set = 0;

		for(i = 0; i < MAXLIGHT; i++)
		{
			if(pLights[i])
			{
				if((set & 1) == 0
					&&
					(pLights[i]->lDat.Type == GFXLIGHT_POINT 
					|| pLights[i]->lDat.Type == GFXLIGHT_SPOT))
				{
					set |= 1;
					_SetLightParam(m_lPoint, pLights[i]);
				}
				else if((set & 2) == 0
					&&
					pLights[i]->lDat.Type == GFXLIGHT_DIRECTIONAL)
				{
					set |= 2;
					_SetLightParam(m_lDir, pLights[i]);
				}
			}

			if(set == 3) break; //we are done getting both lights
		}
	}

	if(m_hMatAmbient != NULL)
		m_effect->SetVector(m_hMatAmbient, (LPD3DXVECTOR4)&mtrl.Ambient);

	if(m_hMatDiffuse != NULL)
		m_effect->SetVector(m_hMatDiffuse, (LPD3DXVECTOR4)&mtrl.Diffuse);

	if(m_hMatEmissive != NULL)
		m_effect->SetVector(m_hMatEmissive, (LPD3DXVECTOR4)&mtrl.Emissive);

	if(m_hMatSpecular != NULL)
		m_effect->SetVector(m_hMatSpecular, (LPD3DXVECTOR4)&mtrl.Specular);

	if(m_hMatPower)
		m_effect->SetFloat(m_hMatPower, mtrl.Power);

	if(fxCB)
		fxCB(this, wParam, lParam);

	if(m_fxCB)
		m_fxCB((hEFFECT)this, userDat);

	return TRUE;
}