#include "engine_local.h"

#include "view.h"

#define VIEW_DELAY_DFLT		0.500f

class IView : public View {
public:
	IView(const Vec3D & pt, const Vec3D & target) : m_ref(0), m_ptNext(pt), m_targetNext(target), m_t(1.0f)
	{
		SetDelay(VIEW_DELAY_DFLT);

		g_GFX->CameraInit(&m_cam, pt, 0,0,0);
		g_GFX->CameraSetTarget(&m_cam, target);

		//set default public variables
		m_bStereo = FALSE;		//defaulted to OFF

		m_ipd = 2.5f;
		m_wp = 52;
		m_dp = 24;

		g_GFX->CameraInit(&m_sCam[0], pt, 0,0,0);
		g_GFX->CameraInit(&m_sCam[1], pt, 0,0,0);
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

	void Update()
	{
		if(!IsDone())
		{
			m_curTick += EngineGetTick();
			
			m_t = m_curTick/m_delay;

			if(m_t >= 1)
			{
				m_t = 1;

				m_curTick = 0;
			}

			Vec3DLerp(&m_cam.vEye, &m_ptNext, m_t, &m_cam.vEye);
			Vec3DLerp(&m_cam.vTarget, &m_targetNext, m_t, &m_cam.vTarget);

			//set stereo-scope
			if(m_bStereo && m_dp && m_wp)
			{
				f32 thetaV; //virtual view angle
				f32 thetaP;	//physical view angle
				f32 wV;		//virtual screen width
				f32 icd, icdH;	//inter-camera distance

				thetaV = thetaP = 2*atanf(m_wp/(2*m_dp));
				
				wV = 2*m_dv*tanf(thetaV*0.5f);

				icd = (m_ipd/m_wp)*wV;
				icdH = icd*0.5f;

				//set projection FOV to thetaV
				g_GFX->GFXSetProj(0,0, thetaV);

				//set the position of the two camera

				Vec3D w(m_cam.vEye - m_cam.vTarget); g_GFX->Vec3DNormal(&w, &w);
				Vec3D u; Vec3DCross(&m_cam.vUp, &w, &u); g_GFX->Vec3DNormal(&u, &u);

				m_sCam[0].vEye = m_cam.vEye - icdH*u;
				m_sCam[0].vTarget = m_cam.vTarget - icdH*u;

				m_sCam[1].vEye = m_cam.vEye + icdH*u;
				m_sCam[1].vTarget = m_cam.vTarget + icdH*u;
			}
		}
	}

	void SetDelay(f32 delay)
	{
		m_delay = delay;
		m_curTick = 0;
	}

	u8 IsDone()
	{
		return (m_t == 1.0f);
	}

	void SetNewPt(const Vec3D & pt)
	{
		m_cam.vEye = m_ptNext = pt;
	}

	void SetNewTgt(const Vec3D & tgt)
	{
		m_cam.vTarget = m_targetNext = tgt;
	}

	const Vec3D & GetPt()
	{
		return m_cam.vEye;
	}

	const Vec3D & GetTgt()
	{
		return m_cam.vTarget;
	}

	void SetPt(const Vec3D & pt)
	{
		if(m_ptNext != pt)
		{ 
			Vec3DLerp(&m_cam.vEye, &m_ptNext, m_t, &m_cam.vEye);

			m_curTick = 0;

			m_t = 0;

			m_ptNext = pt;
		}
	}

	void SetTgt(const Vec3D & tgt)
	{
		if(m_targetNext != tgt) 
		{ 
			Vec3DLerp(&m_cam.vTarget, &m_targetNext, m_t, &m_cam.vTarget);

			m_curTick = 0;

			m_t = 0;

			m_targetNext = tgt;
		}
	}

	void SetScene()
	{
		if(!m_bStereo)
			g_GFX->CameraSetScene(&m_cam);
		else
		{
			//stereo scope...
			g_GFX->CameraSetScene(&m_sCam[m_bFlip]);

			m_bFlip = !m_bFlip;
		}
	}

	void CollideMap(hQBSP qbsp)
	{
		Vec3D vMin(-5,-5,-5), vMax(5,5,5);
		gfxTrace trace;

		f32 t=1;
		
		qbsp->Collision(&vMin, &vMax, m_targetNext, m_ptNext, &trace, 1.0f);
		
		u32 txtFlags = qbsp->GetTextureContents(trace.txtInd);
		
		//hTXT txt = g_GFX->QBSPGetTexture(qbsp, trace.txtInd);
		//hEFFECT fx = g_GFX->QBSPGetEffect(qbsp, trace.txtInd);

		if(TESTFLAGS(txtFlags, CONTENTS_SOLID) || TESTFLAGS(txtFlags, CONTENTS_PLAYERCLIP))
			t = trace.t;

		//check for models
		u32 numMdl = qbsp->GetNumModels();
		for(u32 i = 1; i < numMdl; i++)
		{
			if(qbsp->ModelCollision(i, &vMin, &vMax, m_targetNext, m_ptNext, &trace, t))
			{
				//txt = g_GFX->QBSPGetTexture(qbsp, trace.txtInd);
				//fx = g_GFX->QBSPGetEffect(qbsp, trace.txtInd);
				txtFlags = qbsp->GetTextureContents(trace.txtInd);

				//if(txt || fx)
				if(TESTFLAGS(txtFlags, CONTENTS_SOLID) || TESTFLAGS(txtFlags, CONTENTS_PLAYERCLIP))
					t = trace.t;
			}
		}

		if(t < 1)
			Vec3DLerp(&m_targetNext, &m_ptNext, t, &m_ptNext);
	}

private:
	Vec3D		m_ptNext;		//next pt
	Vec3D		m_targetNext;	//next target

	f32			m_delay;
	f32			m_curTick;

	f32			m_t;

	gfxCamera	m_cam;

	//stereo-scope
	gfxCamera	m_sCam[2]; //0 = left, 1 = right
	u8			m_bFlip;	 //switching between left/right camera for passing in renderer

	s32			m_ref;
};

/////////////////////////////////////
// Name:	ViewCreate
// Purpose:	create a view interface
// Output:	new view created
// Return:	the new view
/////////////////////////////////////
hVIEW ViewCreate(const Vec3D & pt, const Vec3D & target)
{
	return (hVIEW)new IView(pt, target);
}