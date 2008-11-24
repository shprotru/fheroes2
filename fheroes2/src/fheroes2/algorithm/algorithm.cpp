/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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

#include "heroes.h"
#include "castle.h"
#include "algorithm.h"

u32 Algorithm::CalculateExperience(const Army::army_t & army)
{
    u32 res = 0;
    for(u8 ii = 0; ii < army.Size(); ++ii) res += army.At(ii).Count() * Monster::GetStats(army.At(ii).Monster()).hp;

    return res;
}

u32 Algorithm::CalculateExperience(const Heroes & hero)
{
    return 500 + CalculateExperience(hero.GetArmy());
}

u32 Algorithm::CalculateExperience(const Castle & castle)
{
    return 500 + CalculateExperience(castle.GetArmy());
}
