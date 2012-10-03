#ifndef _gfx_effect_scene_h
#define _gfx_effect_scene_h

class FXScnCmdParse {	//this is used for parsing FX scene commands
public:
	FXScnCmdParse() {}
	~FXScnCmdParse() {}

	//add variables from given buff
	void Parse(tCHAR *buff)
	{
		tCHAR lVal[MAXCHARBUFF]={0}, rVal[MAXCHARBUFF]={0};
		tCHAR **walker = &buff;

		while(**walker != 0)
		{
			lVal[0] = 0; rVal[0] = 0;

			//get the two string for lVal and rVal
			if(ParserReadWordBuff(walker, lVal, MAXCHARBUFF, '=') == '=')
				ParserReadStringBuff(walker, rVal, MAXCHARBUFF, '"', '"');
			
			AddVar(lVal, rVal);
		}
	}

	//add a variable with given value
	void AddVar(const tCHAR *lVal, const tCHAR *rVal)
	{
		if(GetVal(lVal))
			return;

		EntVar newVar;
		newVar.lVal = lVal;
		newVar.rVal = rVal;

		m_vars.push_back(newVar);
	}

	//get value of variable (0 if not found)
	const tCHAR *GetVal(const tCHAR *varName) const
	{
		for(u32 i = 0; i < m_vars.size(); i++)
		{
			if(_wcsicmp(m_vars[i].lVal.c_str(), varName) == 0)
				return m_vars[i].rVal.c_str();
		}

		return 0;
	}

private:
	struct EntVar {
		wstring lVal;	//variable
		wstring rVal;	//value
	};

	vector<EntVar> m_vars;	//the variables
};

class fxScnCommand {
public:
	fxScnCommand() {}
	virtual ~fxScnCommand() {}

	virtual s32 Execute(hIEFFECT fx) = 0;

private:
};

#endif