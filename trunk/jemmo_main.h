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

//typedef struct __image_file_info;

/* Структура, в которой храним информацию о файле изображения,
   для хранения ифнормации о списке файлов попробуем использовать
   связанные списки */

typedef struct _image_file_info {
	char FileName;
	int FileSize;
	__source_format source_format;
	struct _image_file_info *prev;
	struct _image_file_info *next;
} __image_file_info;

__image_file_info *ifi_cur;
__image_file_info *ifi_first;

void	jemmo_UpdateWindowSize(HWND hWnd);
int		CreateStatusBar();
HWND	CreateSimpleToolbar(HWND hWndParent);
int		jemmo_MainWindowRepaint();
int		jemmo_AppInit();
void	jemmo_RButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void	jemmo_PreviousImage();
void	jemmo_NextImage();

#endif //_INC_JEMMO_MAIN
