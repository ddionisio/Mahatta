#include "gfx_i.h"

IgfxSprite::IgfxSprite(hTXT txt, u32 numFrame, const iPoint *frameSize, f32 delay, u32 numImgCol, 
		const iPoint *imgOffset, u32 numState, const gfxSprFrame *states)
: m_ref(0), m_theTexture(txt), m_numFrames(numFrame), m_numStates(numState), m_time(0), m_timeDelay(delay),
m_curState(0), m_status(0)
{
	m_theTexture->AddRef();

	//assign stuff
	m_frameSize = *frameSize;

	if(numImgCol > 0)
		m_numImgCol = numImgCol;
	else
		m_numImgCol = m_theTexture->GetWidth()/m_frameSize.x;
	
	if(imgOffset)
		m_imgOffset = *imgOffset;
	else
	{ m_imgOffset.x = m_imgOffset.y = 0; }

	/////////////////////////////////////////////////
	//create the frame rects
	m_frameRects = (iRect*)MemAlloc(sizeof(iRect)*m_numFrames);
	if(!m_frameRects)
	{ LogMsg(LOG_FILE, L"IgfxSprite", L"Unable to allocate frame rects"); }

	//pre-CALIculate the rects
	for(u32 i = 0; i < m_numFrames; i++)
	{
		m_frameRects[i].left = i % m_numImgCol * m_frameSize.x + m_imgOffset.x;
		m_frameRects[i].top = i / m_numImgCol * m_frameSize.y + m_imgOffset.y; //offset
		m_frameRects[i].right = m_frameRects[i].left + m_frameSize.x; //plus the size of frame
		m_frameRects[i].bottom = m_frameRects[i].top + m_frameSize.y; //plus size of frame
	}
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	//create states
	m_states = (gfxSprFrame*)MemAlloc(sizeof(gfxSprFrame)*m_numStates);
	if(!m_states)
	{ LogMsg(LOG_FILE, L"SpriteCreate", L"Unable to allocate states"); }

	//just copy it
	memcpy(m_states, states, sizeof(gfxSprFrame)*m_numStates);
	/////////////////////////////////////////////////
}

IgfxSprite::~IgfxSprite()
{
	if(m_frameRects)
		MemFree(m_frameRects);
	if(m_states)
		MemFree(m_states);

	if(m_theTexture)
		m_theTexture->Release();
}

void IgfxSprite::AddRef()
{
	m_ref++;
}

s32 IgfxSprite::Release()
{
	if(m_ref == 0)
		delete this;
	else
	{ m_ref--; return m_ref; }

	return 0;
}

/////////////////////////////////////
// Name:	SpriteCreate
// Purpose:	creates a sprite with txt.
//			NOTE: states has to be allocated with
//				  size numState.
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
hSPRITE F_API SpriteCreate(hTXT txt, u32 numFrame, const iPoint *frameSize, 
							f32 delay, u32 numImgCol, const iPoint *imgOffset,
							u32 numState, const gfxSprFrame *states)
{
	assert(txt);

	hSPRITE newSpr = (hSPRITE)new IgfxSprite(txt, numFrame, frameSize, delay, numImgCol, imgOffset, numState, states);

	if(!newSpr)
	{ LogMsg(LOG_FILE, L"SpriteCreate", L"Unable to allocate new sprite"); return 0; }

	return newSpr;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
hSPRITE F_API SpriteLoad(hTXT txt, u32 numImageCol, 
							   const iPoint *imgOffset, const tCHAR *filename)
{
	iPoint frameSize;
	u32 delay, numFrames, numStates;
	gfxSprFrame *states;

	//let's load up some stuff
	hFILE theFile = FSOpen(filename, L"rt");

	if(!theFile)
	{ LogMsg(LOG_FILE, L"SpriteLoad", L"Unable to open sprite file"); return 0; }

	tCHAR buff[MAXCHARBUFF]={0};

	//get the frame size
	ParserReadStringFile(theFile, buff, MAXCHARBUFF, 0, '\n');
	swscanf(buff, L"framesize x=%d y=%d", &frameSize.x, &frameSize.y);

	//get the animation delay
	ParserReadStringFile(theFile, buff, MAXCHARBUFF, 0, '\n');
	swscanf(buff, L"aniDelay=%d", &delay);

	//get number of frames
	ParserReadStringFile(theFile, buff, MAXCHARBUFF, 0, '\n');
	swscanf(buff, L"numFrames=%d", &numFrames);

	//get the number of states
	ParserReadStringFile(theFile, buff, MAXCHARBUFF, 0, '\n');
	swscanf(buff, L"numStates=%d", &numStates);

	//now let's allocate the states
	states = (gfxSprFrame*)MemAlloc(sizeof(gfxSprFrame)*numStates);
	if(!states)
	{ LogMsg(LOG_FILE, L"SpriteLoad", L"Unable to allocate states"); FSClose(theFile); return 0; }

	//now fill up the states
	for(u32 i = 0; i < numStates; i++)
	{
		ParserReadStringFile(theFile, buff, MAXCHARBUFF, 0, '\n');
		swscanf(buff, L"State first=%d last=%d", &states[i].firstframe, &states[i].lastframe);
	}

	//create the sprite!
	hSPRITE newSprite = SpriteCreate(txt, numFrames, &frameSize, (f32)delay, numImageCol, 
		imgOffset, numStates, states);

	//we don't need states anymore
	MemFree(states);

	//we don't need the file
	FSClose(theFile);

	if(!newSprite)
		LogMsg(LOG_FILE, L"SpriteLoad", L"Unable to create sprites");

	return newSprite;
}

/////////////////////////////////////
// Purpose:	checks to see if the state
//			of a sprite reached the end
//			frame of current state
// Output:	none
// Return:	true if curframe reached end
/////////////////////////////////////
u8 IgfxSprite::IsStateEnd()
{
	if(m_states[m_curState].curFrame == m_states[m_curState].lastframe)
		return TRUE;
	return FALSE;
}

/////////////////////////////////////
// Purpose:	pause/unpause sprite
// Output:	spr state set
// Return:	none
/////////////////////////////////////
void IgfxSprite::Pause(u8 bPause)
{
	if(bPause)
		SETFLAG(m_status, SPR_PAUSE);
	else
		CLEARFLAG(m_status, SPR_PAUSE);
}

/////////////////////////////////////
// Purpose:	pause animation on end
// Output:	sprite state set
// Return:	
/////////////////////////////////////
void IgfxSprite::PauseOnStateEnd(u8 bPause)
{
	if(bPause)
		SETFLAG(m_status, SPR_PAUSE_ON_END);
	else
		CLEARFLAG(m_status, SPR_PAUSE_ON_END);
}

/////////////////////////////////////
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void IgfxSprite::UpdateFrame()
{
	//do the tick count
	m_time += g_time;

	if(!TESTFLAGS(m_status, SPR_PAUSE) && m_time >= m_timeDelay)
	{
		m_time = 0;

		u32 & theFrame = m_states[m_curState].curFrame;
		theFrame++;

		if(theFrame > m_states[m_curState].lastframe)
		{
			if(TESTFLAGS(m_status, SPR_PAUSE_ON_END))
			{ Pause(TRUE); theFrame = m_states[m_curState].lastframe; }
			else
				theFrame = m_states[m_curState].firstframe;
		}
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void IgfxSprite::UpdateFrameAllState()
{
	//do the tick count
	m_time += g_time;

	if(!TESTFLAGS(m_status, SPR_PAUSE) && m_time >= m_timeDelay)
	{
		m_time = 0;

		for(u32 i = 0; i < m_numStates; i++)
		{
			u32 & theFrame = m_states[i].curFrame;

			if(m_states[i].firstframe <= m_states[i].lastframe)
			{
				theFrame++;

				if(theFrame > m_states[i].lastframe)
					theFrame = m_states[i].firstframe;
			}
			else if(m_states[i].firstframe > m_states[i].lastframe)
			{
				if(theFrame > 0)
				{
					theFrame--;

					if(theFrame < m_states[i].lastframe)
						theFrame = m_states[i].firstframe;
				}
				else
					theFrame = m_states[i].firstframe;
			}
		}
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u32 IgfxSprite::GetMaxState()
{
	return m_numStates;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u32 IgfxSprite::GetCurState()
{
	return m_curState;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u32 IgfxSprite::SetState(u32 state)
{
	if(state >= GetMaxState())
		return FALSE;

	m_curState = state;

	m_states[m_curState].curFrame = m_states[m_curState].firstframe;

	return TRUE;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
const iPoint & IgfxSprite::GetSize()
{
	return m_frameSize;
}

/////////////////////////////////////
// Name:	SpriteBlt
// Purpose:	displays sprite
//			NOTE: GFXBltModeEnable()
//				  must be called first
// Output:	sprite is displayed
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
u32 IgfxSprite::Blt(f32 x, f32 y, gfxBlt *pBlt)
{
	return m_theTexture->Blt(x, y, &m_frameRects[m_states[m_curState].curFrame], pBlt);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
u32 IgfxSprite::StretchBlt(f32 x, f32 y, f32 w, f32 h, gfxBlt *pBlt)
{
	return m_theTexture->StretchBlt(x, y, w, h, &m_frameRects[m_states[m_curState].curFrame], pBlt);
}

/////////////////////////////////////
// Name:	SpriteDuplicate
// Purpose:	duplicate a sprite with
//			given sprite
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
hSPRITE IgfxSprite::Duplicate()
{
	hSPRITE newSpr;

	newSpr = SpriteCreate(m_theTexture, m_numFrames, &m_frameSize,
		m_timeDelay, m_numImgCol, &m_imgOffset,
		m_numStates, m_states);

	return newSpr;
}
