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
#ifndef H2CAPTAIN_H
#define H2CAPTAIN_H

#include "skill.h"
#include "spell_book.h"
#include "gamedefs.h"
#include "heroes.h"

class Castle;

class Captain : public HeroBase
{
  public:
    Captain(const Castle &);

    bool isValid(void) const;
    u8 GetAttack(void) const;
    u8 GetDefense(void) const;
    u8 GetPower(void) const;
    u8 GetKnowledge(void) const;
    s8 GetMorale(void) const;
    s8 GetLuck(void) const;
    Race::race_t GetRace(void) const;
    Color::color_t GetColor(void) const;
    const std::string & GetName(void) const;
    u8 GetType(void) const;
    u16 GetSpellPoints(void) const;
    const SpellBook & GetSpellBook(void) const;
    SpellBook & GetSpellBook(void);
    u8 GetLevelSkill(const Skill::Secondary::skill_t) const;

    const Army::army_t & GetArmy(void) const;
    Army::army_t & GetArmy(void);
    bool HasArtifact(const Artifact::artifact_t) const;
    BagArtifacts & GetBagArtifacts(void);
    const BagArtifacts & GetBagArtifacts(void) const;
    void TakeArtifacts(Heroes &);
    u16 GetMaxSpellPoints(void) const;
    void SetSpellPoints(const u16 point);

  private:
    const Castle & home;
    SpellBook spell_book;
    BagArtifacts artifacts;
    u16 spellPoints;
};

#endif
