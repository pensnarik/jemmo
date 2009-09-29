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
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INC_MAIN
#define _INC_MAIN

#define IDM_NEW 0
#define IDM_OPEN 1
#define IDM_SAVE 2

HWND CreateSimpleToolbar(HWND hWndParent);
BOOL get_module_directory(TCHAR *obuf, size_t osize);

#endif