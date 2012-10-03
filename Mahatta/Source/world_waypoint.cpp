#include "engine_local.h"

#include "world.h"

#include "waypoint.h"

//waypoints
//for easy access from levels

map<wstring, wayPt > g_wps;
typedef map<wstring, wayPt >::iterator wpIt;

/////////////////////////////////////
// Name:	WorldWPExist
// Purpose:	check to see if given
//			WP name exists
// Output:	none
// Return:	TRUE if exists
/////////////////////////////////////
u8 WorldWPExist(const tCHAR *name)
{
	if(name[0] == 0)
		return FALSE;

	wpIt it = g_wps.find(name);

	return (it != g_wps.end());
}

/////////////////////////////////////
// Name:	WorldWPSet
// Purpose:	set a waypoint
// Output:	new waypt added if not exist
// Return:	none
/////////////////////////////////////
void WorldWPSet(const tCHAR *name, const wayPt & wp)
{
	g_wps[name] = wp;
}

/////////////////////////////////////
// Name:	WorldWPGet
// Purpose:	get wp data from given
//			index
// Output:	wpOut set
// Return:	TRUE if wp is found
/////////////////////////////////////
u8 WorldWPGet(const tCHAR *name, wayPt * wpOut)
{
	if(WorldWPExist(name))
	{
		*wpOut = g_wps[name];
		return TRUE;
	}

	return FALSE;
}

//Engine exclusive functions:

/////////////////////////////////////
// Name:	WorldWPAddEnt
// Purpose:	add a waypoint based on given
//			entity parse
// Output:	new target added
// Return:	none
/////////////////////////////////////
void WorldWPAddEnt(const EntityParse & entityDat)
{
	const tCHAR *ID;
	wayPt wp;

	//get ID
	ID = entityDat.GetVal(L"targetname");

	//get target
	if(entityDat.GetVal(L"target"))
		wp.target = entityDat.GetVal(L"target");
	else
		wp.target = L"\0";

	//get wait
	if(entityDat.GetVal(L"wait"))
		swscanf(entityDat.GetVal(L"wait"), L"%f", &wp.wait);
	else
		wp.wait = 0;

	//get delay
	if(entityDat.GetVal(L"delay"))
		swscanf(entityDat.GetVal(L"delay"), L"%f", &wp.delay);
	else
		wp.delay = -1;

	//get location
	if(entityDat.GetVal(L"origin"))
	{
		swscanf(entityDat.GetVal(L"origin"), L"%f %f %f", &wp.pt.x, &wp.pt.z, &wp.pt.y);

		wp.pt.z *= -1;
	}

	//get angle
	if(entityDat.GetVal(L"angle"))
	{
		swscanf(entityDat.GetVal(L"angle"), L"%f", &wp.angle);
		wp.angle = TO_RAD(wp.angle);
	}
	else
		wp.angle = -1;

	//add the waypoint
	if(ID && ID[0] != 0)
		WorldWPSet(ID, wp);
}

/////////////////////////////////////
// Name:	WorldWPClear
// Purpose:	clear out all waypoints
// Output:	all wps removed
// Return:	none
/////////////////////////////////////
void WorldWPClear()
{
	g_wps.clear();
}

//
// WP Node
//

class IWayPtNode : public WayPtNode	{
public:
	IWayPtNode(const tCHAR *wpName) : m_ref(0), m_curTrav(0), m_curWait(0), m_t(0)
	{
		//previous is set to not exist first
		m_bWp[0] = FALSE;

		m_bWp[1] = WorldWPExist(wpName);

		if(m_bWp[1])
		{
			WorldWPGet(wpName, &m_wp[1]);

			//get the next wp
			m_bWp[2] = WorldWPExist(m_wp[1].target.c_str());

			if(m_bWp[2])
				WorldWPGet(m_wp[1].target.c_str(), &m_wp[2]);

			
		}
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
// Purpose:	get the current waypoint
//			location
// Output:	pLoc set
// Return:	FALSE if ptOut is null,
//			wpNode ind invalid or
//			wp for wpNode doesn't exist
/////////////////////////////////////
u8 GetLoc(Vec3D *ptOut)
{
	if(m_bWp[1] && ptOut)
	{
		*ptOut = m_wp[1].pt;
		return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////
// Purpose:	get the current angle
//			(radians)
// Output:	none
// Return:	the angle
/////////////////////////////////////
f32 GetAngle()
{
	return m_wp[1].angle;
}

/////////////////////////////////////
// Purpose:	get the current wait
//			delay
// Output:	none
// Return:	the wait delay (millisec)
/////////////////////////////////////
f32 GetWait()
{
	return m_wp[1].wait;
}

/////////////////////////////////////
// Purpose:	get the current direction
// Output:	vOut set
// Return:	FALSE if vOut is null,
//			wpNode ind invalid or
//			wp for wpNode doesn't exist
//			and if angle doesn't exist
//			for current wp
/////////////////////////////////////
u8 GetDir(Vec3D *vOut)
{
	if(m_bWp[1] && vOut)
	{
		//let's get the dir based on angle
		Matrix mtx;
		Vec3D edir(1,0,0);
		g_GFX->MtxRotateY(m_wp[1].angle, &mtx);
		g_GFX->Vec3DTransformNormal(&edir, &mtx, vOut);

		return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////
// Purpose:	get the current linear
//			location of node
// Output:	ptOut set
// Return:	FALSE if ptOut is null,
//			wpNode ind invalid or
//			wp for wpNode doesn't exist
/////////////////////////////////////
u8 GetCurLinearLoc(Vec3D *ptOut)
{
	if(ptOut && m_bWp[1] && m_bWp[2])
	{
		Vec3DLerp(&m_wp[1].pt, &m_wp[2].pt, m_t, ptOut);
		return TRUE;
	}

	return GetLoc(ptOut);
}

/////////////////////////////////////
// Purpose:	get the current linear 
//			interpolated angle
//			(radians)
// Output:	vOut set
// Return:	the angle
/////////////////////////////////////
f32 GetCurLinearAngle()
{
	if(m_bWp[1] && m_bWp[2])
		return m_wp[1].angle + m_t*(m_wp[2].angle - m_wp[1].angle);

	return m_wp[1].angle;
}

/////////////////////////////////////
// Purpose:	get the current linear
//			interpolated dir of
//			the node (based on t)
// Output:	vOut
// Return:	FALSE if vOut is null,
//			wpNode ind invalid or
//			wp for wpNode doesn't exist
//			and if angle doesn't exist
//			for current wp
/////////////////////////////////////
u8 GetCurLinearDir(Vec3D *vOut)
{
	if(vOut && m_bWp[1] && m_bWp[2])
	{
		f32 fA = m_wp[1].angle + m_t*(m_wp[2].angle - m_wp[1].angle);

		//let's get the dir based on angle
		Matrix mtx;
		Vec3D edir(1,0,0);
		g_GFX->MtxRotateY(fA, &mtx);
		g_GFX->Vec3DTransformNormal(&edir, &mtx, vOut);

		return TRUE;
	}

	return GetDir(vOut);
}

/////////////////////////////////////
// Purpose:	get the current curve
//			location of node
// Output:	ptOut set, targetOut is
//			optional
// Return:	FALSE if ptOut is null,
//			wpNode ind invalid or
//			wp for wpNode doesn't exist
/////////////////////////////////////
u8 GetCurCurvedLoc(Vec3D *ptOut)
{
	if(ptOut && m_bWp[1] && m_bWp[2])
	{
		Vec3D prevPt, next2Pt;

		if(WorldWPExist(m_wp[2].target.c_str()))
		{
			wayPt next2wp;
			WorldWPGet(m_wp[2].target.c_str(), &next2wp);
			next2Pt = next2wp.pt;
		}
		else
			next2Pt = m_wp[2].pt;

		if(m_bWp[0])
			prevPt = m_wp[0].pt;
		else
			prevPt = m_wp[1].pt;

		g_GFX->Vec3DCatMullRom(&prevPt, &m_wp[1].pt, &m_wp[2].pt, &next2Pt, 
			m_t, ptOut);

		return TRUE;
	}

	return GetLoc(ptOut);
}

/////////////////////////////////////
// Purpose:	set the next wp immediately
// Output:	wp next set
// Return:	none
/////////////////////////////////////
void SetNext()
{
	m_bWp[2] = WorldWPExist(m_wp[2].target.c_str());

	if(m_bWp[2])
		WorldWPGet(m_wp[2].target.c_str(), &m_wp[2]);

	m_t = 0;
	m_curTrav = 0;
	m_curWait = 0;
}

/////////////////////////////////////
// Purpose:	jump to next node (similar
//			to WPNodeMoveToNext, except
//			we don't update t)
// Output:	wpNode set to next
// Return:	TRUE if we go to next
/////////////////////////////////////
u8 GotoNext()
{
	m_curWait += EngineGetTick();

	if(m_curWait >= m_wp[1].wait)
	{
		//set prev
		m_bWp[0] = m_bWp[1];
		if(m_bWp[0]) m_wp[0] = m_wp[1];

		//set current if there is a next
		if(m_bWp[2])
		{
			m_bWp[1] = m_bWp[2];
			m_wp[1] = m_wp[2];
		}

		SetNext(); //this will also reset timers and t

		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////
// Purpose:	move current pt between
//			wp and wp next.
// Output:	current pt set
// Return:	TRUE if we reached end
/////////////////////////////////////
u8 MoveToNext(f32 delay)
{
	u8 ret = FALSE;

	if(m_curWait >= m_wp[1].wait)
	{
		//moving...
		m_curTrav += EngineGetTick();

		//get the time interval between the waypoint path
		if(m_wp[1].delay >= 0)
			m_t = m_curTrav/m_wp[1].delay;
		else
			m_t = m_curTrav/delay;

		//that means it's time to move on
		if(m_t >= 1)
		{
			//set prev
			m_bWp[0] = m_bWp[1];
			if(m_bWp[0]) m_wp[0] = m_wp[1];

			//set current if there is a next
			if(m_bWp[2])
			{
				m_bWp[1] = m_bWp[2];
				m_wp[1] = m_wp[2];
			}

			SetNext(); //this will also reset timers and t
		}
	}
	else //still waiting...
		m_curWait += EngineGetTick();

	return !m_bWp[2];
}

/////////////////////////////////////
// Purpose:	check to see if we are
//			done with moving
//			(only applies to
//			WAYPT_STOPATEND)
// Output:	none
// Return:	TRUE if we are done
/////////////////////////////////////
u8 IsDone()
{
	return !m_bWp[2];
}

private:
	wayPt		m_wp[3];				//0=prev 1=cur 2=next
	u8			m_bWp[3];				//each exist?
	
	f32			m_curWait;				//current wait counter for each node
	f32			m_curTrav;				//current travel counter for going from one pt to another

	f32			m_t;					//current time interval between current waypt and next waypt (0-1)

	s32			m_ref;
};

/////////////////////////////////////
// Name:	WPNodeCreate
// Purpose:	create a waypoint tracker
//			interface (node)
// Output:	new waypoint node created
// Return:	the new waypoint node
/////////////////////////////////////
hWPNODE WPNodeCreate(const tCHAR *wpName)
{
	return (hWPNODE)new IWayPtNode(wpName);
}