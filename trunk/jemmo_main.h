/*
    This file is part of Jemmo.

    Jemmo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Jemmo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Jemmo. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INC_JEMMO_MAIN
#define _INC_JEMMO_MAIN

#include "jemmo_image.h"

#define IDM_NEW 0
#define IDM_OPEN 1
#define IDM_SAVE 2

void	jemmo_UpdateWindowSize(HWND hWnd);
int		CreateStatusBar();
HWND	CreateSimpleToolbar(HWND hWndParent);
int		jemmo_MainWindowRepaint();
int		jemmo_AppInit();
void	jemmo_RButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

#endif //_INC_JEMMO_MAIN
