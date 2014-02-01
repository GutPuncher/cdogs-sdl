/*
    C-Dogs SDL
    A port of the legendary (and fun) action/arcade cdogs.
    Copyright (c) 2014, Cong Xu
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include "map_static.h"

#include "actors.h"
#include "campaigns.h"
#include "gamedata.h"
#include "map_build.h"


void MapStaticLoad(Map *map, struct MissionOptions *mo, CharacterStore *store)
{
	Mission *m = mo->missionData;
	Vec2i v;
	for (v.y = 0; v.y < m->Size.y; v.y++)
	{
		for (v.x = 0; v.x < m->Size.x; v.x++)
		{
			int index = v.y * m->Size.x + v.x;
			unsigned short tile =
				*(unsigned short *)CArrayGet(&m->u.Static.Tiles, index);
			if (!AreKeysAllowed(gCampaign.Entry.mode))
			{
				tile &= MAP_MASKACCESS;
			}
			IMapSet(map, v, tile);
		}
	}

	for (int i = 0; i < (int)m->u.Static.Items.size; i++)
	{
		MapObjectPositions *mop = CArrayGet(&m->u.Static.Items, i);
		for (int j = 0; j < (int)mop->Positions.size; j++)
		{
			Vec2i *pos = CArrayGet(&mop->Positions, j);
			MapTryPlaceOneObject(map, *pos, MapObjectGet(mop->Index), 0, 0);
		}
	}

	for (int i = 0; i < (int)m->u.Static.Characters.size; i++)
	{
		CharacterPositions *cp = CArrayGet(&m->u.Static.Characters, i);
		for (int j = 0; j < (int)cp->Positions.size; j++)
		{
			Vec2i *pos = CArrayGet(&cp->Positions, j);
			Character *c = CArrayGet(&store->OtherChars, cp->Index);
			TActor *a = AddActor(c, NULL);
			Vec2i fullPos = Vec2iReal2Full(Vec2iCenterOfTile(*pos));
			MoveActor(a, fullPos.x, fullPos.y);
			ActorInit(a);
		}
	}
	
	for (int i = 0; i < (int)m->u.Static.Objectives.size; i++)
	{
		ObjectivePositions *op = CArrayGet(&m->u.Static.Objectives, i);
		MissionObjective *mobj = CArrayGet(&mo->missionData->Objectives, i);
		struct Objective *obj = CArrayGet(&mo->Objectives, op->Index);
		for (int j = 0; j < (int)op->Positions.size; j++)
		{
			Vec2i *pos = CArrayGet(&op->Positions, j);
			int *index = CArrayGet(&op->Indices, j);
			Vec2i realPos = Vec2iCenterOfTile(*pos);
			Vec2i fullPos = Vec2iReal2Full(realPos);
			switch (mobj->Type)
			{
				case OBJECTIVE_KILL:
				{
					TActor *a = AddActor(store->specials[*index], NULL);
					MoveActor(a, fullPos.x, fullPos.y);
					ActorInit(a);
				}
					break;
				case OBJECTIVE_COLLECT:
					MapPlaceCollectible(mo, op->Index, realPos);
					break;
				case OBJECTIVE_DESTROY:
					MapTryPlaceOneObject(
						map,
						*pos,
						obj->blowupObject,
						ObjectiveToTileItem(op->Index), 1);
					break;
				case OBJECTIVE_RESCUE:
				{
					TActor *a = AddActor(store->prisoners[*index], NULL);
					MoveActor(a, fullPos.x, fullPos.y);
					ActorInit(a);
				}
					break;
				default:
					// do nothing
					break;
			}
			obj->placed++;
		}
	}
	
	for (int i = 0; i < (int)m->u.Static.Keys.size; i++)
	{
		KeyPositions *kp = CArrayGet(&m->u.Static.Keys, i);
		for (int j = 0; j < (int)kp->Positions.size; j++)
		{
			Vec2i *pos = CArrayGet(&kp->Positions, j);
			MapPlaceKey(map, mo, *pos, kp->Index);
		}
	}
	
	if (Vec2iEqual(m->u.Static.Exit.Start, Vec2iZero()) &&
		Vec2iEqual(m->u.Static.Exit.End, Vec2iZero()))
	{
		GenerateRandomExitArea(map->Size, &map->ExitStart, &map->ExitEnd);
	}
	else
	{
		map->ExitStart = m->u.Static.Exit.Start;
		map->ExitEnd = m->u.Static.Exit.End;
	}
}
