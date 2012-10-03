#include "engine_common.h"

#include "world.h"

#include "entity.h"

#define MAX_NUM_BRUTE_FORCE		10 //max number of objects for using brute-force

typedef enum {
	AXIS_INVALID,
	AXIS_X,
	AXIS_Y,
	AXIS_Z
} eAxis;

//This is the list of all entities used by the game
//This is later used for collision
//NOTE: MAKE SURE THIS ENTITY HAS THE EntityCommon INHERITED!!!!
struct EntityBoundDat {
	hIENTITY ent;
	f32	  val;
	bool  bOpen;
};

EntityBoundDat g_entityBounds[8192];	//big giant bound data
u32 g_entityBoundsSize=8192;

const Vec3D g_zeroVel(0,0,0);

//...it's pretty brutal, I say.
void _BrutalCollision(const EntList & group)
{
	const_EntIt iNext;

	hIENTITY pEntity1, pEntity2;

	for(const_EntIt i = group.begin(); i != group.end();)
	{
		iNext = i; iNext++;

		pEntity1 = *i;

		//don't bother entities with ignore collision flag
		if(pEntity1 && !TESTFLAGS(pEntity1->m_flag, ENTITY_FLAG_IGNORE_COLLIDE))
		{
			for(const_EntIt j = iNext; j != group.end(); j++)
			{
				pEntity2 = *j;

				if(pEntity2 && !TESTFLAGS(pEntity2->m_flag, ENTITY_FLAG_IGNORE_COLLIDE))
				{
					pEntity1->AddCollider(pEntity2);
					pEntity2->AddCollider(pEntity1);
				}
			}
		}

		i = iNext;
	}
}

#define BOUND_TOLER		1.0f

static s32 _ListOpenCloseBounds(EntityBoundDat *pBoundDatBuff, const EntList & group, eAxis axis)
{
	s32 outS=0;	//number of bounds added
	hIENTITY pEntity;

	//EntityBoundDat * pNewBoundDat = new EntityBoundDat[group.size()*2];
	//EntityBoundDat *pBoundDatWalker = pBoundDatBuff;

	for(const_EntIt i = group.begin(); i != group.end(); i++)
	{
		pEntity = *i;

		/*float ofs[eMaxPt] = {GAME_ABS(pEntity->GetVel().x),
			GAME_ABS(pEntity->GetVel().y),
			GAME_ABS(pEntity->GetVel().z)};


		if(pEntity->BoundGetMax() != pEntity->BoundGetMin()
			&& pEntity->BoundGetMax().x > ofs[eX]
			&& pEntity->BoundGetMax().y > ofs[eY]
			&& pEntity->BoundGetMax().z > ofs[eZ]
			&& !pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
		{
			ofs[eX] = 0;
			ofs[eY] = 0;
			ofs[eZ] = 0;
		}*/

		if(pEntity)
		{
			//don't add entities with ignore collision flag
			if(TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_IGNORE_COLLIDE))
				continue;

			pBoundDatBuff[outS].ent   = pEntity;
			pBoundDatBuff[outS].bOpen = true;

			switch(axis)
			{
			case AXIS_X:
				pBoundDatBuff[outS].val = pEntity->m_boundMin.x + pEntity->m_loc.x - BOUND_TOLER;

				if(pEntity->m_vel.x < 0)
					pBoundDatBuff[outS].val += pEntity->m_vel.x;
				break;

			case AXIS_Y:
				pBoundDatBuff[outS].val = pEntity->m_boundMin.y + pEntity->m_loc.y - BOUND_TOLER;

				if(pEntity->m_vel.y < 0)
					pBoundDatBuff[outS].val += pEntity->m_vel.y;
				break;

			case AXIS_Z:
				pBoundDatBuff[outS].val = pEntity->m_boundMin.z + pEntity->m_loc.z - BOUND_TOLER;

				if(pEntity->m_vel.z < 0)
					pBoundDatBuff[outS].val += pEntity->m_vel.z;
				break;
			}

			outS++;

			pBoundDatBuff[outS].ent    = pEntity;
			pBoundDatBuff[outS].bOpen  = false;

			switch(axis)
			{
			case AXIS_X:
				pBoundDatBuff[outS].val = pEntity->m_boundMax.x + pEntity->m_loc.x  + BOUND_TOLER;

				if(pEntity->m_vel.x > 0)
					pBoundDatBuff[outS].val += pEntity->m_vel.x;
				break;

			case AXIS_Y:
				pBoundDatBuff[outS].val = pEntity->m_boundMax.y + pEntity->m_loc.y  + BOUND_TOLER;

				if(pEntity->m_vel.y > 0)
					pBoundDatBuff[outS].val += pEntity->m_vel.y;
				break;

			case AXIS_Z:
				pBoundDatBuff[outS].val = pEntity->m_boundMax.z + pEntity->m_loc.z  + BOUND_TOLER;

				if(pEntity->m_vel.z > 0)
					pBoundDatBuff[outS].val += pEntity->m_vel.z;
				break;
			}

			outS++;
		}
		/*else
		{
			pBoundDatWalker->id.counter = 0;
			pBoundDatWalker++;
			pBoundDatWalker->id.counter = 0;
			pBoundDatWalker++;
		}*/
	}

	return outS;
}

int _BoundDatCompare( const void *arg1, const void *arg2 )
{
	EntityBoundDat *b1 = (EntityBoundDat *)arg1;
	EntityBoundDat *b2 = (EntityBoundDat *)arg2;

	if(b1->val < b2->val)
		return -1;
	else if(b1->val > b2->val)
		return 1;

	return 0;
}


//this is the Recursive Dimensional Clustering thing-a-magig fancy snancy
//ultra fast super hyper accurate awesome collision.
void RDC(const EntList & group, eAxis axis1, eAxis axis2, eAxis axis3, u32 boundStartInd = 0)
{
	//uh oh...
	if(boundStartInd >= g_entityBoundsSize)
	{ assert(0); return; }

	//check to see if the group is small enough for brute force
	if(group.size() < MAX_NUM_BRUTE_FORCE || axis1 == AXIS_INVALID)
	{
		_BrutalCollision(group);
		return;
	}

	u32 boundSize = group.size()*2;

	//uh oh...
	if(boundStartInd+boundSize > g_entityBoundsSize)
	{ assert(0); return; }

	EntityBoundDat *pBoundDatBuff = &g_entityBounds[boundStartInd];

	//create the buffer
	boundSize = _ListOpenCloseBounds(pBoundDatBuff, group, axis1);

	//now sort it
	qsort(pBoundDatBuff, boundSize, sizeof(EntityBoundDat), _BoundDatCompare);

	//////////////////////////////////////////////
	//let's slice 'n dice 'em
	EntList subGroup;

	s32 count = 0;

	eAxis newAxis1 = axis2;
	eAxis newAxis2 = axis3;
	
	u8 bGroupSubdivided = FALSE;

	for(u32 i = 0; i < boundSize; i++)
	{
		//Put this in the group
		if(pBoundDatBuff[i].bOpen)// && IDISVALID(pBoundDatBuff[i].id))
		{
			count++;

			subGroup.push_back(pBoundDatBuff[i].ent);
		}
		else
		{
			count--;

			//this means that we've grouped all stuff in the axis
			if(count == 0)
			{
				//now we will divide this group by recursing.

				//check to see if we want to furthermore subdivide
				if(i != boundSize-1)
					bGroupSubdivided = TRUE;

				//set new axis'
				if(bGroupSubdivided)
				{
					switch(axis1)
					{
					case AXIS_X:
						newAxis1 = AXIS_Y;
						newAxis2 = AXIS_Z;
						break;

					case AXIS_Y:
						newAxis1 = AXIS_X;
						newAxis2 = AXIS_Z;
						break;

					case AXIS_Z:
						newAxis1 = AXIS_X;
						newAxis2 = AXIS_Y;
						break;
					}
				}

				RDC(subGroup, newAxis1, newAxis2, AXIS_INVALID,
					boundStartInd+boundSize);

				subGroup.clear();
			}
		}
	}
}

#define PUSH_STUCK_CRAP		1.5f//1.0f//6

//collide 1 to 2
static inline void _CollidePair(hQBSP qbsp, 
							 hIENTITY pEntity1,
							 hIENTITY pEntity2)
{
	//do not collide with non-solid entities...
	if(TESTFLAGS(pEntity2->m_flag, ENTITY_FLAG_NONSOLID))
	{
		//we make exceptions for some...
		if(!TESTFLAGS(pEntity1->m_flag, ENTITY_FLAG_NONSOLID_COLLIDE))
		{
			return;
		}
	}

	//do not collide with own owner, if projectile... (shit, FIXME!)
	if(pEntity1->m_vftbl.AllowCollide)
		if(!pEntity1->m_vftbl.AllowCollide((hENTITY)pEntity1, (hENTITY)pEntity2))
			return;

	u8 ret;

	gfxTrace traceTemp={0}; traceTemp.t = 1;

	//D3DXVECTOR3 tempLoc(pEntity2->GetLoc());
	//pEntity2->SetLoc(pEntity2->GetLoc()+pEntity2->GetVel());

	ret = pEntity2->BoundCollide((hENTITY)pEntity1, qbsp, &traceTemp, 1);
	
	//do we officially collide with this chump?
	if(ret && traceTemp.t <= pEntity1->m_trace.t)
	{
		pEntity1->m_trace = traceTemp;
		pEntity1->m_bcollideWith = TRUE;
		pEntity1->m_lastCollided = pEntity2; //pEntity2->AddRef();

		//do something special upon collision...
		if(pEntity2->m_vftbl.EntityCollide)
			pEntity2->m_vftbl.EntityCollide((hENTITY)pEntity2, (hENTITY)pEntity1, &traceTemp);

		//add this entity to push list if it is pushable
		if(TESTFLAGS(pEntity2->m_flag, ENTITY_FLAG_PUSHABLE))
		{
			pEntity2->m_pushVel += pEntity1->m_vel;
		}

		if(TESTFLAGS(pEntity1->m_flag, ENTITY_FLAG_NONSOLID) && pEntity1->m_vftbl.EntityCollide)
			pEntity1->m_vftbl.EntityCollide((hENTITY)pEntity1, (hENTITY)pEntity2, &traceTemp);
	}
	//else
		//is this useful? FIXME!
	//	pEntity1->CB((hENTITY)pEntity1, ENTITYMSG_ENTITYNOCOLLIDE, (u32)pEntity2, 0);
}

static inline void _CollisionEntities(hQBSP qbsp, hIENTITY pEntity)
{
	const EntList &collideList = pEntity->GetCollideList();

	//collide with entities
	for(const_EntIt itE = collideList.begin(); itE != collideList.end(); itE++)
	{
		_CollidePair(qbsp, pEntity, *itE);
	}
}

//collide with map and entities in the entity list
static inline void _CollisionMap(hQBSP qbsp, hIENTITY pEntity)
{
	Vec3D intLoc;

	if(qbsp
		&& pEntity->m_vel != g_zeroVel 
		&& !TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_NOWORLDCOLLIDE))
	{
		intLoc = pEntity->m_loc + pEntity->m_vel;

		qbsp->Collision(&pEntity->m_boundMin, 
			&pEntity->m_boundMax, pEntity->m_loc, 
			intLoc,  &pEntity->m_trace, 1);

		if(pEntity->m_trace.t < 1 && pEntity->m_vftbl.WorldCollide)
			pEntity->m_vftbl.WorldCollide((hENTITY)pEntity, qbsp, &pEntity->m_trace);
	}

	_CollisionEntities(qbsp, pEntity);
}

#define TOLER_COLLIDE 0.003f//0.003f//0.55f//0.54f//0.6f//0.1f//

//calculate new point and determine if we need to move again.
//returns 1 if done with collision (means given entity
//will not collide with others).  (it can still be collided by others.)
static inline void _CollisionSet(hQBSP qbsp, hIENTITY pEntity)
{
	f32 nd;

	f32 mrT = pEntity->m_trace.t;

	Vec3D ptInOut, vel;

	if(pEntity->m_vel == g_zeroVel)
		goto DONE;

	if(mrT < 1)
	{
		ptInOut = pEntity->m_loc;
		vel     = pEntity->m_vel;

		Vec3D norm(pEntity->m_trace.norm);

		Vec3D newVel(0,0,0);

		////////////////////////////////////
		// get the sliding vector
		// check for extreme angle? FIXME!
		if(mrT > 0)
		{
			//if(!(vel.x == 0 && vel.y < 0 && vel.z == 0))
			//{
				nd = Vec3DDot(&norm, &vel);

				newVel = vel - norm*nd; 
			//}
		}
		////////////////////////////////////
#if 1
		mrT -= TOLER_COLLIDE;

		if(mrT < 0
			&& !TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_NONSOLID))
		{
			gfxTrace traceTmp=pEntity->m_trace;

			pEntity->m_vel = vel*mrT;
			//g_GFX->Vec3DNormal(&vel, &pEntity->pub.vel);
			//pEntity->pub.vel *= mrT;
			pEntity->m_trace.t = 1;
			//_CollisionMap(qbsp, pEntity);
			_CollisionEntities(qbsp, pEntity);

			if(pEntity->m_trace.t < 1)
			{
				vel = pEntity->m_vel;
				mrT = pEntity->m_trace.t;
			}
			else
				pEntity->m_trace = traceTmp;
		}
#else
			if(!TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_NONSOLID))
			{
				gfxTrace traceTmp=pEntity->m_trace;

				pEntity->m_vel = norm*0.01f;
				pEntity->m_trace.t = 1;
				_CollisionMap(qbsp, pEntity);
				//_CollisionEntities(qbsp, pEntity);

				ptInOut += pEntity->m_trace.t*(norm*0.01f);

				/*if(pEntity->trace.t < 1)
				{
					vel = pEntity->pub.vel;
					mrT = pEntity->trace.t;
				}
				else*/
				pEntity->m_vel = vel;
					pEntity->m_trace = traceTmp;
			}
#endif
		//get new point from the ray equation with 't'
		ptInOut += vel*mrT;

		////////////////////////////////////
		// check for stairs
		f32 ofsY = pEntity->m_boundRadius;//2;

		Vec3D stairLoc(ptInOut.x, ptInOut.y+ofsY, ptInOut.z);
		Vec3D stairVel(vel.x, 0, vel.z), stairVel2(0, -ofsY, 0);
		Vec3D stairLocNext(ptInOut.x+vel.x, ptInOut.y+ofsY, ptInOut.z+vel.z);

		// make sure we are solid
		if(!TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_NONSOLID))
		{
			Vec3D tmpLoc = pEntity->m_loc;
			Vec3D tmpVel = pEntity->m_vel;
			gfxTrace traceTmp=pEntity->m_trace;

			//check forward
			pEntity->m_trace.t = 1;

			pEntity->m_loc = stairLoc;
			pEntity->m_vel = stairVel;

			_CollisionMap(qbsp, pEntity);

			//if we don't collide, check downward
			if(pEntity->m_trace.t >= 1)
			{
				pEntity->m_loc = stairLocNext;
				pEntity->m_vel = stairVel2;

				_CollisionMap(qbsp, pEntity);
				
				//FIXME! : better stairs check
				if(pEntity->m_trace.t < 1
					&& pEntity->m_trace.norm.x == 0
					&& pEntity->m_trace.norm.z == 0)
				{
					ptInOut.x = stairLocNext.x;
					ptInOut.y = stairLocNext.y + pEntity->m_trace.t*(-ofsY);
					ptInOut.z = stairLocNext.z;

					//we don't want to collide this guy anymore

					pEntity->m_loc = ptInOut;

					pEntity->m_vel = g_zeroVel;

					goto DONE;
				}
				else //nothing happened
				{
					pEntity->m_loc = tmpLoc;
					pEntity->m_vel = tmpVel;
					pEntity->m_trace = traceTmp;
				}
			}
			else //nothing happened
			{
				pEntity->m_loc = tmpLoc;
				pEntity->m_vel = tmpVel;
				pEntity->m_trace = traceTmp;
			}
		}
		////////////////////////////////////

		//do something if we are colliding with world
		//if(!pEntity->m_bcollideWith)
			//pEntity->CollideWorld(qbsp, pEntity->m_gfxTrace);

		pEntity->m_loc = ptInOut;

		////////////////////////////////////
		// Move with new slide velocity
		if(newVel.x != 0 || newVel.y != 0 || newVel.z != 0)
		{
			pEntity->m_vel = newVel;
		}
		else
		{
			//we don't want to collide this guy anymore
			pEntity->m_vel = g_zeroVel;

			goto DONE;
		}
		////////////////////////////////////
	}
	//didn't collide with anything!
	else if(!pEntity->m_trace.bStartSolid && !pEntity->m_trace.bAllSolid)
	{
		pEntity->m_loc += pEntity->m_vel;

		//we don't want to collide this guy anymore
		pEntity->m_vel = g_zeroVel;
		
		goto DONE;
	}

DONE:

	//pEntity->m_gfxTrace.t = 1;

	return;
}

/////////////////////////////////////
// Name:	WorldCollisionExec
// Purpose:	collide everything!
// Output:	stuff moved around
// Return:	none
/////////////////////////////////////
//void WorldCollisionExec(hQBSP qbsp, const list<ID> & entities)
void WorldCollisionExec(hQBSP qbsp, const EntList & entities)
{
	s32 numDoneCollide = 0;

	hIENTITY pEntity;

	const_EntIt it;

	//////////////////////////////////////////////////////////////
	//group up the entities to each other
	RDC(entities, AXIS_X, AXIS_Y, AXIS_Z);
	//////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	//execute collision
	for(it = entities.begin(); it != entities.end(); it++)
	{
		pEntity = *it;

		if(pEntity)
		{
			if(pEntity->m_lastCollided) { /*pEntity->m_lastCollided->Release();*/ pEntity->m_lastCollided=0; }

			//move entities with ignore collision
			//or entities that are attached, they are not allowed to collide
			if(TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_IGNORE_COLLIDE)
				|| (pEntity->m_obj && pEntity->m_obj->IsAttached()))
			{
				pEntity->m_loc += pEntity->m_vel;
				pEntity->m_vel = g_zeroVel;
				continue;
			}

			for(u8 ack = 0; ack < 2; ack++)
//			do
			{
				//collide to world
				memset(&pEntity->m_trace, 0, sizeof(pEntity->m_trace));
				pEntity->m_trace.t = 1;
				pEntity->m_bcollideWith = FALSE;

				_CollisionMap(qbsp, pEntity);

				_CollisionSet(qbsp, pEntity);

				//check to see if we get stuck...
				//both must be solid
				/*EntityCommon *pColEnt = (EntityCommon *)IDPageQuery(pEntity->m_LastCollided);
				if(pColEnt 
					&& !pColEnt->CheckFlag(ENTITY_FLAG_NONSOLID)
					&& !pEntity->CheckFlag(ENTITY_FLAG_NONSOLID))
				{
					Bound *pBound = dynamic_cast<Bound*>(pColEnt);
					if(pEntity->BoundBoxCheck(pEntity->GetLoc(), *pBound, pColEnt->GetLoc()))
					{
						int asshole = 0;
					}
				}*/

				if(pEntity->m_vel == g_zeroVel)
					break;
					
			} //while(pEntity->pub.vel != g_zeroVel);

			//pEntity->pub.vel = g_zeroVel;

			pEntity->ClearCollideList();
		}
	}
	//////////////////////////////////////////////////////////////
}

/////////////////////////////////////
// Name:	WorldPreview
// Purpose:	preview collisions on an
//			entity
// Output:	stuff moved around
// Return:	none
/////////////////////////////////////
void WorldPreview (hENTITY ent)
{
	hIENTITY pEntity = static_cast<hIENTITY>(ent);

	if (0 == pEntity) return;

	//move entities with ignore collision
	//or entities that are attached, they are not allowed to collide
	if(TESTFLAGS(pEntity->m_flag, ENTITY_FLAG_IGNORE_COLLIDE)
		|| (pEntity->m_obj && pEntity->m_obj->IsAttached()))
	{
		pEntity->m_loc += pEntity->m_vel;
		pEntity->m_vel = g_zeroVel;
		return;
	}

	for (u8 ack = 0; ack < 2 && pEntity->m_vel != g_zeroVel; ack++)
	{
		//collide to world
		memset(&pEntity->m_trace, 0, sizeof(pEntity->m_trace));
		pEntity->m_trace.t = 1;
		pEntity->m_bcollideWith = FALSE;

		_CollisionMap(WorldGetMap(), pEntity);
		_CollisionSet(WorldGetMap(), pEntity);	
	}
}

/////////////////////////////////////
// Name:	WorldPreviewEx
// Purpose:	preview collisions on a
//			list of entities
// Output:	stuff moved around
// Return:	none
/////////////////////////////////////
void WorldPreviewEx (hQBSP qbsp, std::list<hENTITY> & entities)
{
	EntList ents;

	for (std::list<hENTITY>::iterator ei = entities.begin(); ei != entities.end(); ++ei)
	{
		hIENTITY iEnt = static_cast<hIENTITY>(*ei);

		ents.push_back(iEnt);

		iEnt->Update(qbsp);
	}

	WorldCollisionExec(qbsp, ents);
}