// swad_menu.h: menu (horizontal or vertical) selection

#ifndef _SWAD_MNU
#define _SWAD_MNU
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Ca�as Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Mnu_NUM_MENUS 2

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Mnu_MENU_HORIZONTAL = 0,
   Mnu_MENU_VERTICAL   = 1,
   Mnu_MENU_UNKNOWN    = 2,
   } Mnu_Menu_t;	// Stored in database. Don't change numbers!
#define Mnu_MENU_DEFAULT Mnu_MENU_HORIZONTAL

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Mnu_WriteMenuThisTab (void);
void Mnu_WriteVerticalMenuThisTab (void);

void Mnu_PutIconsToSelectMenu (void);
void Mnu_ChangeMenu (void);
Mnu_Menu_t Mnu_GetParamMenu (void);

#endif
