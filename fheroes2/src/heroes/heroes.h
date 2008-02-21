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

#ifndef H2HEROES_H
#define H2HEROES_H

#include <string>
#include <vector>
#include <list>
#include "race.h"
#include "spell.h"
#include "color.h"
#include "morale.h"
#include "mp2.h"
#include "luck.h"
#include "dialog.h"
#include "army.h"
#include "skill.h"
#include "artifact.h"
#include "route.h"
#include "object.h"
#include "gamedefs.h"

#define HEROESMAXARTIFACT	14
#define HEROESMAXSKILL		8
#define HEROESMAXARMY		5
#define HEROESMAXCOUNT		60

/*
#define SCOUTINGBASE		4
*/

#define DEFAULT_KNGT_ATTACK	2
#define DEFAULT_KNGT_DEFENCE	2
#define DEFAULT_KNGT_POWER	1
#define DEFAULT_KNGT_KNOWLEDGE	1
#define DEFAULT_BARB_ATTACK	3
#define DEFAULT_BARB_DEFENCE	1
#define DEFAULT_BARB_POWER	1
#define DEFAULT_BARB_KNOWLEDGE	1
#define DEFAULT_NECR_ATTACK	1
#define DEFAULT_NECR_DEFENCE	0
#define DEFAULT_NECR_POWER	2
#define DEFAULT_NECR_KNOWLEDGE	2
#define DEFAULT_SORC_ATTACK	0
#define DEFAULT_SORC_DEFENCE	0
#define DEFAULT_SORC_POWER	2
#define DEFAULT_SORC_KNOWLEDGE	3
#define DEFAULT_WRLK_ATTACK	0
#define DEFAULT_WRLK_DEFENCE	0
#define DEFAULT_WRLK_POWER	3
#define DEFAULT_WRLK_KNOWLEDGE	2
#define DEFAULT_WZRD_ATTACK	0
#define DEFAULT_WZRD_DEFENCE	1
#define DEFAULT_WZRD_POWER	2
#define DEFAULT_WZRD_KNOWLEDGE	2

class Heroes : public Skill::Primary
{
public:
    typedef enum
    {
	// knight
	LORDKILBURN, SIRGALLANTH, ECTOR, GVENNETH, TYRO, AMBROSE, RUBY, MAXIMUS, DIMITRY,
	// barbarian
	THUNDAX, FINEOUS, JOJOSH, CRAGHACK, JEZEBEL, JACLYN, ERGON, TSABU, ATLAS,
	// sorceress
	ASTRA, NATASHA, TROYAN, VATAWNA, REBECCA, GEM, ARIEL, CARLAWN, LUNA,
	// warlock
	ARIE, ALAMAR, VESPER, CRODO, BAROK, KASTORE, AGAR, FALAGAR, WRATHMONT,
	// wizard
	MYRA, FLINT, DAWN, HALON, MYRINI, WILFREY, SARAKIN, KALINDRA, MANDIGAL,
	// necromancer
	ZOM, DARLANA, ZAM, RANLOO, CHARITY, RIALDO, ROXANA, SANDRO, CELIA,
	// from campain
	ROLAND, CORLAGON, ELIZA, ARCHIBALD, HALTON, BAX,
	// from extended
	// 60 - , 61 -, 62 -, 63 -, 64 -, 65 - GALLAVANT, 66 -, 67 -, 68 -, 69 -, 70 -
	// debugger
	SANDYSANDY, UNKNOWN
    } heroes_t;

    Heroes(heroes_t ht, Race::race_t rc, const std::string & str);

    bool isFreeman(void) const{ return Color::GRAY == color; };
    bool isVisited(const MP2::object_t & object) const;
    bool isVisited(const Maps::Tiles & tile) const;

    const Castle* inCastle(void) const;

    void LoadFromMP2(u16 map_index, const void *ptr,  const Color::color_t cl);

    Heroes::heroes_t GetHeroes(void) const{ return heroes; };
    Color::color_t GetColor(void) const{ return color; };
    Race::race_t GetRace(void) const{ return race; };
    const std::string & GetName(void) const{ return name; };
    const Point & GetCenter(void) const{ return mp; };
    const std::vector<Army::Troops> & GetArmy(void) const{ return army; };
    u8 GetCountArmy(void) const;

    u8 GetAttack(void) const;
    u8 GetDefense(void) const;
    u8 GetPower(void) const;
    u8 GetKnowledge(void) const;
    u32 GetExperience(void) const;
    u32 GetNextLevelExperience(u8 level) const;
    u16 GetMaxSpellPoints(void) const;
    u16 GetMaxMovePoints(void) const;
    u16 GetSpellPoints(void) const;
    u16 GetMovePoints(void) const;
    Morale::morale_t GetMorale(void) const;
    Luck::luck_t GetLuck(void) const;
    u8 GetLevel(void) const{ return 1; };
    const std::vector<Artifact::artifact_t> & GetArtifacts(void) const{ return artifacts; };
    Skill::Level::type_t GetLevelSkill(Skill::secondary_t type) const{ return secondary_skills.GetLevel(type); };

    u8 GetMobilityIndexSprite(void) const;
    u8 GetManaIndexSprite(void) const;

    Dialog::answer_t OpenDialog(bool readonly = false);
    void MeetingDialog(Heroes & heroes2);

    void Recruit(const Castle & castle);

    void ActionNewDay(void);
    void ActionNewWeek(void);
    void ActionNewMonth(void);
    void ActionAfterBattle(void);

    const Route & GetPath(void) const{ return path; };
    u16 FindPath(u16 dst_index);

    void SetVisited(const u32 index);
    void SetCenter(const Point& pt){ mp = pt; };
    void Goto(u16 dst_index);
    void Action(u16 dst_index);

private:
    std::string		name;
    Color::color_t	color;
    u32			experience;
    u16			magic_point;
    u16			move_point;

    Skill::Secondary	secondary_skills;

    std::vector<Artifact::artifact_t>	artifacts;
    std::vector<Army::Troops> army;
    //std::vector<Spell::spell_t>	books;

    const heroes_t	heroes;
    const Race::race_t	race;

    bool		army_spread;
    
    MP2::object_t	save_maps_general;

    Point		mp;
    Route		path;

    std::list<Maps::VisitIndexObject> visit_object;
};

#endif
