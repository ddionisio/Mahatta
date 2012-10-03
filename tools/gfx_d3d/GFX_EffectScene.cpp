#include "gfx_i.h"

#include "gfx_effect_scene.h"

class fxScnSetClrTgt : public fxScnCommand {
public:
	fxScnSetClrTgt(hIEFFECT fx, const FXScnCmdParse & cmds) : m_renderInd(-1)
	{
		const tCHAR *var;

		var = cmds.GetVal(L"texture");

		if(var)
			m_renderInd = fx->GetRenderTxtInd(var);
	}

	~fxScnSetClrTgt()
	{
	}

	s32 Execute(hIEFFECT fx)
	{
		//set render target
		if(m_renderInd == -1)
			g_p3DDevice->SetRenderTarget(0, fx->GetScn()->pRTSurf);
		else
			g_p3DDevice->SetRenderTarget(0, fx->GetScn()->renderTxts[m_renderInd].surf);

		fx->GetScn()->renderTxtInd = m_renderInd;

		return 1;
	}

private:
	s32		m_renderInd;
};

class fxScnSetDepthSurf : public fxScnCommand {
public:
	fxScnSetDepthSurf(hIEFFECT fx, const FXScnCmdParse & cmds) : m_depthInd(-1)
	{
		const tCHAR *var;

		var = cmds.GetVal(L"texture");

		if(var)
			m_depthInd = fx->GetDepthSurfInd(var);
	}

	~fxScnSetDepthSurf()
	{
	}

	s32 Execute(hIEFFECT fx)
	{
		if(m_depthInd == -1)
			g_p3DDevice->SetDepthStencilSurface(fx->GetScn()->pDepthSurf);
		else
			g_p3DDevice->SetDepthStencilSurface(fx->GetScn()->depthSurfs[m_depthInd].surf);

		return 1;
	}

private:
	s32		m_depthInd;
};

class fxScnClear : public fxScnCommand {
public:
	fxScnClear(const FXScnCmdParse & cmds) : m_flags(0), m_color(0), m_Z(1.0f), m_stencil(0)
	{
		memset(&m_rect, 0, sizeof(m_rect));

		const tCHAR *var;

		var = cmds.GetVal(L"rect");

		if(var)
			swscanf(var, L"%d,%d,%d,%d", &m_rect.left, &m_rect.top, &m_rect.right, &m_rect.bottom);

		var = cmds.GetVal(L"color");

		if(var)
		{
			f32 r=1.0f,g=1.0f,b=1.0f,a=1.0f;
			swscanf(var, L"%f,%f,%f,%f", &r,&g,&b,&a);
			m_color = GFXCOLOR_COLORVALUE(r,g,b,a);

			m_flags |= GFXCLEAR_TARGET;
		}

		var = cmds.GetVal(L"depth");

		if(var)
		{
			swscanf(var, L"%f", &m_Z);

			m_flags |= GFXCLEAR_ZBUFFER;
		}

		var = cmds.GetVal(L"stencil");

		if(var)
		{
			swscanf(var, L"%d", &m_stencil);

			m_flags |= GFXCLEAR_STENCIL;
		}
	}

	~fxScnClear()
	{
	}

	s32 Execute(hIEFFECT fx)
	{
		if(m_rect.left == m_rect.right)
			GFXClear(GFXCLEAR_ALL, 0, 0, m_color, m_Z, m_stencil);
		else
			GFXClear(GFXCLEAR_ALL, 1, &m_rect, m_color, m_Z, m_stencil);

		return 1;
	}

private:
	u32			m_flags;
	iRect		m_rect;
	u32			m_color;
	f32			m_Z;
	u32			m_stencil;
};

typedef enum {
	FXDRAW_BUFFER,
	FXDRAW_OBJECT,
	FXDRAW_OBJECT_OVR,

	FXDRAW_NONE
} FXSCNDRAWTYPE;

class fxScnDraw : public fxScnCommand {
public:
	fxScnDraw(const FXScnCmdParse & cmds) : m_type(FXDRAW_NONE)
	{
		const tCHAR *var;

		var = cmds.GetVal(L"type");

		if(var)
		{
			if(_wcsicmp(L"buffer", var) == 0)
				m_type = FXDRAW_BUFFER;
			else if(_wcsicmp(L"objects", var) == 0)
				m_type = FXDRAW_OBJECT;
			else if(_wcsicmp(L"objects_override", var) == 0)
				m_type = FXDRAW_OBJECT_OVR;
		}
	}

	~fxScnDraw()
	{
	}

	s32 Execute(hIEFFECT fx)
	{
		HRESULT hr;
		u32 numPass;
		PGFXTEXTURE txt=0;
		GFXVTXBUFF *txtVtx;

		g_p3DDevice->BeginScene();

		switch(m_type)
		{
		case FXDRAW_BUFFER:
			//D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE
			hr = fx->GetFX()->Begin((UINT*)&numPass, 0);

			fx->CallUserCB(0);
				
			if(fx->GetScn()->renderTxtInd >= 0)
				txt=fx->GetScn()->renderTxts[fx->GetScn()->renderTxtInd].txt;

			txtVtx = IgfxTexture::sGetVtx3D();

			VtxSet(txtVtx);
			
			g_p3DDevice->SetTexture(0, txt);
			
			//render the previous target to current target
			//remember, we already set this current target a while ago
			//the effect is also taken care of as well
			hr = fx->GetFX()->Pass(fx->GetScn()->pass);
			g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			VtxUnset(txtVtx);
			
			g_p3DDevice->SetTexture(0, 0);

			fx->GetFX()->End();
			break;

		case FXDRAW_OBJECT:
			_GFXSceneRenderNoSort();
			break;

		case FXDRAW_OBJECT_OVR:
			fx->GetFX()->Begin((UINT*)&numPass, 0);
			_GFXSceneRenderNoSortFXOvride(fx, fx->GetScn()->pass);
			fx->GetFX()->End();
			break;

		case FXDRAW_NONE:
			return 0;
		}

		g_p3DDevice->EndScene();

		return 1;
	}

private:
	FXSCNDRAWTYPE m_type;
};

class fxScnSetTech : public fxScnCommand {
public:
	fxScnSetTech(hIEFFECT fx, const FXScnCmdParse & cmds) : m_tech(0)
	{
		const tCHAR *var;

		var = cmds.GetVal(L"name");

		if(var)
		{
			m_tech = fx->GetTechnique(var);

			//if(m_tech && !EffectValidateTechnique(fx, m_tech))
			//	m_tech = 0;
		}
	}

	~fxScnSetTech()
	{
	}

	s32 Execute(hIEFFECT fx)
	{
		u8 ret = 0;

		if(m_tech)
		{
			ret = fx->SetTechnique(m_tech);
			fx->GetScn()->pass = 0;
		}

		return ret;
	}

private:
	GFXHANDLE m_tech;
};

class fxScnSetPass : public fxScnCommand {
public:
	fxScnSetPass(hIEFFECT fx, const FXScnCmdParse & cmds) : m_passName("")
	{
		const tCHAR *var;

		var = cmds.GetVal(L"name");

		if(var)
		{
			char *buff = (char*)MemAlloc(sizeof(char)*(wcslen(var)+1));

			if(buff)
			{
				wcstombs(buff, var, wcslen(var));

				m_passName = buff;

				MemFree(buff);
			}
		}
	}

	~fxScnSetPass()
	{
	}

	s32 Execute(hIEFFECT fx)
	{
		u8 ret = 0;

		if(m_passName.c_str()[0] != 0)
		{
			GFXHANDLE tech = fx->GetFX()->GetCurrentTechnique();

			if(tech)
			{
				GFXHANDLE passH = fx->GetFX()->GetPassByName(tech, m_passName.c_str());

				if(passH)
				{
					D3DXTECHNIQUE_DESC dfxd;
					fx->GetFX()->GetTechniqueDesc(tech, &dfxd);
					for(u32 i = 0; i < dfxd.Passes; i++)
					{
						if(fx->GetFX()->GetPass(tech, i) == passH)
						{
							fx->GetScn()->pass = i;
							break;
						}
					}
				}
			}
		}

		return ret;
	}

private:
	string	m_passName;
};

u32 IgfxEffect::_SceneParseCommand(const tCHAR *buff)
{
	tCHAR param[MAXCHARBUFF];
	tCHAR *pBuff = (tCHAR*)buff;

	fxScnCommand *newCmd;

	FXScnCmdParse cmds;

	//get first word, the command
	if(ParserReadWordBuff(&pBuff, param, MAXCHARBUFF, ' ') != ' ')
	{
		pBuff = (tCHAR*)buff;
		ParserReadWordBuff(&pBuff, param, MAXCHARBUFF, '/');
	}

	cmds.Parse(pBuff);

	if(wcscmp(param, L"scene_commands") == 0)
	{
		//do nothing!
	}
	else if(wcscmp(param, L"/scene_commands") == 0)
	{
		//end of scene commands, it's OVER!
		return FALSE;
	}
	else if(wcscmp(param, L"clear") == 0)
	{
		newCmd = new fxScnClear(cmds);
		m_pScn->commands.push_back(newCmd);
	}
	else if(wcscmp(param, L"draw") == 0)
	{
		newCmd = new fxScnDraw(cmds);
		m_pScn->commands.push_back(newCmd);
	}
	else if(wcscmp(param, L"settechnique") == 0)
	{
		newCmd = new fxScnSetTech(this, cmds);
		m_pScn->commands.push_back(newCmd);
	}
	else if(wcscmp(param, L"setpass") == 0)
	{
		newCmd = new fxScnSetPass(this, cmds);
		m_pScn->commands.push_back(newCmd);
	}
	else if(wcscmp(param, L"setcolortarget") == 0)
	{
		newCmd = new fxScnSetClrTgt(this, cmds);
		m_pScn->commands.push_back(newCmd);
	}
	else if(wcscmp(param, L"setdepthstenciltarget") == 0)
	{
		newCmd = new fxScnSetDepthSurf(this, cmds);
		m_pScn->commands.push_back(newCmd);
	}

	return TRUE;
}

u32 IgfxEffect::_SceneParseBuff(const s8 *buff)
{
	//don't even bother, if we don't have m_pScn
	if(!m_pScn)
		return TRUE;

	//temporarily create the buffer for the sake of getting the scene stuff
	tCHAR *wBuff = (tCHAR *)MemAlloc(sizeof(tCHAR)*(strlen((const char*)buff)+1));
	mbstowcs(wBuff, (const char *)buff, strlen((const char*)buff)+1);

	tCHAR *pBuff = wBuff;
	tCHAR cBuff[MAXCHARBUFF];

	while(pBuff[0] != 0)
	{
		if(ParserReadStringBuff(&pBuff, cBuff, MAXCHARBUFF, '<', '>') == RETCODE_ENDREACHED)
		{
			if(_SceneParseCommand(cBuff) == FALSE)
				break; //end of scene commands
		}
	}

	MemFree(wBuff);

	return TRUE;
}

u32 IgfxEffect::_SceneParseFile(const tCHAR *filepath)
{
	//don't even bother, if we don't have m_pScn
	if(!m_pScn)
		return TRUE;

	hFILE fp = FSOpen(filepath, L"rt");
	tCHAR cBuff[MAXCHARBUFF];

	if(fp)
	{
		while(!FSEOF(fp))
		{
			if(ParserReadStringFile(fp, cBuff, MAXCHARBUFF, '<', '>') == RETCODE_ENDREACHED)
			{
				if(_SceneParseCommand(cBuff) == FALSE)
					break; //end of scene commands
			}
		}

		FSClose(fp);
	}

	return TRUE;
}

/////////////////////////////////////
// Purpose:	render the scene with
//			special FX
// Output:	cool scene rendered
// Return:	TRUE if success
/////////////////////////////////////
u32 IgfxEffect::SceneRender()
{
	if(m_pScn)
	{
		u32 i;

		if(TESTFLAGS(g_FLAGS, GFX_BEGINSCENESET))
			g_p3DDevice->EndScene(); //in case outside, the user already began gfx scene

		//get the primary render target and depth surface
		g_p3DDevice->GetRenderTarget(0, &m_pScn->pRTSurf);
		g_p3DDevice->GetDepthStencilSurface(&m_pScn->pDepthSurf);

		//set to nothing first
		m_pScn->renderTxtInd = -1;
		m_pScn->pass = 0;

		_GFXSceneSort();

		SetGlobalParams();

		//set the render texture targets
		for(i = 0; i < m_pScn->renderTxts.size(); i++)
		{
			char theName[MAXCHARBUFF]; wcstombs(theName, m_pScn->renderTxts[i].name.c_str(), MAXCHARBUFF);
			LPDIRECT3DBASETEXTURE9 pTex = m_pScn->renderTxts[i].txt;
			m_effect->SetTexture(theName, pTex);
			//SAFE_RELEASE(pTex);
		}

		//Execute commands
		for(i = 0; i < m_pScn->commands.size(); i++)
			m_pScn->commands[i]->Execute(this);

		//set the primary render target and depth surface to device
		//and release them!
		if(m_pScn->pRTSurf)
		{
			g_p3DDevice->SetRenderTarget(0, m_pScn->pRTSurf);
			m_pScn->pRTSurf->Release();
		}

		if(m_pScn->pDepthSurf)
		{
			g_p3DDevice->SetDepthStencilSurface(m_pScn->pDepthSurf);
			m_pScn->pDepthSurf->Release();
		}
		
		if(TESTFLAGS(g_FLAGS, GFX_BEGINSCENESET))
			g_p3DDevice->BeginScene(); //as stated from above
	}

	return TRUE;
}