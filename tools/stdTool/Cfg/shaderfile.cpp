#include "cfg_i.h"

class IShaderF : public ShaderFile {
public:

IShaderF(const tCHAR *filename) : m_ref(0)
{
	hFILE fp = FSOpen(filename, L"rt");

	if(fp)
	{
		tCHAR retC;
		tCHAR name[MAXCHARBUFF], *chunk, *chunkPtr;
		tCHAR param[DMAXCHARBUFF], *paramPtr;

		s32 chunkSize=0, paramStrSize;

		while(!FSEOF(fp))
		{
			//read a word
			retC = ParserReadWordFile(fp, name, MAXCHARBUFF, 0);

			//check if it is a comment
			if(name[0] == '/' && name[1] == '/')
			{
				if(retC != '\n') //skip the entire line if not yet done
					ParserSkipCharFile(fp, '\n', FALSE);

				continue;
			}

			//let's get the chunk
			if(ParserCountStringFile(fp, &chunkSize, '{', '}') == RETCODE_ENDREACHED)
			{
				//allocate the chunk
				chunk = (tCHAR *)MemAlloc(sizeof(tCHAR)*(chunkSize+1));

				if(chunk)
				{
					//get the chunk from file
					ParserReadStringFile(fp, chunk, chunkSize+1, '{', '}');

					//create the new section
					ShaderSec newSec;
					newSec.name = name;

					//now parse the chunk
					chunkPtr = chunk;

					while(*chunkPtr != 0)
					{
						//get the variable name
						retC = ParserReadWordBuff(&chunkPtr, name, MAXCHARBUFF, 0);

						//check if it is a comment
						if(name[0] == '/' && name[1] == '/')
						{
							if(retC != '\n') //skip the entire line if not yet done
								ParserSkipCharBuff(&chunkPtr, '\n', FALSE);

							continue;
						}

						//bother with finding the param?
						if(retC != '\n')
						{
							//skip out some spaces and TABS
							while(*chunkPtr != 0)
							{ if(*chunkPtr == ' ' || *chunkPtr == 9) chunkPtr++; else break; }

							//get the param
							ParserReadStringBuff(&chunkPtr, param, DMAXCHARBUFF, 0, '\n');

							//zero out anything in param that is a comment
							paramPtr = param;
							while(*paramPtr != 0)
							{ if(*paramPtr == '/' && *(paramPtr+1) == '/') { *paramPtr = 0; break; } paramPtr++; }

							//remove spaces and TABS at the end
							paramStrSize = wcslen(param)-1;
							while(paramStrSize >= 0) 
							{ 
								if(param[paramStrSize] == ' ' || param[paramStrSize] == 9) 
									param[paramStrSize] = 0;
								else break;
								paramStrSize--;
							}
						}
						else
							param[0] = 0;

						//add to a shaderitem
						//we should at least have a variable name
						if(name[0] != 0)
						{
							ShaderVar newVar;
							newVar.var = name;
							newVar.param = param;

							newSec.vars.push_back(newVar);
						}
					}

					//free the chunk
					MemFree(chunk);

					//add the section
					m_sec.push_back(newSec);
				}
			}
			else
				break;
		}

		FSClose(fp);
	}
	else
		LogMsg(LOG_FILE, L"ShaderFileLoad: Unable to load", filename);
}

~IShaderF()
{
}

void AddRef()
{
	m_ref++;
}

s32 Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Purpose:	get number of sections in
//			the shader file
// Output:	none
// Return:	number of shader sections
/////////////////////////////////////
u32 SectionGetNum()
{
	return m_sec.size();
}

/////////////////////////////////////
// Purpose:	get the name of the given
//			section index
// Output:	none
// Return:	the name
/////////////////////////////////////
const tCHAR *SectionGetName(u32 ind)
{
	return m_sec[ind].name.c_str();
}

/////////////////////////////////////
// Purpose:	get number of variables (lines)
//			in given section index
// Output:	none
// Return:	number of variables (lines)
/////////////////////////////////////
u32 SectionGetVarNum(u32 ind)
{
	return m_sec[ind].vars.size();
}

/////////////////////////////////////
// Purpose:	get the name of the given
//			variable index
//			The left hand side
// Output:	none
// Return:	the name ptr.
/////////////////////////////////////
const tCHAR *VarGetName(u32 sectionInd, u32 ind)
{
	return m_sec[sectionInd].vars[ind].var.c_str();
}

/////////////////////////////////////
// Purpose:	get the param of the given
//			variable index
//			The right hand side
// Output:	none
// Return:	the param ptr.
/////////////////////////////////////
const tCHAR *VarGetParam(u32 sectionInd, u32 ind)
{
	return m_sec[sectionInd].vars[ind].param.c_str();
}

private:
	s32					m_ref;
	vector<ShaderSec>	m_sec;
};

/////////////////////////////////////
// Name:	ShaderFileLoad
// Purpose:	loads a given shader file
// Output:	shader file ready to be parsed
// Return:	something new
/////////////////////////////////////
hSHADERF ShaderFileLoad(const tCHAR *filename)
{
	hSHADERF newShader = (hSHADERF)new IShaderF(filename);

	if(!newShader)
		LogMsg(LOG_FILE, L"ShaderFileLoad: Unable to allocate new shader file", filename);

	return newShader;
}
