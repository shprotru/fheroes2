/*
    freeHeroes2 engine
    turn-based game engine (clone of Heroes Of the Might and Magic II)
    Copyright (C) 2006

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    3DO, New World Computing, Heroes of Might and Magic, and the related
    3DO logos are trademarks and or registered trademarks of 3DO Company.
    All other trademarks belong to their respective owners.

    Web: http://sourceforge.net/projects/fheroes2

    Description:
*/

#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#include "gamedefs.h"
#include "tools.h"
#include "element.h"
#include "config.h"
#include "animation.h"
#include "draw.h"
#include "cursor.h"
#include "loadgame.h"
#include "mp2maps.h"
#include "kingdom.h"
#include "heroes.h"
#include "heroesinfo.h"
#include "debug.h"
#include "payment.h"
#include "spell.h"
#include "magictower.h"
#include "castle.h"
#include "castleaction.h"
#include "castle_kngt.h"
#include "castle_brbn.h"
#include "castle_scrs.h"
#include "castle_ncrm.h"
#include "castle_wrlk.h"
#include "castle_wzrd.h"

void RedrawCastleAnimation(void);
void RedrawBottomBar(void);
void RedrawRamkaCastleName(void);
void RedrawHeroesMonster(E_NAMEHEROES);
void UpdateCastleBuilding(void);

ACTION ActionCASTLELOOP(INTERFACEACTION *); 

ACTION ActionExitCastle(void); 
ACTION ActionLeftCastle(void); 
ACTION ActionRightCastle(void); 
ACTION ActionClickRedistributeMonster(void);
ACTION ActionClickCastleMonster(void);
ACTION ActionClickCastleMonsterEmpty(Uint8);
ACTION ActionClickHeroesMonster(void);
ACTION ActionClickHeroesMonsterEmpty(Uint8);
ACTION ActionClickHeroes(void);

static 	S_CASTLE	*ptrCastle	= NULL;
static 	Uint8		countCastle	= 0;

static	S_CASTLE	*currentCastle	= NULL;
	E_NAMEHEROES	heroesName 	= HEROESNULL;

	S_ANIMATION    	*castanim	= NULL;
	INTERFACEACTION *castlact	= NULL;

static	BOOL		flagUpdateBuilding = FALSE;

struct {
    Uint8		select;
    BOOL		castle;
    SDL_Rect            rect;
    SDL_Surface         *back;
    SDL_Surface         *cursor;
    BOOL                use;
} backMonsterCursor;

BOOL	AddCastle(FILE *fd, Uint8 seek, Uint8 ax, Uint8 ay){

    if(! fd){
	fprintf(stderr, "AddCastle: descriptor NULL\n");
	return FALSE;
    }

    Uint16 countBlock = 0;
    MP2CASTLE *ptr = NULL;

    while(seek){
	fread(&countBlock, sizeof(Uint16), 1, fd);
	fseek(fd, countBlock, SEEK_CUR);
	--seek;
    }

    fread(&countBlock, sizeof(Uint16), 1, fd);
    if(0x0046 != countBlock){
	fprintf(stderr, "AddCastle: error magic: %hX\n", countBlock);
	return FALSE;
    }

    if(NULL == (ptr = (MP2CASTLE *) malloc(sizeof(MP2CASTLE)))){
	fprintf(stderr, "AddCastle: error malloc: %d\n", sizeof(MP2CASTLE));
	return FALSE;
    }

    memset(ptr, 0, sizeof(MP2CASTLE));

    fread((char *) &ptr->color, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->customBuilding, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->building, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->dwelling, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->magicTower, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->customTroops, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster1, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster2, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster3, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster4, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->monster5, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->count1, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count2, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count3, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count4, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->count5, 1, sizeof(Uint16), fd);
    fread((char *) &ptr->capitan, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->customCastleName, 1, sizeof(Uint8), fd);
    fread((char *) ptr->castleName, 1, 13, fd);
    fread((char *) &ptr->type, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->castle, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->allowCastle, 1, sizeof(Uint8), fd);
    fread((char *) &ptr->unknown, 1, 29, fd);

    if(NULL == (ptrCastle = (S_CASTLE *) realloc(ptrCastle, sizeof(S_CASTLE) * (countCastle + 1)))){
	fprintf(stderr, "AddCastle: error malloc: %d\n", sizeof(S_CASTLE) * (countCastle + 1));
	return FALSE;
    }

	switch(ptr->color){
	
	    case 0:
		ptrCastle[countCastle].color = BLUE;
		break;
		
	    case 1:
		ptrCastle[countCastle].color = GREEN;
		break;
		
	    case 2:
		ptrCastle[countCastle].color = RED;
		break;
		
	    case 3:
		ptrCastle[countCastle].color = YELLOW;
		break;
		
	    case 4:
		ptrCastle[countCastle].color = ORANGE;
		break;
		
	    case 5:
		ptrCastle[countCastle].color = PURPLE;
		break;

	    case 0xff:
		ptrCastle[countCastle].color = GRAY;
		break;

	    default:
		ptrCastle[countCastle].color = GRAY;
		fprintf(stderr, "AddCastle: unknown colors, default GRAY\n");
		break;
	}

	switch(ptr->type){
	
	    case 0:
		ptrCastle[countCastle].race = KNIGHT;
		break;

	    case 1:
		ptrCastle[countCastle].race = BARBARIAN;
		break;

	    case 2:
		ptrCastle[countCastle].race = SORCERESS;
		break;

	    case 3:
		ptrCastle[countCastle].race = WARLOCK;
		break;

	    case 4:
		ptrCastle[countCastle].race = WIZARD;
		break;

	    case 5:
		ptrCastle[countCastle].race = NECROMANCER;
		break;

	    // rnd
	    case 6:
		ptrCastle[countCastle].race = rand() % BOMG;
		break;
	
	    default:
		ptrCastle[countCastle].race = KNIGHT;
		fprintf(stderr, "AddCastle: unknown race, default KNIGHT\n");
		break;
	}

	strncpy(ptrCastle[countCastle].name, ptr->castleName, 13);

	if(ptr->customBuilding){
	    ptrCastle[countCastle].building = ptr->building;
	    ptrCastle[countCastle].dwelling = ptr->dwelling;
	}else{
	    ptrCastle[countCastle].building = BUILD_TAVERN; // BUILD_THIEVESGUILD BUILD_TAVERN BUILD_SHIPYARD BUILD_WELL BUILD_STATUE BUILD_LEFTTURRET BUILD_RIGHTTURRET BUILD_MARKETPLACE BUILD_MOAT BUILD_WEL2 BUILD_SPEC
	    ptrCastle[countCastle].dwelling = DWELLING_MONSTER1 | DWELLING_MONSTER2; // DWELLING_MONSTER1 DWELLING_MONSTER2 DWELLING_MONSTER3 DWELLING_MONSTER4 DWELLING_MONSTER5 DWELLING_MONSTER6 DWELLING_UPGRADE2 DWELLING_UPGRADE3 DWELLING_UPGRADE4 DWELLING_UPGRADE5 DWELLING_UPGRADE6
	}

	if(0 < ptr->magicTower) FillMageGuildLevel(&ptrCastle[countCastle].mageGuild, &ptrCastle[countCastle], MAGIC_LEVEL1);
	if(1 < ptr->magicTower) FillMageGuildLevel(&ptrCastle[countCastle].mageGuild, &ptrCastle[countCastle], MAGIC_LEVEL2);
	if(2 < ptr->magicTower) FillMageGuildLevel(&ptrCastle[countCastle].mageGuild, &ptrCastle[countCastle], MAGIC_LEVEL3);
	if(3 < ptr->magicTower) FillMageGuildLevel(&ptrCastle[countCastle].mageGuild, &ptrCastle[countCastle], MAGIC_LEVEL4);
	if(4 < ptr->magicTower) FillMageGuildLevel(&ptrCastle[countCastle].mageGuild, &ptrCastle[countCastle], MAGIC_LEVEL5);
	

	if(ptr->customTroops){
	    ptrCastle[countCastle].army[0].monster = ptr->monster1;
	    ptrCastle[countCastle].army[1].monster = ptr->monster2;
	    ptrCastle[countCastle].army[2].monster = ptr->monster3;
	    ptrCastle[countCastle].army[3].monster = ptr->monster4;
	    ptrCastle[countCastle].army[4].monster = ptr->monster5;
	    ptrCastle[countCastle].army[0].count = ptr->count1;
	    ptrCastle[countCastle].army[1].count = ptr->count2;
	    ptrCastle[countCastle].army[2].count = ptr->count3;
	    ptrCastle[countCastle].army[3].count = ptr->count4;
	    ptrCastle[countCastle].army[4].count = ptr->count5;

	}else{	
	    ptrCastle[countCastle].army[0].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 1));
	    ptrCastle[countCastle].army[1].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 2));
	    switch(ptrCastle[countCastle].race){
	
		case KNIGHT:
		    ptrCastle[countCastle].army[0].monster = PEASANT;
		    ptrCastle[countCastle].army[1].monster = ARCHER;
		    break;

		case BARBARIAN:
		    ptrCastle[countCastle].army[0].monster = GOBLIN;
		    ptrCastle[countCastle].army[1].monster = ORC;
		    break;

		case SORCERESS:
		    ptrCastle[countCastle].army[0].monster = SPRITE;
		    ptrCastle[countCastle].army[1].monster = DWARF;
		    break;

		case WARLOCK:
		    ptrCastle[countCastle].army[0].monster = CENTAUR;
		    ptrCastle[countCastle].army[1].monster = GARGOYLE;
		    break;

		case WIZARD:
		    ptrCastle[countCastle].army[0].monster = HALFLING;
		    ptrCastle[countCastle].army[1].monster = BOAR;
		    break;

		case NECROMANCER:
		    ptrCastle[countCastle].army[0].monster = SKELETON;
		    ptrCastle[countCastle].army[1].monster = ZOMBIE;
		    break;
	
		default:
		    break;
	    }

	    ptrCastle[countCastle].army[0].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 1));
	    ptrCastle[countCastle].army[1].count = 2 * GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 2));

	    ptrCastle[countCastle].army[2].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[3].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[4].monster = MONSTERNONE;
	    ptrCastle[countCastle].army[2].count = 0;
	    ptrCastle[countCastle].army[3].count = 0;
	    ptrCastle[countCastle].army[4].count = 0;
	}

    if(ptrCastle[countCastle].dwelling & DWELLING_MONSTER1)
	ptrCastle[countCastle].monster[0] = GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 1));
    else
	ptrCastle[countCastle].monster[0] = 0;

    if(ptrCastle[countCastle].dwelling & DWELLING_MONSTER2)
	ptrCastle[countCastle].monster[1] = GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 2));
    else
	ptrCastle[countCastle].monster[1] = 0;

    if(ptrCastle[countCastle].dwelling & DWELLING_MONSTER3)
	ptrCastle[countCastle].monster[2] = GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 3));
    else
	ptrCastle[countCastle].monster[2] = 0;

    if(ptrCastle[countCastle].dwelling & DWELLING_MONSTER4)
	ptrCastle[countCastle].monster[3] = GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 4));
    else
	ptrCastle[countCastle].monster[3] = 0;

    if(ptrCastle[countCastle].dwelling & DWELLING_MONSTER5)
	ptrCastle[countCastle].monster[4] = GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 5));
    else
	ptrCastle[countCastle].monster[4] = 0;

    if(ptrCastle[countCastle].dwelling & DWELLING_MONSTER6)
	ptrCastle[countCastle].monster[5] = GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[countCastle], 6));
    else
	ptrCastle[countCastle].monster[5] = 0;

    if(ptr->capitan) ptrCastle[countCastle].building |= BUILD_CAPTAIN;

    if(ptr->castle)
        ptrCastle[countCastle].building |= BUILD_CASTLE;

    if(ptr->allowCastle)
        ptrCastle[countCastle].allowCastle = FALSE;
    else
        ptrCastle[countCastle].allowCastle = TRUE;
    
    ptrCastle[countCastle].boat = FALSE;
    ptrCastle[countCastle].af = SPREAD;

    if(ax < 2) ptrCastle[countCastle].pos.x = 0; else ptrCastle[countCastle].pos.x = ax - 2;
    if(ay < 3) ptrCastle[countCastle].pos.y = 0; else ptrCastle[countCastle].pos.y = ay - 3;

    ptrCastle[countCastle].ax = ax;
    ptrCastle[countCastle].ay = ay;
    ptrCastle[countCastle].pos.w = 5;
    ptrCastle[countCastle].pos.h = 5;


    ptrCastle[countCastle].next = NULL;

    ++countCastle;

    free(ptr);

    return TRUE;
}

void	FreeCastle(void){

    if(ptrCastle) free(ptrCastle);
    countCastle = 0;
}

E_RACE GetRaceRNDCastle(Uint8 ax, Uint8 ay){

    Uint8 i;
    SDL_Rect rect;
    
    rect.w = 8;
    rect.h = 5;

    for(i = 0; i < countCastle; ++i){
	if(ptrCastle[i].pos.x > 3){ rect.x = ptrCastle[i].pos.x - 3; }else{ rect.x = 0; }
	rect.y = ptrCastle[i].pos.y;
	if(ValidPoint(&rect, ax, ay))   return ptrCastle[i].race;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetRaceRNDCastle: unknown castle, ax: %d, ay: %d\n", ax, ay);
    return BOMG;
}

const S_CASTLE  *GetCurrentCastle(void){

    return currentCastle;
}

S_CASTLE *GetStatCastlePos(Uint8 ax, Uint8 ay){

    Uint8 i;

    for(i = 0; i < countCastle; ++i)

	if(ValidPoint(&ptrCastle[i].pos, ax, ay))

	    return &ptrCastle[i];

    return NULL;
}

S_CASTLE *GetStatCastle(Uint8 index){

    if(index >= countCastle) return NULL;

    return &ptrCastle[index];
}

ACTION EnterCastle(Uint8 ax, Uint8 ay, E_NAMEHEROES castleHeroes){

    // определяем тип замка
    SDL_Surface *format, *back, *image, *video;
    SDL_Rect rectBack, rectCur;
    AGGSPRITE sprite;
    ACTION result;
    ICNHEADER *header = NULL;
    S_CASTLE *castle = GetStatCastlePos(ax, ay);
    S_HEROES *heroes = GetStatHeroes(castleHeroes);
    flagUpdateBuilding = FALSE;

    // в серые замки не заходим
    if(GRAY == castle->color) return NONE;
    currentCastle = castle;
    heroesName = castleHeroes;
    // инициализируем backgroundCursor
    FillSPRITE(&sprite, "STRIP.ICN", 1);
    backMonsterCursor.cursor = GetICNSprite(&sprite);
    backMonsterCursor.select = 0xFF;
    backMonsterCursor.use = FALSE;
    backMonsterCursor.rect.x = 0;
    backMonsterCursor.rect.y = 0;
    backMonsterCursor.rect.w = backMonsterCursor.cursor->w;
    backMonsterCursor.rect.h = backMonsterCursor.cursor->h;
    if(NULL == (backMonsterCursor.back = SDL_CreateRGBSurface(SDL_SWSURFACE, backMonsterCursor.rect.w, backMonsterCursor.rect.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "EnterCastle: CreateRGBSurface failed: %s\n", SDL_GetError());
        return EXIT;
    }

    // переопределяем курсор и выключаем анимацию карты
    SetIntValue(ANIM1, FALSE);
    CursorOff();
    Uint32 cursor = GetCursor();
    INTERFACEACTION action;

    char message[8];
    Uint16 cx, cy;
    Uint8 i;

    // рисуем бакгроунд 640х480
    // если videomode более то рисуем бордюр + тень и по центру экрана
    video = SDL_GetVideoSurface();
    if(GetIntValue(VIDEOMODE)){
	rectBack.x = video->w / 2 - 320 - BORDERWIDTH - SHADOWWIDTH;
	rectBack.y = video->h / 2 - 240 - BORDERWIDTH;
	rectBack.w = 640 + 2 * BORDERWIDTH + SHADOWWIDTH;
	rectBack.h = 480 + 2 * BORDERWIDTH + SHADOWWIDTH;
    }else{
	rectBack.x = 0;
	rectBack.y = 0;
	rectBack.w = 640;
	rectBack.h = 480;
    }
    if(NULL == (format = SDL_CreateRGBSurface(SDL_SWSURFACE, rectBack.w, rectBack.h, 16, 0, 0, 0, 0))){
        fprintf(stderr, "EnterCastle: CreateRGBSurface failed: %s\n", SDL_GetError());
        return EXIT;
    }

    // сохраняем бакгроунд
    back = SDL_DisplayFormat(format);
    SDL_FreeSurface(format);
    SDL_BlitSurface(video, &rectBack, back, NULL);

    // рисуем бордюр
    if(GetIntValue(VIDEOMODE)) ShowBorder(&rectBack);

    switch(castle->race){

	case KNIGHT:
	    FillSPRITE(&sprite, "TOWNBKG0.ICN", 0);
	    break;

	case BARBARIAN:
	    FillSPRITE(&sprite, "TOWNBKG1.ICN", 0);
	    break;

	case SORCERESS:
	    FillSPRITE(&sprite, "TOWNBKG2.ICN", 0);
	    break;

	case WARLOCK:
	    FillSPRITE(&sprite, "TOWNBKG3.ICN", 0);
	    break;

	case WIZARD:
	    FillSPRITE(&sprite, "TOWNBKG4.ICN", 0);
	    break;

	case NECROMANCER:
	    FillSPRITE(&sprite, "TOWNBKG5.ICN", 0);
	    break;
	
	default:
	    break;
    }
    image = GetICNSprite(&sprite);
    rectCur.x = 0;
    rectCur.y = 0;
    rectCur.w = image->w;
    rectCur.h = image->h;
    if(GetIntValue(VIDEOMODE)){
	rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH;
	rectCur.y = rectBack.y + BORDERWIDTH;
    }
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // рисуем сетку
    FillSPRITE(&sprite, "STRIP.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    //SDL_BlitSurface(image, NULL, video, &rectCur);
    cx = rectCur.x;
    cy = rectCur.y;

    // левый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.y += rectCur.h;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SMALLBAR.ICN", 1);
    FillSPRITE(&action.objectPush, "SMALLBAR.ICN", 2);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionLeftCastle;
    AddActionEvent(&castlact, &action);

    // нижний бар
    FillSPRITE(&sprite, "SMALLBAR.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x += rectCur.w;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // правый скролинг замка
    FillSPRITE(&sprite, "SMALLBAR.ICN", 3);
    image = GetICNSprite(&sprite);
    rectCur.x += rectCur.w;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SMALLBAR.ICN", 3);
    FillSPRITE(&action.objectPush, "SMALLBAR.ICN", 4);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionRightCastle;
    AddActionEvent(&castlact, &action);

    // рисуем цветовой знак
    switch(castle->color){
    
	case BLUE:
	    FillSPRITE(&sprite, "CREST.ICN", 0);
	    break;

	case GREEN:
	    FillSPRITE(&sprite, "CREST.ICN", 1);
	    break;

	case RED:
	    FillSPRITE(&sprite, "CREST.ICN", 2);
	    break;

	case YELLOW:
	    FillSPRITE(&sprite, "CREST.ICN", 3);
	    break;

	case ORANGE:
	    FillSPRITE(&sprite, "CREST.ICN", 4);
	    break;

	case PURPLE:
	    FillSPRITE(&sprite, "CREST.ICN", 5);
	    break;

	default:
	    break;
    }
    image = GetICNSprite(&sprite);
    if(GetIntValue(VIDEOMODE)){ rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH + 5; }else{ rectCur.x = rectBack.x + 5; }
    rectCur.y = cy + 6;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // рисуем фон ячеек для монстров
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    image = GetICNSprite(&sprite);
    cx = rectCur.x + rectCur.w + 6;
    cy = rectCur.y;
    for(i = 0; i < CASTLEMAXARMY; ++i){

	// если есть в замке монстры
	if(castle->army[i].count){
		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(castle->army[i].monster)){
		
		    case KNIGHT:
			FillSPRITE(&sprite, "STRIP.ICN", 4);
			break;

		    case BARBARIAN:
			FillSPRITE(&sprite, "STRIP.ICN", 5);
			break;

		    case SORCERESS:
			FillSPRITE(&sprite, "STRIP.ICN", 6);
			break;

		    case WARLOCK:
			FillSPRITE(&sprite, "STRIP.ICN", 7);
			break;

		    case WIZARD:
			FillSPRITE(&sprite, "STRIP.ICN", 8);
			break;

		    case NECROMANCER:
			FillSPRITE(&sprite, "STRIP.ICN", 9);
			break;

		    default:
			FillSPRITE(&sprite, "STRIP.ICN", 10);
			break;
		}
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);

		// регистрируем событие на click
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickCastleMonster;
		AddActionEvent(&castlact, &action);
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_RCLICK;
		action.pf = ActionClickRedistributeMonster;
		AddActionEvent(&castlact, &action);

		// рисуем монстров
		FillSPRITE(&sprite, MonsterBigNamePortrait(castle->army[i].monster), 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
		sprintf(message, "%5d", castle->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
	}else{
	    FillSPRITE(&sprite, "STRIP.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + (image->w + 6) * i;
	    rectCur.y = cy;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);

	    // регистрируем событие на click
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_LCLICK;
	    action.pf = ActionClickCastleMonster;
	    AddActionEvent(&castlact, &action);
	    ZeroINTERFACEACTION(&action);
	    action.rect = rectCur;
	    action.mouseEvent = MOUSE_RCLICK;
	    action.pf = ActionClickRedistributeMonster;
	    AddActionEvent(&castlact, &action);
	}
    }

    // рисуем фон ячейки героя
    if(heroes){
	FillSPRITE(&sprite, HeroesBigNamePortrait(castleHeroes), 0);
	// наведение мыши
	if(GetIntValue(VIDEOMODE)){ rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH + 5; }else{ rectCur.x = rectBack.x + 5; }
	rectCur.y = cy + image->w - 2;
	rectCur.w = image->w;
	rectCur.h = image->h;
	ZeroINTERFACEACTION(&action);
	action.rect = rectCur;
	action.mouseEvent = MOUSE_PRESENT;
	action.pf = ActionOverHeroes;
	AddActionEvent(&castlact, &action);
	// клик
	ZeroINTERFACEACTION(&action);
        action.rect = rectCur;
	action.mouseEvent = MOUSE_LCLICK;
	action.pf = ActionClickHeroes;
	AddActionEvent(&castlact, &action);

    // рисуем капитана
    }else if(castle->building & BUILD_CAPTAIN)
	FillSPRITE(&sprite, CapitanBigNamePortrait(castle->race), 0);
    else
	FillSPRITE(&sprite, "STRIP.ICN", 3);

    image = GetICNSprite(&sprite);
    if(GetIntValue(VIDEOMODE)){ rectCur.x = rectBack.x + BORDERWIDTH + SHADOWWIDTH + 5; }else{ rectCur.x = rectBack.x + 5; }
    rectCur.y = cy + image->w - 2;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // рисуем фон ячеек для монстров героя
    if(heroes){

	cx = rectCur.x + rectCur.w + 6;
	cy = rectCur.y;

	for( i = 0; i < HEROESMAXARMY; ++i)
	    // если есть у героя монстры
	    if(heroes->army[i].count){

		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(heroes->army[i].monster)){
		
		    case KNIGHT:
			FillSPRITE(&sprite, "STRIP.ICN", 4);
			break;

		    case BARBARIAN:
			FillSPRITE(&sprite, "STRIP.ICN", 5);
			break;

		    case SORCERESS:
			FillSPRITE(&sprite, "STRIP.ICN", 6);
			break;

		    case WARLOCK:
			FillSPRITE(&sprite, "STRIP.ICN", 7);
			break;

		    case WIZARD:
			FillSPRITE(&sprite, "STRIP.ICN", 8);
			break;

		    case NECROMANCER:
			FillSPRITE(&sprite, "STRIP.ICN", 9);
			break;

		    default:
			FillSPRITE(&sprite, "STRIP.ICN", 10);
			break;
		}
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);

		// регистрируем событие на click
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickHeroesMonster;
		AddActionEvent(&castlact, &action);
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_RCLICK;
		action.pf = ActionClickRedistributeMonster;
		AddActionEvent(&castlact, &action);

		// рисуем монстров
		FillSPRITE(&sprite, MonsterBigNamePortrait(heroes->army[i].monster), 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
		sprintf(message, "%5d", heroes->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
	    }else{
		FillSPRITE(&sprite, "STRIP.ICN", 2);
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);

		// регистрируем событие на click
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_LCLICK;
		action.pf = ActionClickHeroesMonster;
		AddActionEvent(&castlact, &action);
		ZeroINTERFACEACTION(&action);
		action.rect = rectCur;
		action.mouseEvent = MOUSE_RCLICK;
		action.pf = ActionClickRedistributeMonster;
		AddActionEvent(&castlact, &action);
	    }
	cy -= 99;
    }else{
	FillSPRITE(&sprite, "STRIP.ICN", 11);
	image = GetICNSprite(&sprite);
	rectCur.x += rectCur.w + 6;
	rectCur.w = image->w;
	rectCur.h = image->h;
	SDL_BlitSurface(image, NULL, video, &rectCur);
    }

    RedrawCastleInfoResource();
    // кнопка exit
    FillSPRITE(&sprite, "SWAPBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx +  440 + 1;
    rectCur.y = cy + 166;
    rectCur.w = image->w;
    rectCur.h = image->h;
    //SDL_BlitSurface(image, NULL, video, &rectCur);
    // наведение мыши
    ZeroINTERFACEACTION(&action);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_PRESENT;
    action.pf = ActionOverExit;
    AddActionEvent(&castlact, &action);
    // клик
    ZeroINTERFACEACTION(&action);
    FillSPRITE(&action.objectUp, "SWAPBTN.ICN", 0);
    FillSPRITE(&action.objectPush, "SWAPBTN.ICN", 1);
    action.rect = rectCur;
    action.mouseEvent = MOUSE_LCLICK;
    action.pf = ActionExitCastle;
    AddActionEvent(&castlact, &action);

    switch(castle->race){

	case KNIGHT:
	    DrawKNGTCastle(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawKNGTCapitan(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawKNGTWel2(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawKNGTLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawKNGTRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawKNGTMoat(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawKNGTMarketplace(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawKNGTDwelling2(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawKNGTThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawKNGTTavern(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawKNGTMageGuild(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawKNGTDwelling5(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawKNGTDwelling6(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawKNGTDwelling1(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawKNGTDwelling3(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawKNGTDwelling4(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawKNGTWell(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawKNGTStatue(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawKNGTShipyard(&castanim, &castlact);
	    else DrawKNGTExt0(&castanim, &castlact);
	    //DrawKNGTExt1(&castanim, &castlact); // развилка дорог?
	    //DrawKNGTExt2(&castanim, &castlact); // развилка дорог?
	    break;

	case BARBARIAN:
	    if(castle->building & BUILD_SPEC) DrawBRBNSpec(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawBRBNWel2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawBRBNDwelling6(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)){
		DrawBRBNMageGuild(&castanim, &castlact);
		DrawBRBNExt2(&castanim, &castlact);
	    }
	    if(castle->building & BUILD_CAPTAIN) DrawBRBNCapitan(&castanim, &castlact);
	    DrawBRBNCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawBRBNLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBRBNRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawBRBNMoat(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawBRBNDwelling3(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawBRBNThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawBRBNTavern(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawBRBNDwelling1(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawBRBNMarketplace(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawBRBNDwelling2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawBRBNDwelling4(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawBRBNDwelling5(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawBRBNWell(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawBRBNStatue(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawBRBNShipyard(&castanim, &castlact);
	    else DrawBRBNExt0(&castanim, &castlact);
	    DrawBRBNExt1(&castanim, &castlact);
	    //DrawBRBNExt3(&castanim, &castlact); // развилка дорог?
	    break;

	case SORCERESS:
	    if(castle->building & BUILD_SPEC) DrawSCRSSpec(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawSCRSDwelling6(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawSCRSMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawSCRSCapitan(&castanim, &castlact);
	    DrawSCRSCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawSCRSLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawSCRSRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawSCRSMoat(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawSCRSDwelling3(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawSCRSShipyard(&castanim, &castlact);
	    else DrawSCRSExt0(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawSCRSMarketplace(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawSCRSDwelling2(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawSCRSThievesGuild(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawSCRSDwelling1(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawSCRSTavern(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE && castle->building & BUILD_WEL2) DrawSCRSExt1(&castanim, &castlact);
	    else if(castle->building & BUILD_STATUE) DrawSCRSStatue(&castanim, &castlact);
	    else if(castle->building & BUILD_WEL2) DrawSCRSWel2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawSCRSDwelling4(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawSCRSWell(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawSCRSDwelling5(&castanim, &castlact);
	break;

	case NECROMANCER:
	    if(castle->building & BUILD_SPEC) DrawNCRMSpec(&castanim, &castlact);
	    DrawNCRMCastle(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawNCRMCapitan(&castanim, &castlact);
    	    if(castle->building & BUILD_LEFTTURRET) DrawNCRMLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawNCRMRTurret(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawNCRMDwelling6(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawNCRMMoat(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawNCRMShipyard(&castanim, &castlact);
	    else DrawNCRMExt0(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawNCRMThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawNCRMTavern(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawNCRMDwelling3(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawNCRMDwelling5(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawNCRMDwelling2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawNCRMDwelling4(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawNCRMDwelling1(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawNCRMMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawNCRMWel2(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawNCRMMarketplace(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawNCRMStatue(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawNCRMWell(&castanim, &castlact);
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_MONSTER5) DrawWRLKDwelling5(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawWRLKDwelling3(&castanim, &castlact);
	    DrawWRLKCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawWRLKLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWRLKRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawWRLKCapitan(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawWRLKMoat(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawWRLKShipyard(&castanim, &castlact);
	    else DrawWRLKExt0(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawWRLKMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawWRLKTavern(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawWRLKThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawWRLKMarketplace(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawWRLKStatue(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWRLKDwelling1(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawWRLKWel2(&castanim, &castlact);
	    if(castle->building & BUILD_SPEC) DrawWRLKSpec(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawWRLKDwelling4(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawWRLKDwelling2(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawWRLKDwelling6(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawWRLKWell(&castanim, &castlact);
	break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_MONSTER6) DrawWZRDDwelling6(&castanim, &castlact);
	    DrawWZRDCastle(&castanim, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawWZRDLTurret(&castanim, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWZRDRTurret(&castanim, &castlact);
	    if(castle->building & BUILD_MOAT) DrawWZRDMoat(&castanim, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawWZRDCapitan(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawWZRDDwelling2(&castanim, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawWZRDThievesGuild(&castanim, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawWZRDTavern(&castanim, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawWZRDShipyard(&castanim, &castlact);
	    else DrawWZRDExt0(&castanim, &castlact);
	    if(castle->building & BUILD_WELL) DrawWZRDWell(&castanim, &castlact);
	    if(castle->building & BUILD_SPEC) DrawWZRDSpec(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawWZRDDwelling3(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawWZRDDwelling5(&castanim, &castlact);
	    if(GetMageGuildLevel(castle)) DrawWZRDMageGuild(&castanim, &castlact);
	    if(castle->building & BUILD_STATUE) DrawWZRDStatue(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWZRDDwelling1(&castanim, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawWZRDDwelling4(&castanim, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawWZRDMarketplace(&castanim, &castlact);
	    if(castle->building & BUILD_WEL2) DrawWZRDWel2(&castanim, &castlact);
	break;
	
	default:
	return NONE;
	break;
    }

    // наименование замка
    FillSPRITE(&sprite, "TOWNNAME.ICN", 0);
    image = GetICNSprite(&sprite);
    if(GetIntValue(VIDEOMODE)){
	rectCur.x = rectBack.x + 264;
	rectCur.y = rectBack.y + 264;
    }else{
	rectCur.x = rectBack.x + 264 - BORDERWIDTH * 3;
	rectCur.y = rectBack.y + 264 - BORDERWIDTH;
    }
    rectCur.w = image->w;
    rectCur.h = image->h;
    //SDL_BlitSurface(image, NULL, video, &rectCur);
    rectCur.x = rectCur.x + 90 - GetLengthText(castle->name, FONT_SMALL) / 2 ;
    rectCur.y = rectCur.y + 1;
    rectCur.w = GetLengthText(castle->name, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    //PrintText(video, &rectCur, castle->name, FONT_SMALL);
    RedrawRamkaCastleName();
                    
    SetCursor(CURSOR_POINTER);
    CursorOn();

    result = ActionCASTLELOOP(castlact);

    CursorOff();
    // чистим данные
    FreeAnimationEvent(castanim);
    FreeActionEvent(castlact);
    if(backMonsterCursor.back) SDL_FreeSurface(backMonsterCursor.back);

    // востанавливаем бакгроунд
    SDL_BlitSurface(back, NULL, video, &rectBack);
    SDL_FreeSurface(back);

    castlact = NULL;
    castanim = NULL;
    currentCastle = NULL;

    // востанавливаем курсор и анимацию карты
    SetCursor(cursor);
    CursorOn();

    SetIntValue(ANIM1, TRUE);
    
    return result;
}

ACTION ActionExitCastle(void){

    return CANCEL;
}

ACTION ActionLeftCastle(void){

    return NONE;
}

ACTION ActionRightCastle(void){

    return NONE;
}

ACTION ActionCASTLELOOP(INTERFACEACTION *action){

    SDL_Event event;
    SDL_Surface *video = SDL_GetVideoSurface();;
    ACTION exit = NONE;
    S_OLDOBJECT old;
    memset(old.object.name, 0, AGGSIZENAME);
    old.object.number = 0xFFFF;
    old.pushRect.x = 0;
    old.pushRect.y = 0;
    old.pushRect.w = 0;
    old.pushRect.h = 0;
    old.presRect = old.pushRect;
    old.flagPush = FALSE;
    old.flagPres = FALSE;
    INTERFACEACTION *ptr = NULL;

    Uint32	ticket = 0;
    Sint32 	cx, cy;

    // цикл по событиям
    while(exit == NONE){

	ptr = action;

	SDL_GetMouseState(&cx, &cy);

	if(flagUpdateBuilding)  UpdateCastleBuilding();

        if(old.flagPres && !ValidPoint(&old.presRect, cx, cy)){
            RedrawBottomBar();
            old.flagPres = FALSE;
        }else
            while(ptr){
                if((ptr->mouseEvent & MOUSE_PRESENT) && ValidPoint(&ptr->rect, cx, cy) && ptr->pf && !old.flagPres){
                    old.presRect = ptr->rect;
                    old.flagPres = TRUE;
		    exit = (*ptr->pf)();
            	}
        	ptr = (INTERFACEACTION *) ptr->next;
	}

	while(SDL_PollEvent(&event)){

	    switch(event.type){
	    
		case SDL_QUIT:

		    // close windows
		    exit = EXIT;
		    break;

		case SDL_KEYDOWN:

		    // нажатия клавиатуры
		    switch(event.key.keysym.sym){

			case SDLK_ESCAPE:

			    exit = ESC;
			    break;

			// F4 switch to full screen
                	case SDLK_F4:

			    SDL_WM_ToggleFullScreen(video);

			    GetIntValue(FULLSCREEN) ? SetIntValue(FULLSCREEN, FALSE) : SetIntValue(FULLSCREEN, TRUE);

			    break;
								    
			default:
			    break;
		    }
		    break;

		case SDL_MOUSEBUTTONDOWN:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка down
			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK)){
				    old.pushRect = ptr->rect;
				    old.object = ptr->objectUp;
				    old.flagPush = TRUE;
				    DrawSprite(&ptr->rect, &ptr->objectPush);
				}
    				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;

			case SDL_BUTTON_RIGHT:
			    
			    if(GetIntValue(DEBUG)) fprintf(stderr, "x: %d, y: %d\n", event.button.x, event.button.y);
			    break;

			default:
			    break;
		    }
		    break;

		case SDL_MOUSEBUTTONUP:

		    switch(event.button.button){
		    
			case SDL_BUTTON_LEFT:

			    // левая кнопка up
			    ptr = action;
			    if(old.flagPush){
				DrawSprite(&old.pushRect, &old.object);
				old.flagPush = FALSE;
			    }

			    while(ptr){
				if(ValidPoint(&old.pushRect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_LCLICK) && CompareRect(&ptr->rect, &old.pushRect) && ptr->pf )
					exit = (*ptr->pf)();

				ptr = (INTERFACEACTION *) ptr->next;
			    }
			    break;

			case SDL_BUTTON_RIGHT:
			    ptr = action;
			    while(ptr){
				if(ValidPoint(&ptr->rect, event.button.x, event.button.y) &&
				    (ptr->mouseEvent & MOUSE_RCLICK) && ptr->pf )
					exit = (*ptr->pf)();

				ptr = (INTERFACEACTION *) ptr->next;
			    }

			default:
			    break;
		    }
		    break;

		case SDL_MOUSEMOTION:

		    CursorShow();
		    break;

		default:
    		    break;
	    }

            if(exit != EXIT && 0 == ticket % (GetIntValue(ANIMATIONDELAY) / 2)) RedrawCastleAnimation();
            ++ticket;

	}

	if(0 == ticket % GetIntValue(ANIMATIONDELAY)) RedrawCastleAnimation();
	else if(GetIntValue(CYCLELOOP)) SDL_Delay(CYCLEDELAY);

	++ticket;
    }

    return exit;
}

void RedrawCastleAnimation(void){

    if(! GetIntValue(ANIMATION)) return;
    if(! GetIntValue(ANIM3)) return;

    static Uint32 animationFrame = 0;
 
    Sint32 x; 
    Sint32 y; 
     
    SDL_Surface *video = SDL_GetVideoSurface(); 
    SDL_GetMouseState(&x, &y); 
    S_ANIMATION *ptr = castanim; 

    CursorOff();
 
    while(ptr){ 
 
	// востановить фон предыдущего спрайта
	if(ptr->background){
    	    SDL_BlitSurface(ptr->background, NULL, video, &ptr->rectBack);
	    SDL_FreeSurface(ptr->background);
	    ptr->background = NULL;
	}
	
	if(NULL == (ptr->background = SDL_CreateRGBSurface(SDL_SWSURFACE, ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h, 16, 0, 0, 0, 0))){
            fprintf(stderr, "RedrawMenuAnimation: CreateRGBSurface failed: %s, %d, %d\n", SDL_GetError(), ptr->rect[animationFrame % ptr->count].w, ptr->rect[animationFrame % ptr->count].h);
            return;
        }

	// сохраняем фон нового спрайта
	ptr->rectBack = ptr->rect[animationFrame % ptr->count];
    	SDL_BlitSurface(video, &ptr->rectBack, ptr->background, NULL);

	// рисуем спрайт
        SDL_BlitSurface(ptr->surface[animationFrame % ptr->count], NULL, video, &ptr->rectBack);

        ptr = ptr->next; 
    } 
 
    // в зависимости от замка дорисовываем постройки
    if(currentCastle)
	switch(currentCastle->race){

	    case WARLOCK:
		if(currentCastle->building & BUILD_MARKETPLACE) DrawWRLKMarketplace(NULL, NULL);
		if(currentCastle->building & BUILD_STATUE) DrawWRLKStatue(NULL, NULL);
        	if(currentCastle->dwelling & DWELLING_MONSTER6) DrawWRLKDwelling6(NULL, NULL);
            	break;

	    case BARBARIAN:
		if(currentCastle->building & BUILD_THIEVESGUILD) DrawBRBNThievesGuild(NULL, NULL);
		if(currentCastle->dwelling & DWELLING_MONSTER4) DrawBRBNDwelling4(NULL, NULL);
		if(currentCastle->building & BUILD_STATUE) DrawBRBNStatue(NULL, NULL);
		if(currentCastle->building & BUILD_TAVERN) DrawBRBNTavern(NULL, NULL);
		if(currentCastle->dwelling & DWELLING_MONSTER5) DrawBRBNDwelling5(NULL, NULL);
		if(currentCastle->building & BUILD_WELL) DrawBRBNWell(NULL, NULL);
		// необходимо перерисовать имя замка
		RedrawRamkaCastleName();
		break;

	    default:
		break;
	}
                            
    CursorOn(); 
    //SDL_Flip(video); 
    SDL_Delay(GetIntValue(ANIMATIONDELAY));

    ++animationFrame;

    return;
}

void RedrawBottomBar(void){

    AGGSPRITE sprite;
    SDL_Surface *image = NULL;
    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Rect rect;

    // нижний бар
    FillSPRITE(&sprite, "SMALLBAR.ICN", 0);
    image = GetICNSprite(&sprite);
    rect.x = video->w / 2 - 320 + BORDERWIDTH + 5;
    rect.y = video->h / 2 + 240 - BORDERWIDTH - 3;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rect);
    SDL_Flip(video);
}

ACTION ActionClickRedistributeMonster(void){
/*
    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    Uint16 cy1 = video->h / 2 + 22;
    Uint16 cy2 = video->h / 2 + 121;

    BOOL fromCastle = FALSE;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint16) (mx - cx) / 88;

    if(my > cy2) fromCastle = TRUE;
    
    S_HEROES *heroes = GetStatHeroes(heroesName);

    if(! heroes || (fromCastle && MONSTERNONE != currentCastle->army[index].monster) || (!fromCastle && MONSTERNONE != heroes->army[index].monster)) return NONE;
*/
    return NONE;
}

ACTION ActionClickCastleMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();

    CursorOff();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    Uint16 cy = video->h / 2 + 22;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint16) (mx - cx) / 88;
    Uint16 count = 0;
    E_MONSTER monster = MONSTERNONE;
    S_HEROES *heroes = NULL;

    if(MONSTERNONE == currentCastle->army[index].monster) return ActionClickCastleMonsterEmpty(index);

    // двойной клик - инфо монстра
    if(backMonsterCursor.select == index && backMonsterCursor.use){
	SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	switch(ShowArmyInfo(&currentCastle->army[index], NULL)){
	    case DISMISS:
		CursorOff();
		currentCastle->army[index].monster = MONSTERNONE;
		currentCastle->army[index].count = 0;
		RedrawCastleMonster();
		CursorOn();
		break;

	    case UPGRADE:
		CursorOff();
		currentCastle->army[index].monster = UpgradeMonster(currentCastle->army[index].monster);
		RedrawCastleMonster();
		CursorOn();
		break;
	
	    case EXIT:
		return EXIT;
		break;

	    default:
		break;
	}
	CursorOff();
	backMonsterCursor.use = FALSE;
    // обмен
    }else if(backMonsterCursor.use){
	SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	backMonsterCursor.use = FALSE;
	monster = currentCastle->army[index].monster;
	count = currentCastle->army[index].count;
	// в замке
	if(backMonsterCursor.castle){
	    // одинаковых объединяем
	    if(currentCastle->army[index].monster == currentCastle->army[backMonsterCursor.select].monster){
		currentCastle->army[index].count += currentCastle->army[backMonsterCursor.select].count;
		currentCastle->army[backMonsterCursor.select].monster = MONSTERNONE;
		currentCastle->army[backMonsterCursor.select].count = 0;
	    }else{
		currentCastle->army[index].monster = currentCastle->army[backMonsterCursor.select].monster;
		currentCastle->army[index].count = currentCastle->army[backMonsterCursor.select].count;
		currentCastle->army[backMonsterCursor.select].monster = monster;
		currentCastle->army[backMonsterCursor.select].count = count;
	    }
	    RedrawCastleMonster();
	// с героем
	}else if(HEROESNULL != heroesName){
	    heroes = GetStatHeroes(heroesName);
	    // одинаковых объединяем
	    if(currentCastle->army[index].monster == heroes->army[backMonsterCursor.select].monster){
		currentCastle->army[index].count += heroes->army[backMonsterCursor.select].count;
		heroes->army[backMonsterCursor.select].monster = MONSTERNONE;
		heroes->army[backMonsterCursor.select].count = 0;
	    }else{
		currentCastle->army[index].monster = heroes->army[backMonsterCursor.select].monster;
		currentCastle->army[index].count = heroes->army[backMonsterCursor.select].count;
		heroes->army[backMonsterCursor.select].monster = monster;
		heroes->army[backMonsterCursor.select].count = count;
	    }
	    RedrawCastleMonster();
	    RedrawHeroesMonster(heroesName);
	}
    // первый клик рисуем рамку
    }else{
	// востанавливаем background
	if(backMonsterCursor.use){
	    SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	    backMonsterCursor.use = FALSE;
	}
	backMonsterCursor.rect.x = cx + index * (backMonsterCursor.rect.w + 6);
	backMonsterCursor.rect.y = cy;
	backMonsterCursor.select = index;
	backMonsterCursor.castle = TRUE;
	// сохраняем background
	SDL_BlitSurface(video, &backMonsterCursor.rect, backMonsterCursor.back, NULL);
	backMonsterCursor.use = TRUE;
        // рисуем рамку
	SDL_BlitSurface(backMonsterCursor.cursor, NULL, video, &backMonsterCursor.rect);
    }

    CursorOn();

    return NONE;
}

ACTION ActionClickCastleMonsterEmpty(Uint8 index){

    if(! backMonsterCursor.use) return NONE;

    CursorOff();
    SDL_Surface *video = SDL_GetVideoSurface();

    // востанавливаем background
    backMonsterCursor.use = FALSE;
    S_HEROES *heroes = GetStatHeroes(heroesName);

    if(! backMonsterCursor.castle && 2 > HeroesCountArmy(heroes)){
	SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	CursorOn();
	return NONE;
    }

    // перемещаем на пустую клетку
    if(backMonsterCursor.castle){
	currentCastle->army[index].monster = currentCastle->army[backMonsterCursor.select].monster;
	currentCastle->army[index].count = currentCastle->army[backMonsterCursor.select].count;
	currentCastle->army[backMonsterCursor.select].monster = MONSTERNONE;
	currentCastle->army[backMonsterCursor.select].count = 0;
    }else{
	currentCastle->army[index].monster = heroes->army[backMonsterCursor.select].monster;
	currentCastle->army[index].count = heroes->army[backMonsterCursor.select].count;
	heroes->army[backMonsterCursor.select].monster = MONSTERNONE;
	heroes->army[backMonsterCursor.select].count = 0;
    }

    RedrawCastleMonster();
    RedrawHeroesMonster(heroesName);

    CursorOn();

    return NONE;
}

ACTION ActionClickHeroesMonster(void){

    Sint32 mx, my;
    SDL_Surface *video = SDL_GetVideoSurface();

    CursorOff();

    // верхний левый угол начала
    Uint16 cx = video->w / 2 - 208;
    Uint16 cy = video->h / 2 + 121;

    SDL_GetMouseState(&mx, &my);

    Uint8 index = (Uint16) (mx - cx) / 88;
    Uint16 count = 0;
    E_MONSTER monster = MONSTERNONE;
    S_HEROES *heroes = GetStatHeroes(heroesName);
    if(! heroes) return NONE;

    if(MONSTERNONE == heroes->army[index].monster) return ActionClickHeroesMonsterEmpty(index);

    // двойной клик - инфо монстра
    if(backMonsterCursor.select == index && backMonsterCursor.use){
	SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	switch(ShowArmyInfo(&heroes->army[index], heroes)){
	    case DISMISS:
		CursorOff();
		heroes->army[index].monster = MONSTERNONE;
		heroes->army[index].count = 0;
		RedrawHeroesMonster(heroesName);
		CursorOn();
		break;

	    case UPGRADE:
		CursorOff();
		heroes->army[index].monster = UpgradeMonster(heroes->army[index].monster);
		RedrawHeroesMonster(heroesName);
		CursorOn();
		break;

	    case EXIT:
		return EXIT;
		break;

	    default:
		break;
	}
	backMonsterCursor.use = FALSE;
	CursorOff();
    // обмен
    }else if(backMonsterCursor.use){
	SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	backMonsterCursor.use = FALSE;
	monster = heroes->army[index].monster;
	count = heroes->army[index].count;
	// с замком
	if(backMonsterCursor.castle){
	    // одинаковых объединяем
	    if(heroes->army[index].monster == currentCastle->army[backMonsterCursor.select].monster){
		heroes->army[index].count += currentCastle->army[backMonsterCursor.select].count;
		currentCastle->army[backMonsterCursor.select].monster = MONSTERNONE;
		currentCastle->army[backMonsterCursor.select].count = 0;
	    }else{
		heroes->army[index].monster = currentCastle->army[backMonsterCursor.select].monster;
		heroes->army[index].count = currentCastle->army[backMonsterCursor.select].count;
		currentCastle->army[backMonsterCursor.select].monster = monster;
		currentCastle->army[backMonsterCursor.select].count = count;
	    }
	    RedrawHeroesMonster(heroesName);
	    RedrawCastleMonster();
	// в герое
	}else{
	    // одинаковых объединяем
	    if(heroes->army[index].monster == heroes->army[backMonsterCursor.select].monster){
		heroes->army[index].count += heroes->army[backMonsterCursor.select].count;
		heroes->army[backMonsterCursor.select].monster = MONSTERNONE;
		heroes->army[backMonsterCursor.select].count = 0;
	    }else{
		heroes->army[index].monster = heroes->army[backMonsterCursor.select].monster;
		heroes->army[index].count = heroes->army[backMonsterCursor.select].count;
		heroes->army[backMonsterCursor.select].monster = monster;
		heroes->army[backMonsterCursor.select].count = count;
	    }
	    RedrawHeroesMonster(heroesName);
	}
    // первый клик рисуем рамку
    }else{
	// востанавливаем background
	if(backMonsterCursor.use){
	    SDL_BlitSurface(backMonsterCursor.back, NULL, video, &backMonsterCursor.rect);
	    backMonsterCursor.use = FALSE;
	}
	backMonsterCursor.rect.x = cx + index * (backMonsterCursor.rect.w + 6);
	backMonsterCursor.rect.y = cy;
	backMonsterCursor.select = index;
	backMonsterCursor.castle = FALSE;
	// сохраняем background
	SDL_BlitSurface(video, &backMonsterCursor.rect, backMonsterCursor.back, NULL);
	backMonsterCursor.use = TRUE;
        // рисуем рамку
	SDL_BlitSurface(backMonsterCursor.cursor, NULL, video, &backMonsterCursor.rect);
    }

    CursorOn();

    return NONE;
}

ACTION ActionClickHeroesMonsterEmpty(Uint8 index){

    if(! backMonsterCursor.use) return NONE;

    CursorOff();

    S_HEROES *heroes = GetStatHeroes(heroesName);
    backMonsterCursor.use = FALSE;

    // перемещаем на пустую клетку
    if(backMonsterCursor.castle){
	heroes->army[index].monster = currentCastle->army[backMonsterCursor.select].monster;
	heroes->army[index].count = currentCastle->army[backMonsterCursor.select].count;
	currentCastle->army[backMonsterCursor.select].monster = MONSTERNONE;
	currentCastle->army[backMonsterCursor.select].count = 0;
    }else{
	heroes->army[index].monster = heroes->army[backMonsterCursor.select].monster;
	heroes->army[index].count = heroes->army[backMonsterCursor.select].count;
	heroes->army[backMonsterCursor.select].monster = MONSTERNONE;
	heroes->army[backMonsterCursor.select].count = 0;
    }

    RedrawCastleMonster();
    RedrawHeroesMonster(heroesName);

    CursorOn();

    return NONE;
}

ACTION ActionClickHeroes(void){

    ShowHeroesInfo(SANDYSANDY);
    return NONE;
}

void RedrawCastleMonster(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    AGGSPRITE sprite;
    SDL_Rect rectCur;
    Uint16 cx, cy;
    Uint8 i;
    ICNHEADER *header = NULL;

    char message[8];

    // рисуем фон ячеек для монстров
    FillSPRITE(&sprite, "STRIP.ICN", 2);
    image = GetICNSprite(&sprite);
    cx = video->w / 2 - 208;
    cy = video->h / 2 + 22;
    for(i = 0; i < CASTLEMAXARMY; ++i){

	// если есть в замке монстры
	if(currentCastle->army[i].count){
		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(currentCastle->army[i].monster)){
		
		    case KNIGHT:
			FillSPRITE(&sprite, "STRIP.ICN", 4);
			break;

		    case BARBARIAN:
			FillSPRITE(&sprite, "STRIP.ICN", 5);
			break;

		    case SORCERESS:
			FillSPRITE(&sprite, "STRIP.ICN", 6);
			break;

		    case WARLOCK:
			FillSPRITE(&sprite, "STRIP.ICN", 7);
			break;

		    case WIZARD:
			FillSPRITE(&sprite, "STRIP.ICN", 8);
			break;

		    case NECROMANCER:
			FillSPRITE(&sprite, "STRIP.ICN", 9);
			break;

		    default:
			FillSPRITE(&sprite, "STRIP.ICN", 10);
			break;
		}
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);

		// рисуем монстров
		FillSPRITE(&sprite, MonsterBigNamePortrait(currentCastle->army[i].monster), 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
		sprintf(message, "%5d", currentCastle->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);
	}else{
	    FillSPRITE(&sprite, "STRIP.ICN", 2);
	    image = GetICNSprite(&sprite);
	    rectCur.x = cx + (image->w + 6) * i;
	    rectCur.y = cy;
	    rectCur.w = image->w;
	    rectCur.h = image->h;
	    SDL_BlitSurface(image, NULL, video, &rectCur);
	}
    }
}

void RedrawHeroesMonster(E_NAMEHEROES name){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    AGGSPRITE sprite;
    SDL_Rect rectCur;
    Uint16 cx, cy;
    Uint8 i;
    ICNHEADER *header = NULL;
    S_HEROES *heroes = GetStatHeroes(name);

    char message[8];

    // рисуем фон ячеек для монстров героя
    if(heroes){

	cx = video->w / 2 - 208;
	cy = video->h / 2 + 121;

	for( i = 0; i < HEROESMAXARMY; ++i)
	    // если есть у героя монстры
	    if(heroes->army[i].count){

		// то рисуем фон в зависимости от расы
		switch(GetRaceMonster(heroes->army[i].monster)){
		
		    case KNIGHT:
			FillSPRITE(&sprite, "STRIP.ICN", 4);
			break;

		    case BARBARIAN:
			FillSPRITE(&sprite, "STRIP.ICN", 5);
			break;

		    case SORCERESS:
			FillSPRITE(&sprite, "STRIP.ICN", 6);
			break;

		    case WARLOCK:
			FillSPRITE(&sprite, "STRIP.ICN", 7);
			break;

		    case WIZARD:
			FillSPRITE(&sprite, "STRIP.ICN", 8);
			break;

		    case NECROMANCER:
			FillSPRITE(&sprite, "STRIP.ICN", 9);
			break;

		    default:
			FillSPRITE(&sprite, "STRIP.ICN", 10);
			break;
		}
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);

		// рисуем монстров
		FillSPRITE(&sprite, MonsterBigNamePortrait(heroes->army[i].monster), 0);
		header = GetICNHeader(&sprite);
		rectCur.x = cx + 88 * i + header->offsetX;
		rectCur.y = cy + header->offsetY;
		rectCur.w = header->surface->w;
		rectCur.h = header->surface->h;
		SDL_BlitSurface(header->surface, NULL, video, &rectCur);
		
		// рисуем количество
		sprintf(message, "%5d", heroes->army[i].count);
		rectCur.x = cx + 88 * i + 54;
		rectCur.y = cy + 80;
		rectCur.w = GetLengthText(message, FONT_SMALL);
		rectCur.h = FONT_HEIGHTSMALL;
		PrintText(video, &rectCur, message, FONT_SMALL);

	    }else{
		FillSPRITE(&sprite, "STRIP.ICN", 2);
		image = GetICNSprite(&sprite);
		rectCur.x = cx + (image->w + 6) * i;
		rectCur.y = cy;
		rectCur.w = image->w;
		rectCur.h = image->h;
		SDL_BlitSurface(image, NULL, video, &rectCur);
	    }
    }
}

S_CASTLE *GetFirstCastle(E_COLORS color){

    Uint8 i;
    currentCastle = NULL;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color){
	    currentCastle = &ptrCastle[i];
	    return currentCastle;
	}

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetFirstCastle: return NULL\n");

    return NULL;
}

S_CASTLE *GetNextCastle(E_COLORS color){

    Uint8 i;
    BOOL  flag = FALSE;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color){
	    if(flag){
		currentCastle = &ptrCastle[i];
		return currentCastle;
	    }else if(currentCastle == &ptrCastle[i]) flag = TRUE;
	}

    return NULL;
}

S_CASTLE *GetPrevCastle(E_COLORS color){

    Uint8 i;
    S_CASTLE *result = NULL;
    
    for(i = 0; i < countCastle; ++i)

	if(ptrCastle[i].color == color){
	    if(currentCastle == &ptrCastle[i]){
		currentCastle = result;
		return result;
	    }else
		result = &ptrCastle[i];
	}

    return NULL;
}

S_CASTLE *GetEndCastle(E_COLORS color){

    Uint8 i;
    currentCastle = NULL;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color) currentCastle = &ptrCastle[i];

    if(!currentCastle && GetIntValue(DEBUG)) fprintf(stderr, "GetEndCastle: return NULL\n");

    return currentCastle;
}

Uint8 GetCountCastle(E_COLORS color){

    Uint8 i;
    Uint8 result = 0;

    for(i = 0; i < countCastle; ++i)
	if(ptrCastle[i].color == color) ++result;

    return result;
}

E_MONSTER GetMonsterFromCastle(const S_CASTLE *castle, Uint8 level){

    switch(castle->race){
	
	case BARBARIAN:
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return WAR_TROLL;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return LORD_OGRE;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return CHIEF_ORC;
	    if(level == 6) return CYCLOPS;
	    if(level == 5) return TROLL;
	    if(level == 4) return OGRE;
	    if(level == 3) return WOLF;
	    if(level == 2) return ORC;
	    if(level == 1) return GOBLIN;
	    break;

	case KNIGHT:
	    if(castle->dwelling & DWELLING_UPGRADE6 && level == 6) return CRUSADER;
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return CHAMPION;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return MASTER_SWORDSMAN;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return VETERAN_PIKEMAN;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return RANGER;
	    if(level == 6) return PALADIN;
	    if(level == 5) return CAVALIRY;
	    if(level == 4) return SWORDSMAN;
	    if(level == 3) return PIKEMAN;
	    if(level == 2) return ARCHER;
	    if(level == 1) return PEASANT;
	    break;

	case SORCERESS:
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return GREATER_DRUID;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return GRAND_ELF;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return BATTLE_DWARF;
	    if(level == 6) return PHOENIX;
	    if(level == 5) return UNICORN;
	    if(level == 4) return DRUID;
	    if(level == 3) return ELF;
	    if(level == 2) return DWARF;
	    if(level == 1) return SPRITE;
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_UPGRADE7 && level == 6) return BLACK_DRAGON;
	    if(castle->dwelling & DWELLING_UPGRADE6 && level == 6) return RED_DRAGON;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return KNIGHT_MINOTAUR;
	    if(level == 6) return GREEN_DRAGON;
	    if(level == 5) return HIDRA;
	    if(level == 4) return MINOTAUR;
	    if(level == 3) return GRIFFIN;
	    if(level == 2) return GARGOYLE;
	    if(level == 1) return CENTAUR;
	    break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_UPGRADE6 && level == 6) return TITAN;
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return ARCHMAGE;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return STEEL_GOLEM;
	    if(level == 6) return GIANT;
	    if(level == 5) return MAGE;
	    if(level == 4) return ROC;
	    if(level == 3) return IRON_GOLEM;
	    if(level == 2) return BOAR;
	    if(level == 1) return HALFLING;
	    break;

	case NECROMANCER:
	    if(castle->dwelling & DWELLING_UPGRADE5 && level == 5) return POWER_LICH;
	    if(castle->dwelling & DWELLING_UPGRADE4 && level == 4) return LORD_VAMPIRE;
	    if(castle->dwelling & DWELLING_UPGRADE3 && level == 3) return ROYAL_MUMMY;
	    if(castle->dwelling & DWELLING_UPGRADE2 && level == 2) return MUTANT_ZOMBIE;
	    if(level == 6) return BONE_DRAGON;
	    if(level == 5) return LICH;
	    if(level == 4) return VAMPIRE;
	    if(level == 3) return MUMMY;
	    if(level == 2) return ZOMBIE;
	    if(level == 1) return SKELETON;
	    break;
				
	default:
	    break;
    }

    if(GetIntValue(DEBUG)) fprintf(stderr, "GetMonsterFromCastle: return MONSTERNONE, level: %d, castle: %s, race: %s, ax: %d, ay: %d\n", level, castle->name, GetStringRace(castle->race), castle->ax, castle->ay);

    return MONSTERNONE;
}

BOOL BuyMonsterFromCastle(const S_CASTLE *castle, Uint8 level, Uint16 count){

    currentCastle = NULL;
    Uint8 i;
    BOOL free = FALSE;

    for(i = 0; i < countCastle; ++i)
	if(&ptrCastle[i] == castle) currentCastle = &ptrCastle[i];

    if(	! count ||
	! currentCastle ||
	! level ||
	level > CASTLEMAXMONSTER ||
	currentCastle->monster[level - 1] < count ) return FALSE;

    E_MONSTER emonster = GetMonsterFromCastle(castle, level);

    // проверяем свободный слот
    for(i = 0; i < CASTLEMAXARMY; ++i)
	if(! castle->army[i].count || castle->army[i].monster == emonster){ free = TRUE; break; }

    if( ! free ||
	MONSTERNONE == emonster ||
	! CheckBuyMonsterFromCastle(castle, level, count) ) return FALSE;
 
    KingdomWasteResource(castle->color, GetMultiPayment(PaymentConditionsMonster(emonster), count));

    currentCastle->monster[level - 1] -= count;
    currentCastle->army[i].monster = emonster;
    currentCastle->army[i].count += count;

    return TRUE;
}

BOOL CheckBuyMonsterFromCastle(const S_CASTLE *castle, Uint8 level, Uint16 count){

    E_MONSTER emonster = GetMonsterFromCastle(castle, level);

    if(MONSTERNONE == emonster) return FALSE;

    const S_PAYMENT *payment = PaymentConditionsMonster(emonster);

    // проверяем платежеспособность
    if( !KingdomAllowPayment(castle->color, GetMultiPayment(payment, count))) return FALSE;
    
    return TRUE;
}

void RedrawCastleInfoResource(void){

    SDL_Rect rectCur, rectBack;
    SDL_Surface *video, *image;
    const S_CASTLE *castle = GetCurrentCastle();
    S_KINGDOM *kingdom = NULL;
    AGGSPRITE sprite;
    char message[8];
    Uint16 cx, cy;
    
    video = SDL_GetVideoSurface();
    if(GetIntValue(VIDEOMODE)){
        rectBack.x = video->w / 2 - 320 - BORDERWIDTH - SHADOWWIDTH;
        rectBack.y = video->h / 2 - 240 - BORDERWIDTH;
        rectBack.w = 640 + 2 * BORDERWIDTH + SHADOWWIDTH;
        rectBack.h = 480 + 2 * BORDERWIDTH + SHADOWWIDTH;
    }else{
        rectBack.x = 0;
        rectBack.y = 0;
        rectBack.w = 640;
        rectBack.h = 480;
    }

    // закрашиваем фон для инфо ресурсов
    if(GetIntValue(VIDEOMODE)){
	rectCur.x = rectBack.x + 584;
	rectCur.y = rectBack.y + 278;
	rectCur.w = 82;
	rectCur.h = 192;
    }else{
	rectCur.x = rectBack.x + 584 - BORDERWIDTH * 2;
	rectCur.y = rectBack.y + 278 - BORDERWIDTH;
	rectCur.w = 82;
	rectCur.h = 192;
    }
    SDL_FillRect(video, &rectCur, 0);
    cx = rectCur.x;
    cy = rectCur.y;
    // sprite wood
    FillSPRITE(&sprite, "RESOURCE.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 1;
    rectCur.y = cy + 10;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count wood
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%5d", kingdom->wood);
    rectCur.x = cx + 8;
    rectCur.y = cy + 32;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // sprite sulfur
    FillSPRITE(&sprite, "RESOURCE.ICN", 3);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 42;
    rectCur.y = cy + 6;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count sulfur
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%5d", kingdom->sulfur);
    rectCur.x = cx + 48;
    rectCur.y = cy + 32;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // sprite crystal
    FillSPRITE(&sprite, "RESOURCE.ICN", 4);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 1;
    rectCur.y = cy + 45;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count crystal
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%5d", kingdom->crystal);
    rectCur.x = cx + 8;
    rectCur.y = cy + 78;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // sprite mercury
    FillSPRITE(&sprite, "RESOURCE.ICN", 1);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 44;
    rectCur.y = cy + 47;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count mercury
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%5d", kingdom->mercury);
    rectCur.x = cx + 48;
    rectCur.y = cy + 78;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // sprite ore
    FillSPRITE(&sprite, "RESOURCE.ICN", 2);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 1;
    rectCur.y = cy + 92;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count ore
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%5d", kingdom->ore);
    rectCur.x = cx + 8;
    rectCur.y = cy + 118;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // sprite gems
    FillSPRITE(&sprite, "RESOURCE.ICN", 5);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 45;
    rectCur.y = cy + 92;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count gems
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%5d", kingdom->gems);
    rectCur.x = cx + 48;
    rectCur.y = cy + 118;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // sprite gold
    FillSPRITE(&sprite, "RESOURCE.ICN", 6);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 6;
    rectCur.y = cy + 130;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
    // text count gold
    kingdom = GetStatKingdom(castle->color);
    sprintf(message, "%7d", kingdom->gold);
    rectCur.x = cx + 24;
    rectCur.y = cy + 154;
    rectCur.w = GetLengthText(message, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, message, FONT_SMALL);
    // exit
    FillSPRITE(&sprite, "SWAPBTN.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx +  1;
    rectCur.y = cy + 166;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);
}

BOOL CastleNearOcean(const S_CASTLE * castle){

    return FALSE;
}

BOOL CastleDwellingUpgradable(const S_CASTLE *castle, E_DWELLINGCASTLE level){

    switch(castle->race){
    
	case BARBARIAN:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE4) ||
		(level & DWELLING_UPGRADE5) ) return TRUE;
	    break;
	
	case KNIGHT:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE3) ||
		(level & DWELLING_UPGRADE4) ||
		(level & DWELLING_UPGRADE5) ||
		(level & DWELLING_UPGRADE6) ) return TRUE;
	    break;

	case NECROMANCER:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE3) ||
		(level & DWELLING_UPGRADE4) ||
		(level & DWELLING_UPGRADE5) ) return TRUE;
	    break;

	case SORCERESS:
	    if( (level & DWELLING_UPGRADE2) ||
	        (level & DWELLING_UPGRADE3) ||
		(level & DWELLING_UPGRADE4) ) return TRUE;
	    break;

	case WARLOCK:
	    if( (level & DWELLING_UPGRADE4) ||
	        (level & DWELLING_UPGRADE6) ||
		(level & DWELLING_UPGRADE7) ) return TRUE;
	    break;

	case WIZARD:
	    if( (level & DWELLING_UPGRADE3) ||
	        (level & DWELLING_UPGRADE5) ||
		(level & DWELLING_UPGRADE6) ) return TRUE;
	    break;

	default:
	    break;
    }

    return FALSE;
}

BUILDACTION AllowBuildCastle(const S_CASTLE *castle){

    if(castle->building & BUILD_CASTLE) return ALREADY_BUILD;
    if(! castle->allowCastle) return CANNOT_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;
    
    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_CASTLE))) return BUILD_OK;
    
    return CANNOT_BUILD;
}

BUILDACTION AllowBuildMageGuild(const S_CASTLE *castle){

    if(MAGIC_LEVEL5 == GetMageGuildLevel(castle)) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;
    
    switch(GetMageGuildLevel(castle)){
	case MAGIC_NONE:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD1))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL1:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD2))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL2:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD3))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL3:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD4))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL4:
	    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD5))) return BUILD_OK;
	    break;
	case MAGIC_LEVEL5:
	    return ALREADY_BUILD;
	    break;
    }

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildTavern(const S_CASTLE *castle){

    if(castle->building & BUILD_TAVERN) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_TAVERN))) return BUILD_OK;
    
    return CANNOT_BUILD;
}


BUILDACTION AllowBuildThievesGuild(const S_CASTLE *castle){

    if(castle->building & BUILD_THIEVESGUILD) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_THIEVESGUILD))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildShipyard(const S_CASTLE *castle){

    if(castle->building & BUILD_SHIPYARD) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_SHIPYARD))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildStatue(const S_CASTLE *castle){

    if(castle->building & BUILD_STATUE) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_STATUE))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildMarketplace(const S_CASTLE *castle){

    if(castle->building & BUILD_MARKETPLACE) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MARKETPLACE))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildWell(const S_CASTLE *castle){

    if(castle->building & BUILD_WELL) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_WELL))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildMoat(const S_CASTLE *castle){

    if(castle->building & BUILD_MOAT) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_MOAT))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildLeftTurret(const S_CASTLE *castle){

    if(castle->building & BUILD_LEFTTURRET) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_LEFTTURRET))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildRightTurret(const S_CASTLE *castle){

    if(castle->building & BUILD_RIGHTTURRET) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_RIGHTTURRET))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildCaptain(const S_CASTLE *castle){

    if(castle->building & BUILD_CAPTAIN) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_CAPTAIN))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildBoat(const S_CASTLE *castle){

    if(castle->boat) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_BOAT))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildWel2(const S_CASTLE *castle){

    if(castle->building & BUILD_WEL2) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_WEL2))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildSpec(const S_CASTLE *castle){

    if(castle->building & BUILD_SPEC) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsBuilding(castle->race, BUILD_SPEC))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling1(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_MONSTER1) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER1))) return BUILD_OK;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling2(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE2) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER2))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
	    case NECROMANCER:
	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER2)) &&
		    castle->dwelling & DWELLING_MONSTER1 ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER2)) &&
		    castle->dwelling & DWELLING_MONSTER1  &&
		    castle->building & BUILD_TAVERN ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE2))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE2)) &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;
	    
	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_UPGRADE2)) ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_UPGRADE2)) &&
		    castle->building & BUILD_WELL ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling3(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE3) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER3))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_MONSTER3)) &&
		    castle->dwelling & DWELLING_MONSTER1 &&
		    castle->building & BUILD_WELL ) return BUILD_OK;
		break;
	    
	    case BARBARIAN:
	    case NECROMANCER:
	    case SORCERESS:
	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER3)) &&
		    castle->dwelling & DWELLING_MONSTER1 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE3))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_UPGRADE3)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;
	    
	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_UPGRADE3)) ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_UPGRADE3)) &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;

	    case WIZARD:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WIZARD, DWELLING_UPGRADE3)) &&
		    castle->building & BUILD_WELL ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling4(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE4) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER4))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER1 &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->building & BUILD_TAVERN ) return BUILD_OK;
		break;
	    
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(BARBARIAN, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER1 ) return BUILD_OK;
		break;
	    
	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->building & BUILD_THIEVESGUILD ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    GetMageGuildLevel(castle) ) return BUILD_OK;
		break;

	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER4)) &&
		    castle->dwelling & DWELLING_MONSTER2 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE4))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE4)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 ) return BUILD_OK;
		break;

	    case NECROMANCER:
	    case SORCERESS:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE4)) ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling5(const S_CASTLE *castle){

    if(castle->dwelling & DWELLING_UPGRADE5) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER5))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;

	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    GetMageGuildLevel(castle) ) return BUILD_OK;
		break;
	    
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(SORCERESS, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;

	    case WIZARD:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WIZARD, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    GetMageGuildLevel(castle) ) return BUILD_OK;
		break;
	
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WARLOCK, DWELLING_MONSTER5)) &&
		    castle->dwelling & DWELLING_MONSTER3 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE5))

	switch(castle->race){

	    case KNIGHT:
	    case BARBARIAN:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE5)) ) return BUILD_OK;
		break;

	    case NECROMANCER:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(NECROMANCER, DWELLING_UPGRADE5)) &&
		    GetMageGuildLevel(castle) > MAGIC_LEVEL1) return BUILD_OK;
		break;

	    case WIZARD:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WIZARD, DWELLING_UPGRADE5)) &&
		    castle->building & BUILD_SPEC ) return BUILD_OK;
		break;

	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BUILDACTION AllowBuildDwelling6(const S_CASTLE *castle){

    if((castle->race != WARLOCK && castle->dwelling & DWELLING_UPGRADE6) || castle->dwelling & DWELLING_UPGRADE7) return ALREADY_BUILD;

    if(! KingdomAllowBuild(castle->color)) return END_TUR;

    if(! (castle->dwelling & DWELLING_MONSTER6))

	switch(castle->race){

	    case KNIGHT:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(KNIGHT, DWELLING_MONSTER6)) &&
		    castle->dwelling & DWELLING_MONSTER2 &&
		    castle->dwelling & DWELLING_MONSTER3 &&
		    castle->dwelling & DWELLING_MONSTER4 ) return BUILD_OK;
		break;
	    
	    case BARBARIAN:
	    case NECROMANCER:
	    case SORCERESS:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER6)) &&
		    castle->dwelling & DWELLING_MONSTER5 ) return BUILD_OK;
		break;

	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER6)) &&
		    castle->dwelling & DWELLING_MONSTER4 &&
		    castle->dwelling & DWELLING_MONSTER5 ) return BUILD_OK;
		break;
	
	    default:
		break;
	}

    else if(castle->dwelling & DWELLING_UPGRADE6 && CastleDwellingUpgradable(castle, DWELLING_UPGRADE7))

	switch(castle->race){

	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(WARLOCK, DWELLING_UPGRADE7)) ) return BUILD_OK;
		break;


	    default:
		break;
	}

    else if(CastleDwellingUpgradable(castle, DWELLING_UPGRADE6))

	switch(castle->race){

	    case KNIGHT:
	    case WIZARD:
	    case WARLOCK:
		if( KingdomAllowPayment(castle->color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE6)) ) return BUILD_OK;
		break;


	    default:
		break;
	}

    else return ALREADY_BUILD;

    return CANNOT_BUILD;
}

BOOL BuildMageGuild(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    switch(GetMageGuildLevel(&ptrCastle[i])){
	    
		case MAGIC_NONE:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD1));
		    ptrCastle[i].building |= BUILD_MAGEGUILD1;
		    FillMageGuildLevel(&ptrCastle[i].mageGuild, &ptrCastle[i], MAGIC_LEVEL1);
		    break;
		    
		case MAGIC_LEVEL1:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD2));
		    ptrCastle[i].building |= BUILD_MAGEGUILD2;
		    FillMageGuildLevel(&ptrCastle[i].mageGuild, &ptrCastle[i], MAGIC_LEVEL2);
		    break;

		case MAGIC_LEVEL2:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD3));
		    ptrCastle[i].building |= BUILD_MAGEGUILD3;
		    FillMageGuildLevel(&ptrCastle[i].mageGuild, &ptrCastle[i], MAGIC_LEVEL3);
		    break;

		case MAGIC_LEVEL3:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD4));
		    ptrCastle[i].building |= BUILD_MAGEGUILD4;
		    FillMageGuildLevel(&ptrCastle[i].mageGuild, &ptrCastle[i], MAGIC_LEVEL4);
		    break;

		case MAGIC_LEVEL4:
		    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MAGEGUILD5));
		    ptrCastle[i].building |= BUILD_MAGEGUILD5;
		    FillMageGuildLevel(&ptrCastle[i].mageGuild, &ptrCastle[i], MAGIC_LEVEL5);
		    break;
		    
		case MAGIC_LEVEL5:
		    return FALSE;
		    break;
	    }

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}
	
    return FALSE;
}

BOOL BuildRightTurret(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_RIGHTTURRET;
	    
	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_RIGHTTURRET));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildLeftTurret(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_LEFTTURRET;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_LEFTTURRET));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildTavern(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_TAVERN;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_TAVERN));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildStatue(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_STATUE;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_STATUE));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildMarketplace(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_MARKETPLACE;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MARKETPLACE));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildThievesGuild(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_THIEVESGUILD;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_THIEVESGUILD));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildWell(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_WELL;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_WELL));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildMoat(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_MOAT;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_MOAT));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildWel2(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_WEL2;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_WEL2));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildSpec(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_SPEC;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_SPEC));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildShipyard(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_SHIPYARD;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_SHIPYARD));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildCaptain(const S_CASTLE *castle){

    Uint8 i;
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].building |= BUILD_CAPTAIN;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsBuilding(castle->race, BUILD_CAPTAIN));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling1(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER1;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER1));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling2(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER2;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER2));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling3(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER3;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER3));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling4(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER4;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER4));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling5(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER5;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER5));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildDwelling6(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_MONSTER6;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_MONSTER6));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade2(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE2;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE2));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade3(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE3;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE3));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade4(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE4;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE4));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade5(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE5;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE5));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade6(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE6;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE6));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

BOOL BuildUpgrade7(const S_CASTLE *castle){

    Uint8 i;
    
    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    ptrCastle[i].dwelling |= DWELLING_UPGRADE7;

	    KingdomWasteResource(ptrCastle[i].color, PaymentConditionsDwelling(castle->race, DWELLING_UPGRADE7));

	    KingdomSetAllowBuild(ptrCastle[i].color, FALSE);

	    return TRUE;
	}

    return FALSE;
}

void UpdateCastleBuilding(void){

    const S_CASTLE *castle = GetCurrentCastle();
    
    CursorOff();
    SetIntValue(ANIM3, FALSE);

    RemoveActionLevelEvent(castlact, LEVELEVENT_CASTLEUPDATEBUILD);

    switch(castle->race){

	case KNIGHT:
	    DrawKNGTCastle(NULL, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawKNGTCapitan(NULL, &castlact);
	    if(castle->building & BUILD_WEL2) DrawKNGTWel2(NULL, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawKNGTLTurret(NULL, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawKNGTRTurret(NULL, &castlact);
	    if(castle->building & BUILD_MOAT) DrawKNGTMoat(NULL, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawKNGTMarketplace(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawKNGTDwelling2(NULL, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawKNGTThievesGuild(NULL, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawKNGTTavern(NULL, &castlact);
	    DrawKNGTMageGuild(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawKNGTDwelling5(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawKNGTDwelling6(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawKNGTDwelling1(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawKNGTDwelling3(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawKNGTDwelling4(NULL, &castlact);
	    if(castle->building & BUILD_WELL) DrawKNGTWell(NULL, &castlact);
	    if(castle->building & BUILD_STATUE) DrawKNGTStatue(NULL, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawKNGTShipyard(NULL, &castlact);
	    else DrawKNGTExt0(NULL, &castlact);
	    //DrawKNGTExt1(NULL, &castlact); // развилка дорог?
	    //DrawKNGTExt2(NULL, &castlact); // развилка дорог?
	    break;

	case BARBARIAN:
	    if(castle->building & BUILD_SPEC) DrawBRBNSpec(NULL, &castlact);
	    if(castle->building & BUILD_WEL2) DrawBRBNWel2(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawBRBNDwelling6(NULL, &castlact);
	    if(GetMageGuildLevel(castle)){
		DrawBRBNMageGuild(NULL, &castlact);
		DrawBRBNExt2(NULL, &castlact);
	    }
	    if(castle->building & BUILD_CAPTAIN) DrawBRBNCapitan(NULL, &castlact);
	    DrawBRBNCastle(NULL, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawBRBNLTurret(NULL, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawBRBNRTurret(NULL, &castlact);
	    if(castle->building & BUILD_MOAT) DrawBRBNMoat(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawBRBNDwelling3(NULL, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawBRBNThievesGuild(NULL, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawBRBNTavern(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawBRBNDwelling1(NULL, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawBRBNMarketplace(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawBRBNDwelling2(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawBRBNDwelling4(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawBRBNDwelling5(NULL, &castlact);
	    if(castle->building & BUILD_WELL) DrawBRBNWell(NULL, &castlact);
	    if(castle->building & BUILD_STATUE) DrawBRBNStatue(NULL, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawBRBNShipyard(NULL, &castlact);
	    else DrawBRBNExt0(NULL, &castlact);
	    DrawBRBNExt1(NULL, &castlact);
	    //DrawBRBNExt3(NULL, &castlact); // развилка дорог?
	    break;

	case SORCERESS:
	    if(castle->building & BUILD_SPEC) DrawSCRSSpec(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawSCRSDwelling6(NULL, &castlact);
	    DrawSCRSMageGuild(NULL, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawSCRSCapitan(NULL, &castlact);
	    DrawSCRSCastle(NULL, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawSCRSLTurret(NULL, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawSCRSRTurret(NULL, &castlact);
	    if(castle->building & BUILD_MOAT) DrawSCRSMoat(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawSCRSDwelling3(NULL, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawSCRSShipyard(NULL, &castlact);
	    else DrawSCRSExt0(NULL, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawSCRSMarketplace(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawSCRSDwelling2(NULL, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawSCRSThievesGuild(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawSCRSDwelling1(NULL, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawSCRSTavern(NULL, &castlact);
	    if(castle->building & BUILD_STATUE && castle->building & BUILD_WEL2) DrawSCRSExt1(NULL, &castlact);
	    else if(castle->building & BUILD_STATUE) DrawSCRSStatue(NULL, &castlact);
	    else if(castle->building & BUILD_WEL2) DrawSCRSWel2(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawSCRSDwelling4(NULL, &castlact);
	    if(castle->building & BUILD_WELL) DrawSCRSWell(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawSCRSDwelling5(NULL, &castlact);
	break;

	case NECROMANCER:
	    if(castle->building & BUILD_SPEC) DrawNCRMSpec(NULL, &castlact);
	    DrawNCRMCastle(NULL, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawNCRMCapitan(NULL, &castlact);
    	    if(castle->building & BUILD_LEFTTURRET) DrawNCRMLTurret(NULL, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawNCRMRTurret(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawNCRMDwelling6(NULL, &castlact);
	    if(castle->building & BUILD_MOAT) DrawNCRMMoat(NULL, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawNCRMShipyard(NULL, &castlact);
	    else DrawNCRMExt0(NULL, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawNCRMThievesGuild(NULL, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawNCRMTavern(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawNCRMDwelling3(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawNCRMDwelling5(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawNCRMDwelling2(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawNCRMDwelling4(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawNCRMDwelling1(NULL, &castlact);
	    DrawNCRMMageGuild(NULL, &castlact);
	    if(castle->building & BUILD_WEL2) DrawNCRMWel2(NULL, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawNCRMMarketplace(NULL, &castlact);
	    if(castle->building & BUILD_STATUE) DrawNCRMStatue(NULL, &castlact);
	    if(castle->building & BUILD_WELL) DrawNCRMWell(NULL, &castlact);
	    break;

	case WARLOCK:
	    if(castle->dwelling & DWELLING_MONSTER5) DrawWRLKDwelling5(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawWRLKDwelling3(NULL, &castlact);
	    DrawWRLKCastle(NULL, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawWRLKLTurret(NULL, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWRLKRTurret(NULL, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawWRLKCapitan(NULL, &castlact);
	    if(castle->building & BUILD_MOAT) DrawWRLKMoat(NULL, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawWRLKShipyard(NULL, &castlact);
	    else DrawWRLKExt0(NULL, &castlact);
	    DrawWRLKMageGuild(NULL, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawWRLKTavern(NULL, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawWRLKThievesGuild(NULL, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawWRLKMarketplace(NULL, &castlact);
	    if(castle->building & BUILD_STATUE) DrawWRLKStatue(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWRLKDwelling1(NULL, &castlact);
	    if(castle->building & BUILD_WEL2) DrawWRLKWel2(NULL, &castlact);
	    if(castle->building & BUILD_SPEC) DrawWRLKSpec(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawWRLKDwelling4(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawWRLKDwelling2(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER6) DrawWRLKDwelling6(NULL, &castlact);
	    if(castle->building & BUILD_WELL) DrawWRLKWell(NULL, &castlact);
	break;

	case WIZARD:
	    if(castle->dwelling & DWELLING_MONSTER6) DrawWZRDDwelling6(NULL, &castlact);
	    DrawWZRDCastle(NULL, &castlact);
	    if(castle->building & BUILD_LEFTTURRET) DrawWZRDLTurret(NULL, &castlact);
	    if(castle->building & BUILD_RIGHTTURRET) DrawWZRDRTurret(NULL, &castlact);
	    if(castle->building & BUILD_MOAT) DrawWZRDMoat(NULL, &castlact);
	    if(castle->building & BUILD_CAPTAIN) DrawWZRDCapitan(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER2) DrawWZRDDwelling2(NULL, &castlact);
	    if(castle->building & BUILD_THIEVESGUILD) DrawWZRDThievesGuild(NULL, &castlact);
	    if(castle->building & BUILD_TAVERN) DrawWZRDTavern(NULL, &castlact);
	    if(castle->building & BUILD_SHIPYARD) DrawWZRDShipyard(NULL, &castlact);
	    else DrawWZRDExt0(NULL, &castlact);
	    if(castle->building & BUILD_WELL) DrawWZRDWell(NULL, &castlact);
	    if(castle->building & BUILD_SPEC) DrawWZRDSpec(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER3) DrawWZRDDwelling3(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER5) DrawWZRDDwelling5(NULL, &castlact);
	    if(GetMageGuildLevel(castle)) DrawWZRDMageGuild(NULL, &castlact);
	    if(castle->building & BUILD_STATUE) DrawWZRDStatue(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER1) DrawWZRDDwelling1(NULL, &castlact);
	    if(castle->dwelling & DWELLING_MONSTER4) DrawWZRDDwelling4(NULL, &castlact);
	    if(castle->building & BUILD_MARKETPLACE) DrawWZRDMarketplace(NULL, &castlact);
	    if(castle->building & BUILD_WEL2) DrawWZRDWel2(NULL, &castlact);
	break;
	
	default:
	return;
	break;
    }

    // обновим рамку
    RedrawRamkaCastleName();
    
    // раз купили, так обновим инфо
    RedrawCastleInfoResource();

    flagUpdateBuilding = FALSE;
    SetIntValue(ANIM3, TRUE);
    CursorOn();
}

void EnableCastleUpdateBuilding(void){

    flagUpdateBuilding = TRUE;
}

void RedrawRamkaCastleName(void){

    SDL_Surface *video = SDL_GetVideoSurface();
    SDL_Surface *image = NULL;
    SDL_Rect rectCur;
    AGGSPRITE sprite;
    Uint16 cx, cy;
    
    const S_CASTLE *castle = GetCurrentCastle();

    if(GetIntValue(VIDEOMODE)){
        cx = video->w / 2 - 320;
	cy = video->h / 2 - 240;
    }else{
        cx = 0;
        cy = 0;
    }

    // рисуем сетку
    FillSPRITE(&sprite, "STRIP.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx;
    rectCur.y = cy + 256;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // рамка наименования
    FillSPRITE(&sprite, "TOWNNAME.ICN", 0);
    image = GetICNSprite(&sprite);
    rectCur.x = cx + 320 - image->w / 2;
    rectCur.y = cy + 248;
    rectCur.w = image->w;
    rectCur.h = image->h;
    SDL_BlitSurface(image, NULL, video, &rectCur);

    // наименование замка
    rectCur.x = cx + 320 - GetLengthText(castle->name, FONT_SMALL) / 2 ;
    rectCur.y = cy + 248;
    rectCur.w = GetLengthText(castle->name, FONT_SMALL);
    rectCur.h = FONT_HEIGHTSMALL;
    PrintText(video, &rectCur, castle->name, FONT_SMALL);
}

void CastleIncreaseArmy(const S_CASTLE *castle, E_DWELLINGCASTLE dwelling, Uint8 count){

    Uint8 i;

    for(i = 0; i < countCastle; ++i)

	if(&ptrCastle[i] == castle){

	    switch(dwelling){
	    
		case DWELLING_MONSTER1:
		    ptrCastle[i].monster[0] += count;
		    break;

		case DWELLING_MONSTER2:
		    ptrCastle[i].monster[1] += count;
		    break;

		case DWELLING_MONSTER3:
		    ptrCastle[i].monster[2] += count;
		    break;

		case DWELLING_MONSTER4:
		    ptrCastle[i].monster[3] += count;
		    break;

		case DWELLING_MONSTER5:
		    ptrCastle[i].monster[4] += count;
		    break;

		case DWELLING_MONSTER6:
		    ptrCastle[i].monster[5] += count;
		    break;
		
		default:
		    break;
	    }
	    
	    return;
	}
}

Uint8 GetMonsterGrownCastle(const S_CASTLE *castle, E_MONSTER name){

    Uint8 grown = GetMonsterGrown(name);

        if(castle){
            // well
            if(castle->building & BUILD_WELL) grown +=2;
            // wel2
            if(GOBLIN == name && castle->building & BUILD_WEL2) grown += 8;
            if(PEASANT == name && castle->building & BUILD_WEL2) grown += 8;
            if(SKELETON == name && castle->building & BUILD_WEL2) grown += 8;
            if(SPRITE == name  && castle->building & BUILD_WEL2) grown += 8;
            if(HALFLING == name && castle->building & BUILD_WEL2) grown += 8;
            if(CENTAUR == name && castle->building & BUILD_WEL2) grown += 8;
        }

    return grown;
}

void AllCastleIncreaseArmy(void){

    Uint8 i;

    for(i = 0; i < countCastle; ++i){

	if(ptrCastle[i].dwelling & DWELLING_MONSTER1) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER1, GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[i], 1)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER2 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE2) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER2, GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[i], 2)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER3 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE3) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER3, GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[i], 3)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER4 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE4) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER4, GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[i], 4)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER5 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE5) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER5, GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[i], 5)));

	if(ptrCastle[i].dwelling & DWELLING_MONSTER6 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE6 ||
	   ptrCastle[i].dwelling & DWELLING_UPGRADE7) CastleIncreaseArmy(&ptrCastle[i], DWELLING_MONSTER6, GetMonsterGrown(GetMonsterFromCastle(&ptrCastle[i], 6)));
    }
}

E_MAGICLEVEL GetMageGuildLevel(const S_CASTLE *castle){

    if(castle->building & BUILD_MAGEGUILD5) return MAGIC_LEVEL5;
    if(castle->building & BUILD_MAGEGUILD4) return MAGIC_LEVEL4;
    if(castle->building & BUILD_MAGEGUILD3) return MAGIC_LEVEL3;
    if(castle->building & BUILD_MAGEGUILD2) return MAGIC_LEVEL2;
    if(castle->building & BUILD_MAGEGUILD1) return MAGIC_LEVEL1;

    return MAGIC_NONE;
}
