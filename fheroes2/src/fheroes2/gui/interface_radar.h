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

#ifndef H2INTERFACE_RADAR_H
#define H2INTERFACE_RADAR_H

#include "gamedefs.h"

namespace Interface
{
    class Radar : protected Rect
    {
    public:
	static Radar & Get(void);
	~Radar();

	void SetPos(s16, s16);
	const Rect & GetArea(void) const;

	void Build(void);
	void Generate(void);
	void Redraw(void);
	void RedrawArea(const u8 color = 0xFF);
	void HideArea(void);
	void RedrawCursor(void);

	void QueueEventProcessing(void);

    private:
	Surface *GetSurfaceFromColor(const u8);
	Radar();

        Surface *spriteArea;
	Surface *spriteCursor;
	SpriteCursor *cursor;

        Surface* sf_blue;
	Surface* sf_green;
        Surface* sf_red;
	Surface* sf_yellow;
        Surface* sf_orange;
	Surface* sf_purple;
        Surface* sf_gray;
	Surface* sf_black;
    };
};

#endif
