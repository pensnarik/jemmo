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
#include "jemmo_malloc.h"
#include "resource.h"
#include <shlwapi.h>

image *current_image;
HWND hStatusBar;		// Status bar
HINSTANCE hInst;		// Program hInstance
HBRUSH	bgBrush;
HWND hwnd;

WIN32_FIND_DATA lpFindFileData;
__image_file_info *ifi_cur;
__image_file_info *ifi_first;
char * command_line;
extern HWND hwnd;

/* Функция возвращает true, если path содержит имя директории,
   в противном случае функция возвращает false */

bool	jemmo_IsDirectory(const char *path)
{
	WIN32_FIND_DATA lpF;
	TCHAR fullpath[MAX_PATH];
	HANDLE f;

	GetFullPathName(path, MAX_PATH, fullpath, NULL); 

	if ((f = FindFirstFile(fullpath, &lpF)) != INVALID_HANDLE_VALUE) 
	{
		if (lpF.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			FindClose(f);
			return true;
		}
	}
	FindClose(f);
	return false;
}

bool	jemmo_PathExists(const char *command_line)
{
	TCHAR fullpath[MAX_PATH];

	GetFullPathName(command_line, MAX_PATH, fullpath, NULL); 
	if (PathFileExists(fullpath)) {
		return true;
	}
	return false;
}

/* Разбор параметров коммандной строки */

void	jemmo_ParseCommandLine()
{
	command_line = GetCommandLineA();
	size_t command_length;

	char stopChar = ' ';

	if (command_line[0] == '\"') {
		stopChar = '\"';
		command_line++;
	}
	
	while (command_line[0] && command_line[0] != stopChar) 
		command_line++;
	command_line++;
	while (command_line[0] == ' ')
		command_line++;
	/* Теперь мы исключили пусть к исполняемому файлу с учетом того,
	   что он может быть заключен в кавычки. Далее может быть имя файла
	   или директории */
	if (command_line[0] == '\"') command_line++;
	command_length = lstrlen(command_line);
	if (command_line[command_length-1] == '\"') command_line[command_length-1] = 0;
	if (command_line[command_length-1] == '\\') command_line[command_length-1] = 0;
	/* Если есть такой файл, то открываем его, в противном случае предполагаем,
	   что в качестве параметра указана директория, тогда нужно прочитать ее и
	   составить список всех файлов */
	if (jemmo_PathExists(command_line))
	{
		if (jemmo_IsDirectory(command_line))
		{
			jemmo_GetDirectoryListing(command_line);
		} else {
			jemmo_OpenImage(command_line);
		}
	}
}

/* Получение списка файлов в директории */

void	jemmo_GetDirectoryListing(char *dirname)
{
	HANDLE f;
	_image_file_info * prev = NULL;
	char search_path[MAX_PATH];
	wsprintf(search_path, "%s\\%s", dirname, "*.jpg");
	f = FindFirstFile(search_path, &lpFindFileData);
	if (f != INVALID_HANDLE_VALUE)
	{
		do
		{
			ifi_cur = (_image_file_info *) jemmo_malloc(sizeof(__image_file_info));
			ifi_cur->prev = NULL; /* это первая запись */
			strcpy(&ifi_cur->FileName, lpFindFileData.cFileName);
			ifi_cur->source_format = frtJFIF;
			ifi_cur->pImage = NULL;
			if (prev != NULL) 
			{
				ifi_cur->prev = prev;
				ifi_cur->prev->next = ifi_cur;
			} else
			{
				ifi_first = ifi_cur;
				ifi_cur->prev = NULL;
			}
			prev = ifi_cur;
		}
		while (FindNextFile(f, &lpFindFileData) != 0);
		FindClose(f);
	}
}

void	jemmo_NextImage()
{
	
}

void	jemmo_PreviousImage()
{
}


void	jemmo_RButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HMENU	hPopupMenu;
	POINT pos;

	GetCursorPos(&pos);

	hPopupMenu = GetSubMenu(LoadMenu(hInst, (LPCTSTR)IDR_POPUPMENU), 0);
	TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,  pos.x, pos.y, 0, hWnd, NULL);
	DestroyMenu(hPopupMenu);
}

void	jemmo_UpdateWindowSize(HWND hWnd)
{
	if (current_image == NULL)
		return;
	int width = (int) current_image->width * current_image->zoom - 20;
	int height = (int) current_image->height * current_image->zoom - 20;

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
	// ToDo
	// Вынести GetSystemMetrics из процедуры
	RECT rLeft, rBottom, rTop, rRight, rect /* main window rect */;
	HDC hdc;
	PAINTSTRUCT ps;

	if (ifi_cur->pImage != NULL)
	{
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		// We have to fill 4 rects to paint a border around the image
		// Now we'll calculate them
		//rect.bottom -= GetSystemMetrics(SM_CYCAPTION);

		rLeft.left = 0;
		rLeft.top = 0;
		rLeft.bottom = rect.bottom;
		rLeft.right = (rect.right/2 - ifi_cur->pImage->width/2);
		FillRect(hdc, &rLeft, bgBrush);

		rRight.left = (rect.right/2 +ifi_cur->pImage->width/2);
		rRight.top = 0;
		rRight.bottom = rect.bottom;
		rRight.right = rect.right;
		FillRect(hdc, &rRight, bgBrush);

		rTop.left = 0;
		rTop.top = 0;
		rTop.bottom = (rect.bottom/2 - ifi_cur->pImage->height/2);// + GetSystemMetrics(SM_CYCAPTION);
		rTop.right = rect.right;
		FillRect(hdc, &rTop, bgBrush);

		rBottom.left = 0;
		rBottom.top = (rect.bottom/2 + ifi_cur->pImage->height/2);
		rBottom.bottom = rect.bottom;
		rBottom.right = rect.right;
		FillRect(hdc, &rBottom, bgBrush);

		EndPaint(hwnd, &ps);
	} else {
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, bgBrush);
		EndPaint(hwnd, &ps);
	}

	return 0;
}

int jemmo_AppInit()
{
	CreateSimpleToolbar(hwnd);
	CreateStatusBar();
	SetScrollRange(hwnd, SB_VERT, 0, 100, TRUE);
	SetScrollRange(hwnd, SB_HORZ, 0, 100, TRUE);
	
	bgBrush = CreateSolidBrush(0x0000);

	jemmo_ParseCommandLine();

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

void	jemmo_DrawImage(image *img)
{
	HDC hDc; BITMAPINFOHEADER bmiHeader;
	unsigned int left, top;
	unsigned char *tmp;
	/* первый раз hwnd почему-то равен 0, что приводит к отрисовке изображения на рабочем столе */
	if (hwnd == 0) return;
	if (img == NULL) {
		return;
	} else {
		hDc = GetDC(hwnd);
		if (hDc != NULL) {
			RECT rect;
			GetClientRect(hwnd, &rect);

			left = ((rect.right - rect.left)/2 - img->width/2);
			top  = ((rect.bottom - rect.top)/2 - img->height/2);

			tmp = img->aligned_data; //MakeDwordAlignedBuf(img->data, img->width, img->height, &m_widthDW);

			bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmiHeader.biWidth = img->width;
			bmiHeader.biHeight = img->height;
			bmiHeader.biPlanes = 1;
			bmiHeader.biBitCount = 24;
			bmiHeader.biCompression = BI_RGB;
			bmiHeader.biSizeImage = 0;
			bmiHeader.biXPelsPerMeter = 0;
			bmiHeader.biYPelsPerMeter = 0;
			bmiHeader.biClrUsed = 0;
			bmiHeader.biClrImportant = 0;
			
			// В StretchDIBits указываем ширину и высоту изображения, соответствующкую
			// масштабу 1:1, т.к. судя по всему, эта функция довольно плохо масштабирует
			// В дальнейшем попробуем использовать алгоритм Lanczos
			//StretchDIBits(hDc, 0, 0, img->width, img->height, 0, 0, img->width, img->height, tmp, (LPBITMAPINFO)&bmiHeader,
			//	DIB_RGB_COLORS, SRCCOPY);
			SetStretchBltMode(hDc, COLORONCOLOR);
			SetBrushOrgEx(hDc, 0, 0, NULL);
			StretchDIBits(hDc, left, top, img->width, img->height, 0, 0, img->width, img->height, tmp, (LPBITMAPINFO)&bmiHeader,
				DIB_RGB_COLORS, SRCCOPY);

			ReleaseDC(hwnd, hDc);
		} else {
			jemmo_Error("hDC is null");
		}
	} // if
}

void	jemmo_Error(char *msg)
{
	MessageBox(hwnd, msg, "Error", MB_ICONERROR);
}

int		jemmo_OpenImage(__image_file_info * jemmo_image)
{
	char str[MAX_PATH];
//	ifi_cur = (__image_file_info *) jemmo_malloc(sizeof(__image_file_info *));

	jemmo_image->pImage = jemmo_LoadImage(jemmo_image->FileName);
	if (jemmo_image->pImage == NULL) {
		jemmo_Error("Error loading image");
	} else {
		sprintf(str, "Image size: %d x %d, size: %d KB", jemmo_image->pImage->width,
				jemmo_image->pImage->height, _msize(jemmo_image->pImage->data)/1024);
	
	jemmo_DrawImage(jemmo_image->pImage);
	jemmo_UpdateWindowSize(hwnd);
	ifi_cur = jemmo_image;
	}

	return 0;
}