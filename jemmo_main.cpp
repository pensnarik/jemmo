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

#include <windows.h>
#include "jemmo_image.h"
#include "jemmo_main.h"

image *current_image;

void	jemmo_UpdateWindowSize(HWND hWnd)
{
	if (current_image == NULL)
		return;
	int width = current_image->width * current_image->zoom;
	int height = current_image->height * current_image->zoom;

	MoveWindow(hWnd, 0, 0, width, height, TRUE);
	SendMessage(hWnd, WM_SIZE, 0, 0);
}