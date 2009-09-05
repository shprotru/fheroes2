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
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "dialog.h"

bool Dialog::SelectCount(const std::string &header, u16 min, u16 max, u16 & cur)
{
    Display & display = Display::Get();

    if(min >= max) min = 0;
    if(cur > max || cur < min) cur = min;

    const ICN::icn_t system = Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    // preload
    AGG::PreloadObject(system);

    // cursor
    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    Box box(60, OK|CANCEL);

    const Rect & pos = box.GetArea();
    Point pt;
    std::string message;
    Text text;

    // text
    text.Set(header, Font::BIG);
    pt.x = pos.x + (pos.w - text.w()) / 2;
    pt.y = pos.y;
    text.Blit(pt);

    // sprite edit
    const Surface & sprite_edit = AGG::GetICN(ICN::TOWNWIND, 4);
    pt.x = pos.x + 80;
    pt.y = pos.y + 55;
    display.Blit(sprite_edit, pt);

    message.clear();
    String::AddInt(message, cur);
    text.Set(message);
    pt.x = pos.x + 80 + (sprite_edit.w() - text.w()) / 2;
    pt.y = pos.y + 56;
    text.Blit(pt);

    // buttons
    pt.x = pos.x + 150;
    pt.y = pos.y + 51;
    Button buttonUp(pt, ICN::TOWNWIND, 5, 6);

    pt.x = pos.x + 150;
    pt.y = pos.y + 67;
    Button buttonDn(pt, ICN::TOWNWIND, 7, 8);

    pt.x = pos.x;
    pt.y = pos.y + pos.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button buttonOk(pt, system, 1, 2);

    pt.x = pos.x + pos.w - AGG::GetICN(system, 3).w();
    pt.y = pos.y + pos.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
    Button buttonCancel(pt, system, 3, 4);

    LocalEvent & le = LocalEvent::Get();

    buttonUp.Draw();
    buttonDn.Draw();
    buttonOk.Draw();
    buttonCancel.Draw();

    bool zero = false;
    bool first = true;
    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
	le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();
	le.MousePressLeft(buttonUp) ? buttonUp.PressDraw() : buttonUp.ReleaseDraw();
	le.MousePressLeft(buttonDn) ? buttonDn.PressDraw() : buttonDn.ReleaseDraw();

	if(le.KeyPress(KEY_BACKSPACE))
	{
	    if(min < cur)
	    {
		cur /= 10;
		if(cur < min)
		{
		    cur = min;
		    zero = true;
		    first = true;
		}
	    }

	    cursor.Hide();
	    pt.x = pos.x + 80;
	    pt.y = pos.y + 55;
	    display.Blit(sprite_edit, pt);

	    message.clear();
	    String::AddInt(message, cur);
	    text.Set(message);
	    pt.x = pos.x + 80 + (sprite_edit.w() - text.w()) / 2;
	    pt.y = pos.y + 56;
	    text.Blit(pt);
	    cursor.Show();
	    display.Flip();
	}

	if(le.KeyPress() && KEY_0 <= le.KeyValue() && KEY_9 >= le.KeyValue())
	{
	    if(first)
	    {
		cur = 0;
		first = false;
	    }

	    if(max > cur)
	    {
		if(zero)
		{
		    cur = 0;
		    zero = false;
		}
		cur *= 10;
		switch(le.KeyValue())
		{
		    case KEY_1:	cur += 1; break;
		    case KEY_2:	cur += 2; break;
		    case KEY_3:	cur += 3; break;
		    case KEY_4:	cur += 4; break;
		    case KEY_5:	cur += 5; break;
		    case KEY_6:	cur += 6; break;
		    case KEY_7:	cur += 7; break;
		    case KEY_8:	cur += 8; break;
		    case KEY_9:	cur += 9; break;
		    default: break;
		}
		if(cur > max) cur = max;
	    }

	    cursor.Hide();
	    pt.x = pos.x + 80;
	    pt.y = pos.y + 55;
	    display.Blit(sprite_edit, pt);

	    message.clear();
	    String::AddInt(message, cur);
	    text.Set(message);
	    pt.x = pos.x + 80 + (sprite_edit.w() - text.w()) / 2;
	    pt.y = pos.y + 56;
	    text.Blit(pt);
	    cursor.Show();
	    display.Flip();
	}

	// up
	if((le.MouseWheelUp(pos) ||
            le.MouseClickLeft(buttonUp)) && cur < max)
	{
	    ++cur;

	    cursor.Hide();
	    pt.x = pos.x + 80;
	    pt.y = pos.y + 55;
	    display.Blit(sprite_edit, pt);

	    message.clear();
	    String::AddInt(message, cur);
	    text.Set(message);
	    pt.x = pos.x + 80 + (sprite_edit.w() - text.w()) / 2;
	    pt.y = pos.y + 56;
	    text.Blit(pt);
	    cursor.Show();
	    display.Flip();
	}

	// down
	if((le.MouseWheelDn(pos) ||
            le.MouseClickLeft(buttonDn)) && min < cur)
	{
	    --cur;

	    cursor.Hide();
	    pt.x = pos.x + 80;
	    pt.y = pos.y + 55;
	    display.Blit(sprite_edit, pt);

	    message.clear();
	    String::AddInt(message, cur);
	    text.Set(message);
	    pt.x = pos.x + 80 + (sprite_edit.w() - text.w()) / 2;
	    pt.y = pos.y + 56;
	    text.Blit(pt);
	    cursor.Show();
	    display.Flip();
	}

        if(le.KeyPress(KEY_RETURN) || le.MouseClickLeft(buttonOk)){ cursor.Hide(); return true; }
	if(le.KeyPress(KEY_ESCAPE) || le.MouseClickLeft(buttonCancel)){ cur = 0;  break; }
    }
    cursor.Hide();
    return false;
}

bool Dialog::InputString(const std::string &header, std::string &res)
{
    const ICN::icn_t system = Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM;

    Display & display = Display::Get();
    Cursor & cursor = Cursor::Get();
    cursor.Hide();
    Cursor::themes_t oldcursor = cursor.Themes();
    cursor.SetThemes(cursor.POINTER);

    if(res.size()) res.clear();
    res.reserve(48);

    TextBox textbox(header, Font::BIG, BOXAREA_WIDTH);
    Point dst_pt;
    const Surface & sprite = AGG::GetICN((Settings::Get().EvilInterface() ? ICN::BUYBUILD : ICN::BUYBUILE), 3);

    Box box(10 + textbox.h() + 10 + sprite.h(), OK|CANCEL);
    const Rect & box_rt = box.GetArea();

    // text
    dst_pt.x = box_rt.x + (box_rt.w - textbox.w()) / 2;
    dst_pt.y = box_rt.y + 10;
    textbox.Blit(dst_pt);

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
    Button buttonOk(dst_pt, system, 1, 2);

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
    Button buttonCancel(dst_pt, system, 3, 4);

    dst_pt.y = box_rt.y + 10 + textbox.h() + 10;
    dst_pt.x = box_rt.x + (box_rt.w - sprite.w()) / 2;
    display.Blit(sprite, dst_pt);

    LocalEvent & le = LocalEvent::Get();

    buttonOk.SetDisable(res.empty());
    buttonOk.Draw();
    buttonCancel.Draw();

    Text text("_", Font::BIG);
    display.Blit(sprite, dst_pt);
    text.Blit(dst_pt.x + (sprite.w() - text.w()) / 2, dst_pt.y - 1);

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
	buttonOk.isEnable() && le.MousePressLeft(buttonOk) ? buttonOk.PressDraw() : buttonOk.ReleaseDraw();
        le.MousePressLeft(buttonCancel) ? buttonCancel.PressDraw() : buttonCancel.ReleaseDraw();

        if(le.KeyPress(KEY_RETURN) || (buttonOk.isEnable() && le.MouseClickLeft(buttonOk))) break;
	else
	if(le.KeyPress(KEY_ESCAPE) || le.MouseClickLeft(buttonCancel)){ res.clear(); break; }
	else
	if(le.KeyPress())
	{
	    String::AppendKey(res, le.KeyValue(), le.KeyMod());

	    buttonOk.SetDisable(res.empty());
	    buttonOk.Draw();

	    text.Set(res + "_");
	    if(text.w() < sprite.w() - 24)
	    {
		cursor.Hide();
		display.Blit(sprite, dst_pt);
		text.Blit(dst_pt.x + (sprite.w() - text.w()) / 2, dst_pt.y - 1);
		cursor.Show();
		display.Flip();
	    }
	}
    }

    cursor.SetThemes(oldcursor);
    cursor.Hide();

    return res.size();
}
