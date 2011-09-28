/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cstdlib>
#include <map>
#include "maps.h"
#include "world.h"
#include "direction.h"
#include "settings.h"
#include "object.h"
#include "heroes.h"
#include "route.h"
#include "algorithm.h"

struct cell_t
{
    cell_t() : cost_g(MAXU16), cost_t(MAXU16), cost_d(MAXU16), parent(-1), open(true){};

    u16		cost_g;
    u16		cost_t;
    u16		cost_d;
    s32		parent;
    bool	open;
};

/*
bool ImpassableCorners(const s32 from, const Direction::vector_t to, const Heroes *hero)
{
    if( to & (Direction::TOP | Direction::BOTTOM | Direction::LEFT | Direction::RIGHT)) return false;

    if(to & (Direction::TOP_LEFT | Direction::BOTTOM_LEFT))
    {
    	if(Maps::isValidDirection(from, Direction::LEFT) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::LEFT)).isPassable(hero)) return true;
    }

    if(to & (Direction::TOP_RIGHT | Direction::BOTTOM_RIGHT))
    {
    	if(Maps::isValidDirection(from, Direction::RIGHT) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::RIGHT)).isPassable(hero)) return true;
    }

    if(to & (Direction::TOP_LEFT | Direction::TOP_RIGHT))
    {
    	if(Maps::isValidDirection(from, Direction::TOP) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::TOP)).isPassable(hero)) return true;
    }

    if(to & (Direction::BOTTOM_LEFT | Direction::BOTTOM_RIGHT))
    {
    	if(Maps::isValidDirection(from, Direction::BOTTOM) &&
	   !world.GetTiles(Maps::GetDirectionIndex(from, Direction::BOTTOM)).isPassable(hero)) return true;
    }

    return false;
}
*/

u32 GetCurrentLength(std::map<s32, cell_t> & list, s32 cur)
{
    u32 res = 0;
    const cell_t* cell = &list[cur];
    while(-1 != cell->parent){ cell = &list[cell->parent]; ++res; };
    return res;
}

bool FromTileToMonster(const s32 from, const s32 to)
{
    return 1 == Maps::GetApproximateDistance(from, to) &&
	MP2::OBJ_MONSTER == world.GetTiles(to).GetObject();
}

bool PassableFromToTile(const Heroes* hero, const s32 from, const s32 to, const Direction::vector_t direct, const s32 dst)
{
    if(to != dst)
    {
	const u16 mons = Maps::TileUnderProtection(to);
	// check monster protection
	if(mons &&
	    ! FromTileToMonster(to, dst)) return false;
    }

    // check direct from object
    if(hero && hero->GetIndex() == from &&
	! Object::AllowDirect(hero->GetUnderObject(), direct)) return false;

    // check obstacles as corners
    //if(ImpassableCorners(from, direct, hero)) return false;  // disable, need fix more objects with passable option

    const Maps::Tiles & fromTile = world.GetTiles(from);
    const Maps::Tiles & toTile = world.GetTiles(to);

    // check direct to object
    if(! Object::AllowDirect(toTile.GetObject(), Direction::Reflect(direct))) return false;


    if(! fromTile.isPassable(hero, direct, false)) return false;

    if(to != dst)
    switch(toTile.GetObject())
    {
	case MP2::OBJ_HEROES:
	case MP2::OBJ_MONSTER:
		return false;

	default: break;
    }

    bool fromTile2 = toTile.isPassable(hero, Direction::Reflect(direct), false);

    // end point
    if(to == dst)
    {
	if(!fromTile2)
	{
	    if(! hero)
		return false;
	    else
	    if(toTile.GetObject() == MP2::OBJ_HEROES)
		return false;
	    else
	    if(MP2::isActionObject(toTile.GetObject(), hero->isShipMaster()))
		return true;
	}
    }

    return fromTile2;
}

bool Algorithm::PathFind(std::list<Route::Step> *result, const s32 from, const s32 to, const u16 limit, const Heroes *hero)
{
    const u8 pathfinding = (hero ? hero->GetLevelSkill(Skill::Secondary::PATHFINDING) : Skill::Level::NONE);

    s32 cur = from;
    s32 alt = 0;
    s32 tmp = 0;
    std::map<s32, cell_t> list;
    std::map<s32, cell_t>::iterator it1 = list.begin();
    std::map<s32, cell_t>::iterator it2 = list.end();
    Direction::vector_t direct = Direction::CENTER;

    list[cur].cost_g = 0;
    list[cur].cost_t = 0;
    list[cur].parent = -1;
    list[cur].open   = false;

    while(cur != to)
    {
	LocalEvent::Get().HandleEvents(false);

	for(direct = Direction::TOP_LEFT; direct != Direction::CENTER; ++direct)
	{
    	    if(Maps::isValidDirection(cur, direct))
	    {
		tmp = Maps::GetDirectionIndex(cur, direct);

		if(list[tmp].open)
		{
		    // new
		    if(-1 == list[tmp].parent)
		    {
	    		const u16 costg = Maps::Ground::GetPenalty(tmp, direct, pathfinding);
			if(MAXU16 == costg) continue;

			if(PassableFromToTile(hero, cur, tmp, direct, to))
			{
			    cell_t & cell = list[tmp];
	    		    cell.cost_g = costg;
			    cell.parent = cur;
			    cell.open = true;
	    		    cell.cost_t = cell.cost_g + list[cur].cost_t;
			    cell.cost_d = 50 * Maps::GetApproximateDistance(tmp, to);
			}
		    }
		    // check alt
		    else
		    {
			alt = Maps::Ground::GetPenalty(cur, direct, pathfinding);
			if(list[tmp].cost_t > list[cur].cost_t + alt &&
			   PassableFromToTile(hero, cur, tmp, direct, to))
			{
			    list[tmp].parent = cur;
			    list[tmp].cost_g = alt;
			    list[tmp].cost_t = list[cur].cost_t + alt;
			}
		    }
    		}
	    }
	}

	list[cur].open = false;

	it1 = list.begin();
	alt = -1;
	tmp = MAXU16;

	DEBUG(DBG_OTHER, DBG_TRACE, "route, from: " << cur);
	
	// find minimal cost
	for(; it1 != it2; ++it1) if((*it1).second.open)
	{
	    const cell_t & cell2 = (*it1).second;

	    if(IS_DEBUG(DBG_OTHER, DBG_TRACE) && cell2.cost_g != MAXU16)
	    {
		direct = Direction::Get(cur, (*it1).first);
		if(Direction::UNKNOWN != direct)
		{
		    VERBOSE("\t\tdirect: " << Direction::String(direct) <<
			    ", index: " << (*it1).first <<
			    ", cost g: " << cell2.cost_g <<
			    ", cost t: " << cell2.cost_t <<
			    ", cost d: " << cell2.cost_d);
		}
	    }

	    if(cell2.cost_t + cell2.cost_d < tmp)
	    {
    		tmp = cell2.cost_t + cell2.cost_d;
    		alt = (*it1).first;
	    }
	}

	// not found, and exception
	if(MAXU16 == tmp || -1 == alt || (limit && GetCurrentLength(list, cur) > limit)) break;
	else
	DEBUG(DBG_OTHER, DBG_TRACE, "select: " << alt);

	cur = alt;
    }

    // save path
    if(cur == to)
    {
	while(cur != from)
	{
	    if(-1 == list[cur].parent) break;
	    alt = cur;
    	    cur = list[alt].parent;
	    if(result) result->push_front(Route::Step(cur, Direction::Get(cur, alt), list[alt].cost_g));
	}
        return true;
    }

    DEBUG(DBG_OTHER, DBG_TRACE, "not found" << ", from:" << from << ", to: " << to);
    list.clear();

    return false;
}
