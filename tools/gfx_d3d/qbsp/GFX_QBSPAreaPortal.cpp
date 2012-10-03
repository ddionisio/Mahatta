#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

/*
===============================================================================

AREAPORTALS

===============================================================================
*/

void IgfxQBSP::AddAreaPortal(s32 portalnum, s32 area, s32 otherarea )
{
	s32 i;
	carea_t *a;
	careaportal_t *ap;

	if ( portalnum >= MAX_CM_PORTALS ) {
		return;
	}
	if ( !area || area > m_numareas || !otherarea || otherarea > m_numareas ) {
		return;
	}

	ap = m_map_areaportals;
	for ( i = 0; i < m_numareaportals; i++, ap++ ) {
		if ( ap->portalnum == portalnum ) {
			return;		// already exists
		}
	}

	if ( m_numareaportals+2 > MAX_CM_AREAPORTALS ) {
		return;
	}

	ap = &m_map_areaportals[m_numareaportals];
	ap->portalnum = portalnum;
	ap->otherarea = otherarea;

	a = &m_map_areas[area];
	a->areaportals[a->numareaportals++] = m_numareaportals++;

	ap = &m_map_areaportals[m_numareaportals];
	ap->portalnum = portalnum;
	ap->otherarea = area;

	a = &m_map_areas[otherarea];
	a->areaportals[a->numareaportals++] = m_numareaportals++;
}

void IgfxQBSP::_FloodArea_r(carea_t *area, int floodnum)
{
	int		i;
	careaportal_t	*p;

	if (area->floodvalid == m_floodvalid)
	{
		if (area->floodnum == floodnum)
			return;
		//Com_Error (ERR_DROP, "FloodArea_r: reflooded");
	}

	area->floodnum = floodnum;
	area->floodvalid = m_floodvalid;
	for (i=0 ; i<area->numareaportals ; i++)
	{
		p = &m_map_areaportals[area->areaportals[i]];
		if (m_portalopen[p->portalnum])
			_FloodArea_r(&m_map_areas[p->otherarea], floodnum);
	}
}

/*
====================
FloodAreaConnections


====================
*/
void IgfxQBSP::FloodAreaConnections()
{
	int		i;
	carea_t	*area;
	int		floodnum;

	// all current floods are now invalid
	m_floodvalid++;
	floodnum = 0;

	// area 0 is not used
	for (i=1 ; i<m_numareas ; i++)
	{
		area = &m_map_areas[i];
		if (area->floodvalid == m_floodvalid)
			continue;		// already flooded into
		floodnum++;
		_FloodArea_r(area, floodnum);
	}

}

void IgfxQBSP::SetAreaPortalState(s32 portalnum, u8 open)
{
	if (portalnum > MAX_CM_PORTALS)
		LogMsg(LOG_FILE, L"QBSPSetAreaPortalState", L"areaportal > numareaportals");

	m_portalopen[portalnum] = open;
	FloodAreaConnections();
}

u8 IgfxQBSP::AreasConnected(s32 area1, s32 area2)
{
	//if (cm_noAreas->value) //debug purpose we don't really need
	//	return qtrue;

	if (area1 > m_numareas || area2 > m_numareas)
		LogMsg(LOG_FILE, L"QBSPAreasConnected", L"area > numareas");

	if (m_map_areas[area1].floodnum == m_map_areas[area2].floodnum)
		return TRUE;
	return FALSE;
}


/*
=================
CM_WriteAreaBits

Writes a length byte followed by a bit vector of all the areas
that area in the same flood as the area parameter

This is used by the client refreshes to cull visibility
=================
*/
s32 IgfxQBSP::WriteAreaBits(u8 *buffer, s32 area)
{
	s32		i;
	s32		bytes;

	bytes = (m_numareas+7)>>3;

	/*if (cm_noAreas->value)
	{	// for debugging, send everything
		memset (buffer, 255, bytes);
	}
	else*/
	{
		memset (buffer, 0, bytes);

		for (i=1 ; i<m_numareas ; i++)
		{
			if (!area || AreasConnected(i, area) || i == area)
				buffer[i>>3] |= 1<<(i&7);
		}
	}

	return bytes;
}

/*
=================
CM_MergeAreaBits
=================
*/
void IgfxQBSP::MergeAreaBits(u8 *buffer, s32 area)
{
	s32		i;

	for (i=1 ; i<m_numareas ; i++)
	{
		if (AreasConnected(i, area ) || i == area)
			buffer[i>>3] |= 1<<(i&7);
	}
}

/*
===================
CM_WritePortalState

Writes the portal state to a savegame file
===================
*/
void IgfxQBSP::WritePortalState(hFILE f)
{
	s32 i, j;

	FSWrite (m_portalopen, sizeof(m_portalopen), 1, f );

	FSWrite ( &m_numareaportals, sizeof(s32), 1, f );
	FSWrite ( m_map_areaportals, sizeof(m_map_areaportals[0])*m_numareaportals, 1, f );

	FSWrite ( &m_numareas, sizeof(s32), 1, f );

	for ( i = 1; i < m_numareas; i++ ) {
		FSWrite ( &m_map_areas[i].numareaportals, sizeof(s32), 1, f );

		for ( j = 0; j < m_map_areas[i].numareaportals; j++ ) {
			FSWrite ( &m_map_areas[i].areaportals[j], sizeof(s32), 1, f );
		}
	}
}

/*
===================
CM_ReadPortalState

Reads the portal state from a savegame file
and recalculates the area connections
===================
*/
void IgfxQBSP::ReadPortalState(hFILE f)
{
	s32 i, j;

	FSRead ( m_portalopen, sizeof(m_portalopen), 1, f );

	FSRead ( &m_numareaportals, sizeof(s32), 1, f );
	FSRead ( m_map_areaportals, sizeof(m_map_areaportals[0])*m_numareaportals, 1, f );

	FSRead ( &m_numareas, sizeof(s32), 1, f );

	for ( i = 1; i < m_numareas; i++ ) {
		FSRead ( &m_map_areas[i].numareaportals, sizeof(s32), 1, f );

		for ( j = 0; j < m_map_areas[i].numareaportals; j++ ) {
			FSRead ( &m_map_areas[i].areaportals[j], sizeof(s32), 1, f );
		}
	}

	FloodAreaConnections();
}