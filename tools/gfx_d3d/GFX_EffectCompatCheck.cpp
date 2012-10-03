#include "gfx_i.h"
//
#include "gfx_effect.h"

//returns TRUE if given tech is compatible with the card,
//o.w. FALSE, you should look at the next technique
u8 IgfxEffect::_CheckCompatibility(GFXHANDLE tech)
{
	//validate this technique first
	if(FAILED(m_effect->ValidateTechnique(tech)))
		return FALSE;

	u8 ret = TRUE;

	//even if it is valid, some graphics card do not necessarily behave correctly...
	//so we check for d3dcaps type compatibilities...
	D3DXTECHNIQUE_DESC techD;

	m_effect->GetTechniqueDesc(tech, &techD);

	GFXHANDLE annotH;
	D3DXPARAMETER_DESC annotD;

	LPCSTR pStr;
	tCHAR buff[DMAXCHARBUFF], key[MAXCHARBUFF], val[MAXCHARBUFF], *pWStr;

	for(u32 annot = 0; annot < techD.Annotations; annot++)
	{
		annotH = m_effect->GetAnnotation(tech, annot);
		m_effect->GetParameterDesc(annotH, &annotD);

		if(stricmp(annotD.Name, "compat") == 0)
		{
			//get the string and parse it.
			m_effect->GetString(annotH, &pStr);
			mbstowcs(buff, pStr, DMAXCHARBUFF);

			pWStr = buff;
			ParserReadWordBuff(&pWStr, key, MAXCHARBUFF, '=');
			ParserReadWordBuff(&pWStr, val, MAXCHARBUFF, ';');

			//check val depending on key:
            if(wcsicmp(key, L"d3dcapsTexCoordCount") == 0)
			{
				u32 count = _wtoi(val);
				u32 nCount = LOWORD(g_d3dcaps.FVFCaps);
				
				if(nCount < count)
				{ ret = FALSE; break; }
			}
		}
	}

	return ret;
}