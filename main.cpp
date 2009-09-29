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
    along with Jemmo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include <WinUser.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <CommCtrl.h> // requires Microsoft SDK

#include "main.h"

#include "jemmo_jpeg.h"
#include "jemmo_image.h"
#include "jemmo_main.h"

BOOL RegClass(WNDPROC, LPCSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL get_module_directory(TCHAR *obuf, size_t osize);

void DrawImage(image *img);

int TestJPEG(LPSTR FileName);

void ErrorMessage(char *msg);

HINSTANCE hInst;
char szClassName[] = "WindowAppClass";
jvirt_sarray_ptr m1;
HWND hwnd;
extern image *current_image;

// ���������� ����������-�������� ������������ �����������

unsigned int m_widthDW;

unsigned char *image_data;
LPSTR	command_line;

/*
	WinMain
*/

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg; 
	
	command_line = lpCmdLine;

	if (!RegClass(WndProc, szClassName, COLOR_WINDOW)) return FALSE;
	hwnd = CreateWindow(szClassName,
						"FastJPEG",
						WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						0,
						0,
						hInstance,
						NULL);
	if (!hwnd) return FALSE;
	while (GetMessage(&msg, 0, 0, 0))
		DispatchMessage(&msg);
	return msg.wParam;
}

/*
	RegClass
*/

BOOL RegClass(WNDPROC proc,
			  LPCTSTR szName,
			  UINT brBackground)
{
	WNDCLASS wc;
	wc.style = wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(brBackground + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szName;
	return (RegisterClass(&wc) != 0);
}


/*
	WndProc
*/

LRESULT CALLBACK WndProc(HWND hwnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
/*			char buf[100]; char full_path[200];
			get_module_directory(buf, 100);
			sprintf(full_path, "%s\\%s", buf, command_line);
			TestJPEG(full_path);
			if (_access(full_path, 0)) {
				TestJPEG(full_path);
			}
*/
			char str[255];
			current_image = jemmo_LoadImage("testimg.jpg");
			if (current_image == NULL) {
				MessageBox(NULL, "Error loading image", "Error", MB_ICONERROR);
			} else {
				sprintf(str, "Image size: %d x %d", current_image->width, current_image->height);
				MessageBox(NULL, str, "OK", MB_ICONINFORMATION);
				DrawImage(current_image);
				jemmo_UpdateWindowSize(hwnd);
			}

		}
	case WM_CREATE:
		{
			CreateSimpleToolbar(hwnd);
		}
	case 0x020a:
		{
//			MessageBox(NULL, "Wheel!", "test", MB_ICONINFORMATION);
		}

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL get_module_directory(TCHAR *obuf, size_t osize)  
{  
    if ( ! GetModuleFileName(0, obuf, osize) )  
    {  
        *obuf = '\0';// insure it's NUL terminated  
        return FALSE;  
    }  
   
    // run through looking for the *last* slash in this path.  
    // if we find it, NUL it out to truncate the following  
    // filename part.  
   
    TCHAR*lastslash = 0;  
   
    for ( ; *obuf; obuf ++)  
    {  
        if ( *obuf == '\\' || *obuf == '/' )  
            lastslash = obuf;  
    }  
   
    if ( lastslash ) *lastslash = '\0';  
   
    return TRUE;  
} 

void ErrorMessage(char *msg)
{
	MessageBox(NULL, msg, "Error", MB_ICONERROR);
}

/*
	DrawImage
*/

void DrawImage(image *img)
{
	HDC hDc; BITMAPINFOHEADER bmiHeader;
	unsigned int left, top;
	unsigned char *tmp;
	if (img == NULL) {
		MessageBox(NULL, "image_data is null", "Error", MB_ICONERROR);
	} else {
		hDc = GetDC(hwnd);
		if (hDc != NULL) {
			RECT rect;
			GetClientRect(hwnd, &rect);

			left = (rect.right - rect.left - img->width) / 2;
			top  = (rect.top - rect.bottom - img->height) / 2;

			tmp = MakeDwordAlignedBuf(img->data, img->width, img->height, &m_widthDW);

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
			
			// � StretchDIBits ��������� ������ � ������ �����������, ����������������
			// �������� 1:1, �.�. ���� �� �����, ��� ������� �������� ����� ������������
			// � ���������� ��������� ������������ �������� Lanczos
			StretchDIBits(hDc, 0, 0, img->width, img->height, 0, 0, img->width, img->height, tmp, (LPBITMAPINFO)&bmiHeader,
				DIB_RGB_COLORS, SRCCOPY);

			ReleaseDC(hwnd, hDc);
		} else {
			ErrorMessage("hDC is null");
		}
	} // if
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