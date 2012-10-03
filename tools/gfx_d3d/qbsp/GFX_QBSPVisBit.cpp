#include "..//gfx_i.h"

#include "gfx_qbsp.h"
#include "gfx_qbsp_i.h"

/*void QBSPVisBitClearAll(u32 *vis, s32 size)
{
	memset(vis, 0, sizeof(u32) * size);
}*/

u32 * QBSPVisBitCreate(u32 count, u32 & sOut)
{
	sOut = count/32 + 1;

	u32 *newVis = (u32 *)MemAlloc(sizeof(u32)*sOut);

	if(!newVis)
	{ LogMsg(LOG_FILE, L"QBSPVisBitCreate", L"Cannot allocate vis bits"); return 0; }

	QBSPVisBitClearAll(newVis, sOut);

	return newVis;
}