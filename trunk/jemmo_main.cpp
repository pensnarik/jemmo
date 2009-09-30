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
#include <commctrl.h>
#include "jemmo_image.h"
#include "jemmo_main.h"

image *current_image;
HWND hwnd;				// Main window handle
HWND hStatusBar;		// Status bar
HINSTANCE hInst;		// Program hInstance
HBRUSH	bgBrush;

void	jemmo_UpdateWindowSize(HWND hWnd)
{
	if (current_image == NULL)
		return;
	int width = current_image->width * current_image->zoom - 20;
	int height = current_image->height * current_image->zoom - 20;

	MoveWindow(hWnd, 0, 0, width, height, TRUE);
	SendMessage(hWnd, WM_SIZE, 0, 0);
}

int CreateStatusBar()
{
	hStatusBar = CreateWindowEx(0,
								STATUSCLASSNAME,
								"Jemmo_Status",
								SBARS_SIZEGRIP | WS_CHILD,
								0,
								0,
								0,
								0,
								hwnd,
								NULL, // Menu?
								hInst,
								NULL);
	if (hStatusBar == NULL) return -1;
	ShowWindow(hStatusBar, SW_SHOW);
	return 0;
}

int	jemmo_MainWindowRepaint()
{
	RECT rLeft, rBottom, rTop, rRight, rect /* main window rect */;
	HDC hdc;
	PAINTSTRUCT ps;

	if (current_image != NULL)
	{
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		// We have to fill 4 rects to paint a border around the image
		// Now we'll calculate them
		rLeft.left = 0;
		rLeft.top = 0;
		rLeft.bottom = rect.bottom;
		rLeft.right = (rect.right/2 - current_image->width/2);
		FillRect(hdc, &rLeft, bgBrush);

		rRight.left = (rect.right/2 +current_image->width/2);
		rRight.top = 0;
		rRight.bottom = rect.bottom;
		rRight.right = rect.right;
		FillRect(hdc, &rRight, bgBrush);

		rTop.left = 0;
		rTop.top = 0;
		rTop.bottom = (rect.bottom/2 - current_image->height/2);
		rTop.right = rect.right;
		FillRect(hdc, &rTop, bgBrush);

		rBottom.left = 0;
		rBottom.top = (rect.bottom/2 + current_image->height/2);
		rBottom.bottom = rect.bottom;
		rBottom.right = rect.right;
		FillRect(hdc, &rBottom, bgBrush);

		EndPaint(hwnd, &ps);
	}

	return 0;
}

int jemmo_AppInit()
{
//	CreateSimpleToolbar(hwnd);
//	CreateStatusBar();

	bgBrush = CreateSolidBrush(0x0000);

	return 0;
}

HWND CreateSimpleToolbar(HWND hWndParent)
{
    // Define some constants.
    const int ImageListID = 0;
    const int numButtons = 3;
    const DWORD buttonStyles = BTNS_AUTOSIZE;
    const int bitmapSize = 16;

    // Create the toolbar.
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, 
        WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_FLAT,
        0, 0, 0, 0,
        hWndParent, NULL, hInst, NULL);
    if (hWndToolbar == NULL)
    {
        return NULL;
    }

    // Create the imagelist.
    HIMAGELIST hImageList = ImageList_Create(
        bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
        ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
        numButtons, 0);

    // Set the image list.
    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, 
        (LPARAM)hImageList);

    // Load the button images.
    SendMessage(hWndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, 
        (LPARAM)HINST_COMMCTRL);

    // Initialize button info.
    // IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.
    TBBUTTON tbButtons[numButtons] = 
    {
        { MAKELONG(STD_FILENEW, ImageListID), IDM_NEW, TBSTATE_ENABLED, 
          buttonStyles, {0}, 0, (INT_PTR)L"New" },
        { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, 
          buttonStyles, {0}, 0, (INT_PTR)L"Open"},
        { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, 0, 
          buttonStyles, {0}, 0, (INT_PTR)L"Save"}
    };

    // Add buttons.
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, 
        (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, 
        (LPARAM)&tbButtons);

    // Tell the toolbar to resize itself, and show it.
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0); 
    ShowWindow(hWndToolbar, TRUE);
    return hWndToolbar;
}