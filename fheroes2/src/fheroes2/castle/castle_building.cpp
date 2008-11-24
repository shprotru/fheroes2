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

#include "agg.h"
#include "castle.h"
#include "settings.h"
#include "cursor.h"

Rect Castle::GetCoordBuilding(building_t building, const Point & pt)
{
    switch(building)
    {
	case BUILD_THIEVESGUILD:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 0, pt.y + 130, 50, 60);
		case Race::BARB:	return Rect(pt.x + 478, pt.y + 100, 76, 42);
		case Race::SORC:	return Rect(pt.x + 423, pt.y + 165, 65, 49);
		case Race::WRLK:	return Rect(pt.x + 525, pt.y + 109, 60, 48);
		case Race::WZRD:	return Rect(pt.x + 507, pt.y + 55, 47, 42);
		case Race::NECR:	return Rect(pt.x + 291, pt.y + 134, 43, 59);
		default: break;
	    }
	    break;

        case BUILD_TAVERN:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 350, pt.y + 110, 46, 56);
		case Race::BARB:	return Rect(pt.x + 0, pt.y + 205, 125, 60);
		case Race::SORC:	return Rect(pt.x + 494, pt.y + 140, 131, 87);
		case Race::WRLK:	return Rect(pt.x + 479, pt.y + 100, 39, 52);
		case Race::WZRD:	return Rect(pt.x, pt.y + 160, 118, 50);
		// shrine
		case Race::NECR:	if(Settings::Get().Modes(Settings::PRICELOYALTY)) return Rect(pt.x + 453, pt.y + 36, 55, 96);
		default: break;
	    }
	    break;

        case BUILD_SHIPYARD:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 537, pt.y + 221, 103, 33);
		case Race::BARB:	return Rect(pt.x + 535, pt.y + 210, 105, 45);
		case Race::SORC:	return Rect(pt.x + 0, pt.y + 220, 134, 35);
		case Race::WRLK:	return Rect(pt.x + 520, pt.y + 206, 120, 47);
		case Race::WZRD:	return Rect(pt.x, pt.y + 218, 185, 35);
		case Race::NECR:	return Rect(pt.x + 516, pt.y + 221, 124, 28);
		default: break;
	    }
	    break;

        case BUILD_WELL:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 194, pt.y + 225, 29, 27);
		case Race::BARB:	return Rect(pt.x + 272, pt.y + 215, 44, 32);
		case Race::SORC:	return Rect(pt.x + 346, pt.y + 209, 43, 25);
		case Race::WRLK:	return Rect(pt.x + 348, pt.y + 221, 63, 30);
		case Race::WZRD:	return Rect(pt.x + 254, pt.y + 143, 19, 28);
		case Race::NECR:	return Rect(pt.x + 217, pt.y + 225, 23, 26);
		default: break;
	    }
	    break;

        case BUILD_STATUE:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 480, pt.y + 205, 45, 50);
		case Race::BARB:	return Rect(pt.x + 470, pt.y + 180, 30, 58);
		case Race::SORC:	return Rect(pt.x + 158, pt.y + 173, 17, 58);
		case Race::WRLK:	return Rect(pt.x + 473, pt.y + 172, 45, 51);
		case Race::WZRD:	return Rect(pt.x + 464, pt.y + 58, 26, 62);
		case Race::NECR:	return Rect(pt.x + 374, pt.y + 174, 26, 70);
		default: break;
	    }
	    break;

        case BUILD_MARKETPLACE:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 220, pt.y + 144, 115, 20);
		case Race::BARB:	return Rect(pt.x + 224, pt.y + 168, 52, 36);
		case Race::SORC:	return Rect(pt.x + 412, pt.y + 122, 56, 40);
		case Race::WRLK:	return Rect(pt.x + 391, pt.y + 185, 70, 26);
		case Race::WZRD:	return Rect(pt.x + 254, pt.y + 176, 105, 39);
		case Race::NECR:	return Rect(pt.x + 415, pt.y + 216, 85, 35);
		default: break;
	    }
	    break;

        case BUILD_WEL2:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 288, pt.y + 97, 63, 18);
		case Race::BARB:	return Rect(pt.x + 252, pt.y + 120, 44, 16);
		case Race::SORC:	return Rect(pt.x + 135, pt.y + 200, 63, 31);
		case Race::WRLK:	return Rect(pt.x + 69, pt.y + 46, 24, 124);
		case Race::WZRD:	return Rect(pt.x + 234, pt.y + 225, 107, 22);
		case Race::NECR:	return Rect(pt.x + 275, pt.y + 206, 68, 39);
		default: break;
	    }
	    break;

        case BUILD_MOAT:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 53, pt.y + 150, 93, 30);
		case Race::BARB:	return Rect(pt.x + 113, pt.y + 155, 106, 30);
		case Race::SORC:	return Rect(pt.x + 143, pt.y + 169, 98, 11);
		case Race::WRLK:	return Rect(pt.x + 327, pt.y + 166, 66, 17);
		case Race::WZRD:	return Rect(pt.x, pt.y + 91, 198, 11);
		case Race::NECR:	return Rect(pt.x + 336, pt.y + 169, 71, 15);
		default: break;
	    }
	    break;

        case BUILD_SPEC:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 0, pt.y + 80, 250, 20);
		case Race::BARB:	return Rect(pt.x + 223, pt.y + 79, 124, 27);
		case Race::SORC:	return Rect(pt.x + 147, pt.y + 0, 252, 30);
		case Race::WRLK:	return Rect(pt.x, pt.y + 162, 70, 77);
		case Race::WZRD:	return Rect(pt.x + 304, pt.y + 111, 95, 50);
		case Race::NECR:	return Rect(pt.x, pt.y, 640, 54);
		default: break;
	    }
	    break;

        case BUILD_CASTLE:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 0, pt.y + 55, 290, 85);
		case Race::BARB:	return Rect(pt.x + 0, pt.y + 88, 202, 62);
		case Race::SORC:	return Rect(pt.x + 0, pt.y + 67, 198, 100);
		case Race::WRLK:	return Rect(pt.x + 268, pt.y + 35, 131, 129);
		case Race::WZRD:	return Rect(pt.x, pt.y + 48, 187, 39);
		case Race::NECR:	return Rect(pt.x + 322, pt.y + 63, 73, 97);
		default: break;
	    }
	    break;

        case BUILD_CAPTAIN:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 293, pt.y + 109, 48, 27);
		case Race::BARB:	return Rect(pt.x + 210, pt.y + 104, 40, 35);
		case Race::SORC:	return Rect(pt.x + 238, pt.y + 136, 32, 34);
		case Race::WRLK:	return Rect(pt.x + 420, pt.y + 102, 52, 60);
		case Race::WZRD:	return Rect(pt.x + 210, pt.y + 55, 28, 32);
		case Race::NECR:	return Rect(pt.x + 423, pt.y + 126, 41, 46);
		default: break;
	    }
	    break;

        case BUILD_MAGEGUILD1:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 398, pt.y + 150, 58, 30);
		case Race::BARB:	return Rect(pt.x + 348, pt.y + 118, 50, 25);
		case Race::SORC:	return Rect(pt.x + 285, pt.y + 32, 55, 129);
		case Race::WRLK:	return Rect(pt.x + 590, pt.y + 135, 50, 35);
		case Race::WZRD:	return Rect(pt.x + 583, pt.y + 93, 57, 28);
		case Race::NECR:	return Rect(pt.x + 565, pt.y + 131, 73, 74);
		default: break;
	    }
	    break;

	case BUILD_MAGEGUILD2:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 398, pt.y + 128, 58, 52);
		case Race::BARB:	return Rect(pt.x + 348, pt.y + 94, 50, 49);
		case Race::SORC:	return Rect(pt.x + 285, pt.y + 32, 55, 129);
		case Race::WRLK:	return Rect(pt.x + 590, pt.y + 108, 50, 60);
		case Race::WZRD:	return Rect(pt.x + 585, pt.y + 69, 55, 50);
		case Race::NECR:	return Rect(pt.x + 568, pt.y + 102, 62, 104);
		default: break;
	    }
	    break;

	case BUILD_MAGEGUILD3:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 398, pt.y + 105, 58, 75);
		case Race::BARB:	return Rect(pt.x + 348, pt.y + 72, 50, 72);
		case Race::SORC:	return Rect(pt.x + 285, pt.y + 32, 55, 129);
		case Race::WRLK:	return Rect(pt.x + 590, pt.y + 77, 50, 90);
		case Race::WZRD:	return Rect(pt.x + 585, pt.y + 44, 55, 78);
		case Race::NECR:	return Rect(pt.x + 570, pt.y + 79, 56, 130);
		default: break;
	    }
	    break;

	case BUILD_MAGEGUILD4:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 398, pt.y + 85, 58, 95);
		case Race::BARB:	return Rect(pt.x + 348, pt.y + 48, 50, 96);
		case Race::SORC:	return Rect(pt.x + 285, pt.y + 32, 55, 129);
		case Race::WRLK:	return Rect(pt.x + 590, pt.y + 45, 50, 125);
		case Race::WZRD:	return Rect(pt.x + 585, pt.y + 20, 54, 102);
		case Race::NECR:	return Rect(pt.x + 570, pt.y + 61, 60, 146);
		default: break;
	    }
	    break;

	case BUILD_MAGEGUILD5:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 398, pt.y + 55, 58, 125);
		case Race::BARB:	return Rect(pt.x + 348, pt.y + 20, 50, 124);
		case Race::SORC:	return Rect(pt.x + 285, pt.y + 32, 55, 129);
		case Race::WRLK:	return Rect(pt.x + 590, pt.y + 14, 50, 155);
		case Race::WZRD:	return Rect(pt.x + 585 , pt.y, 57, 122);
		case Race::NECR:	return Rect(pt.x + 570, pt.y + 45, 61, 162);
		default: break;
	    }
	    break;

	case BUILD_TENT:
	    switch(race)
	    {
                case Race::KNGT:        return Rect(pt.x + 82, pt.y + 132, 42, 30);
                case Race::BARB:        return Rect(pt.x + 53, pt.y + 119, 67, 35);
                case Race::SORC:        return Rect(pt.x + 88, pt.y + 145, 62, 36);
                case Race::WRLK:        return Rect(pt.x + 308, pt.y + 140, 52, 28);
                case Race::WZRD:        return Rect(pt.x + 60, pt.y + 68, 46, 33);
                case Race::NECR:        return Rect(pt.x + 333, pt.y + 131, 49, 51);
		default: break;
	    }
	    break;

	case DWELLING_MONSTER1:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 195, pt.y + 175, 50, 40);
		case Race::BARB:	return Rect(pt.x + 258, pt.y + 142, 71, 41);
		case Race::SORC:	return Rect(pt.x + 478, pt.y + 70, 92, 62);
		case Race::WRLK:	return Rect(pt.x, pt.y + 63, 68, 53);
		case Race::WZRD:	return Rect(pt.x + 459, pt.y + 181, 45, 32);
		case Race::NECR:	return Rect(pt.x + 404, pt.y + 181, 56, 25);
		default: break;
	    }
	    break;

	case DWELLING_MONSTER2:
	case DWELLING_UPGRADE2:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 145, pt.y + 155, 58, 20);
		case Race::BARB:	return Rect(pt.x + 152, pt.y + 190, 68, 50);
		case Race::SORC:	return Rect(pt.x + 345, pt.y + 149, 70, 56);
		case Race::WRLK:	return Rect(pt.x + 248, pt.y + 192, 60, 55);
		case Race::WZRD:	return Rect(pt.x + 253, pt.y + 69, 90, 29);
		case Race::NECR:	return Rect(pt.x + 147, pt.y + 184, 92, 32);
		default: break;
	    }
	    break;

	case DWELLING_MONSTER3:
	case DWELLING_UPGRADE3:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 250, pt.y + 177, 70, 50);
		case Race::BARB:	return Rect(pt.x + 582, pt.y + 81, 58, 40);
		case Race::SORC:	return Rect(pt.x + 90, pt.y + 180, 56, 21);
		case Race::WRLK:	return Rect(pt.x + 504, pt.y + 53, 38, 30);
		case Race::WZRD:	return Rect(pt.x + 156, pt.y + 139, 74, 51);
		case Race::NECR:	return Rect(pt.x + 108, pt.y + 69, 117, 91);
		default: break;
	    }
	    break;

	case DWELLING_MONSTER4:
	case DWELLING_UPGRADE4:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 328, pt.y + 195, 100, 50);
		case Race::BARB:	return Rect(pt.x + 509, pt.y + 148, 123, 57);
		case Race::SORC:	return Rect(pt.x + 208, pt.y + 182, 127, 55);
		case Race::WRLK:	return Rect(pt.x + 154, pt.y + 168, 171, 36);
		case Race::WZRD:	return Rect(pt.x + 593, pt.y + 187, 47, 28);
		case Race::NECR:	return Rect(pt.x, pt.y + 154, 140, 74);
		default: break;
	    }
	    break;

	case DWELLING_MONSTER5:
 	case DWELLING_UPGRADE5:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 0, pt.y + 200, 150, 55);
		case Race::BARB:	return Rect(pt.x + 331, pt.y + 186, 121, 44);
		case Race::SORC:	return Rect(pt.x + 345, pt.y + 231, 165, 21);
		case Race::WRLK:	return Rect(pt.x + 149, pt.y + 98, 95, 58);
		case Race::WZRD:	return Rect(pt.x + 417, pt.y + 25, 35, 140);
		case Race::NECR:	return Rect(pt.x + 235, pt.y + 136, 53, 70);
		default: break;
	    }
	    break;

	case DWELLING_MONSTER6:
 	case DWELLING_UPGRADE6:
 	case DWELLING_UPGRADE7:
	    switch(race)
	    {
		case Race::KNGT:	return Rect(pt.x + 465, pt.y + 85, 175, 110);
		case Race::BARB:	return Rect(pt.x + 407, pt.y + 13, 109, 80);
		case Race::SORC:	return Rect(pt.x + 202, pt.y + 38, 42, 65);
		case Race::WRLK:	return Rect(pt.x + 98, pt.y + 25, 55, 229);
		case Race::WZRD:	return Rect(pt.x + 196, pt.y + 7, 129, 38);
		case Race::NECR:	return Rect(pt.x + 468, pt.y + 112, 92, 78);
		default: break;
	    }
	    break;

	default: break;
    }

    return Rect();
}

/* animation building */
void Castle::RedrawAnimationBuilding(const Point & dst_pt, const building_t build)
{
    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();

    const ICN::icn_t icn = GetICNBuilding(build, race);

    u8 index = 0;
	    
    // correct index
    switch(build)
    {
        case BUILD_MAGEGUILD1: index = 0; break;
        case BUILD_MAGEGUILD2: index = Race::NECR == race ? 6 : 1; break;
        case BUILD_MAGEGUILD3: index = Race::NECR == race ? 12 : 2; break;
        case BUILD_MAGEGUILD4: index = Race::NECR == race ? 18 : 3; break;
        case BUILD_MAGEGUILD5: index = Race::NECR == race ? 24 : 4; break;
        default: break;
    }

    const Sprite & sprite = AGG::GetICN(icn, index);
    const Rect src_rt(dst_pt.x + sprite.x(), dst_pt.y + sprite.y(), sprite.w(), sprite.h());

    Surface bg(src_rt.w, src_rt.h);
    Surface sf(src_rt.w, src_rt.h);

    sf.SetColorKey();

    bg.Blit(display, src_rt, 0, 0);
    sf.Blit(sprite);

    if(const u16 index2 = ICN::AnimationFrame(icn, index))
    {
	const Sprite & sprite2 = AGG::GetICN(icn, index2);
	sf.Blit(sprite2, sprite2.x() - sprite.x(), sprite2.y() - sprite.y());
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();
    u32 ticket = 0;
    u8 ii = 0;

    while(le.HandleEvents() && ii < 250)
    {
        if(!(ticket % ANIMATION_MEDIUM))
        {
    	    cursor.Hide();
    	    display.Blit(bg, src_rt);
    	    sf.SetAlpha(ii);
    	    display.Blit(sf, src_rt);
	    cursor.Show();
    	    display.Flip();
	    ii += 10;
	}

	++ticket;
    }
}

/* redraw town area */
void Castle::RedrawAllBuilding(const Point & dst_pt, const std::vector<building_t> & orders)
{
    static u32 ticket = 0;

    Display & display = Display::Get();

    // before redraw
    switch(race)
    {
	case Race::KNGT:
	{
	    const Sprite & townbkg = AGG::GetICN(ICN::TOWNBKG0, 0);
	    display.Blit(townbkg, dst_pt);
	}
	break;
	case Race::BARB:
	{
	    const Sprite & townbkg = AGG::GetICN(ICN::TOWNBKG1, 0);
	    display.Blit(townbkg, dst_pt);

    	    const Sprite & sprite0 = AGG::GetICN(ICN::TWNBEXT1, 1 + ticket % 5);
	    display.Blit(sprite0, dst_pt.x + sprite0.x(), dst_pt.y + sprite0.y());
	}
	break;
	case Race::SORC:
	{
	    const Sprite & townbkg = AGG::GetICN(ICN::TOWNBKG2, 0);
	    display.Blit(townbkg, dst_pt);
	}
	break;
	case Race::WRLK:
	{
	    const Sprite & townbkg = AGG::GetICN(ICN::TOWNBKG3, 0);
	    display.Blit(townbkg, dst_pt);
	}
	break;
	case Race::WZRD:
	{
	    const Sprite & townbkg = AGG::GetICN(ICN::TOWNBKG4, 0);
	    display.Blit(townbkg, dst_pt);
	}
	break;
	case Race::NECR:
	{
	    const Sprite & townbkg = AGG::GetICN(ICN::TOWNBKG5, 0);
	    display.Blit(townbkg, dst_pt);
	}
	break;
	default: break;
    }

    // sea anime
    if(Race::WZRD == race || (!(BUILD_SHIPYARD & building) && HaveNearlySea()))
    {
    	const Sprite * sprite50 = NULL;
    	const Sprite * sprite51 = NULL;

    	switch(race)
    	{
    	    case Race::KNGT:
    		sprite50 = &AGG::GetICN(ICN::TWNKEXT0, 0);
    		sprite51 = &AGG::GetICN(ICN::TWNKEXT0, 1 + ticket % 5);
    		break;
    	    case Race::BARB:
    		sprite50 = &AGG::GetICN(ICN::TWNBEXT0, 0);
    		sprite51 = &AGG::GetICN(ICN::TWNBEXT0, 1 + ticket % 5);
    		break;
    	    case Race::SORC:
    		sprite50 = &AGG::GetICN(ICN::TWNSEXT0, 0);
    		sprite51 = &AGG::GetICN(ICN::TWNSEXT0, 1 + ticket % 5);
    		break;
    	    case Race::NECR:
    		sprite50 = &AGG::GetICN(ICN::TWNNEXT0, 0);
    		sprite51 = &AGG::GetICN(ICN::TWNNEXT0, 1 + ticket % 5);
    		break;
    	    case Race::WRLK:
    		sprite50 = &AGG::GetICN(ICN::TWNWEXT0, 0);
    		sprite51 = &AGG::GetICN(ICN::TWNWEXT0, 1 + ticket % 5);
    		break;
    	    case Race::WZRD:
    		sprite50 = &AGG::GetICN(ICN::TWNZEXT0, 0);
    		sprite51 = &AGG::GetICN(ICN::TWNZEXT0, 1 + ticket % 5);
    		break;
    	    default:
    		break;
    	}

	if(sprite50) display.Blit(*sprite50, dst_pt.x + sprite50->x(), dst_pt.y + sprite50->y());

	if(sprite51) display.Blit(*sprite51, dst_pt.x + sprite51->x(), dst_pt.y + sprite51->y());
    }

    // redraw builds
    for(u8 ii = 0; ii < orders.size(); ++ii)
    {
	const building_t & build = orders[ii];

	if(! isBuild(build)) continue;

	RedrawBuilding(build, dst_pt, ticket);
    }

    ++ticket;
}

void Castle::RedrawBuilding(const building_t build, const Point & dst_pt, const u32 ticket)
{
    Display & display = Display::Get();

    building_t build2 = build;

	// correct build
	switch(build2)
	{
	    case DWELLING_MONSTER2: if(DWELLING_UPGRADE2 & building) build2 = DWELLING_UPGRADE2; break;
	    case DWELLING_MONSTER3: if(DWELLING_UPGRADE3 & building) build2 = DWELLING_UPGRADE3; break;
	    case DWELLING_MONSTER4: if(DWELLING_UPGRADE4 & building) build2 = DWELLING_UPGRADE4; break;
	    case DWELLING_MONSTER5: if(DWELLING_UPGRADE5 & building) build2 = DWELLING_UPGRADE5; break;
	    case DWELLING_MONSTER6: if(DWELLING_UPGRADE7 & building) build2 = DWELLING_UPGRADE7;
				    else
				    if(DWELLING_UPGRADE6 & building) build2 = DWELLING_UPGRADE6; break;

            case BUILD_MAGEGUILD1:  if(BUILD_MAGEGUILD5 & building) build2 = BUILD_MAGEGUILD5;
        			    else
        			    if(BUILD_MAGEGUILD4 & building) build2 = BUILD_MAGEGUILD4;
        			    else
        			    if(BUILD_MAGEGUILD3 & building) build2 = BUILD_MAGEGUILD3;
        			    else
        			    if(BUILD_MAGEGUILD2 & building) build2 = BUILD_MAGEGUILD2; break;
	    default: break;
	}

    	const ICN::icn_t icn = GetICNBuilding(build2, race);

    	u8 index = 0;
    	    
    	// correct index
	switch(build2)
        {
                case BUILD_MAGEGUILD1: index = 0; break;
                case BUILD_MAGEGUILD2: index = Race::NECR == race ? 6 : 1; break;
                case BUILD_MAGEGUILD3: index = Race::NECR == race ? 12 : 2; break;
                case BUILD_MAGEGUILD4: index = Race::NECR == race ? 18 : 3; break;
                case BUILD_MAGEGUILD5: index = Race::NECR == race ? 24 : 4; break;
                default: break;
        }

    	// simple first sprite
    	const Sprite & sprite1 = AGG::GetICN(icn, index);
	display.Blit(sprite1, dst_pt.x + sprite1.x(), dst_pt.y + sprite1.y());

    	// second anime sprite
    	if(const u16 index2 = ICN::AnimationFrame(icn, index, ticket))
	{
	    const Sprite & sprite2 = AGG::GetICN(icn, index2);
	    display.Blit(sprite2, dst_pt.x + sprite2.x(), dst_pt.y + sprite2.y());
	}

	// shipyard
	if(BUILD_SHIPYARD == build2)
    	{
	    // boat
	    if(Modes(BOATPRESENT))
	    {
		const ICN::icn_t icn2 = GetICNBoat(race);

    		const Sprite & sprite40 = AGG::GetICN(icn2, 0);
		display.Blit(sprite40, dst_pt.x + sprite40.x(), dst_pt.y + sprite40.y());

    		if(const u16 index2 = ICN::AnimationFrame(icn2, 0, ticket))
		{
		    const Sprite & sprite41 = AGG::GetICN(icn2, index2);
		    display.Blit(sprite41, dst_pt.x + sprite41.x(), dst_pt.y + sprite41.y());
		}
	    }
	    else
	    {
    		if(const u16 index2 = ICN::AnimationFrame(icn, index, ticket))
		{
		    const Sprite & sprite3 = AGG::GetICN(icn, index2);
		    display.Blit(sprite3, dst_pt.x + sprite3.x(), dst_pt.y + sprite3.y());
		}
	    }
	}
	else
	// sorc and anime wel2 or statue
	if(Race::SORC == race && BUILD_WEL2 == build2)
	{
	    const ICN::icn_t icn2 = BUILD_STATUE & building ? ICN::TWNSEXT1 : icn;

    	    const Sprite & sprite20 = AGG::GetICN(icn2, 0);
	    display.Blit(sprite20, dst_pt.x + sprite20.x(), dst_pt.y + sprite20.y());

    	    if(const u16 index2 = ICN::AnimationFrame(icn2, 0, ticket))
	    {
		const Sprite & sprite21 = AGG::GetICN(icn2, index2);
		display.Blit(sprite21, dst_pt.x + sprite21.x(), dst_pt.y + sprite21.y());
	    }
	}

}
