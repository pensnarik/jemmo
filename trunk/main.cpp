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
#include <windowsx.h>
#include <WinUser.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <CommCtrl.h> // requires Microsoft SDK
#include <malloc.h>

#include "main.h"
#include "resource.h"

#include "jemmo_jpeg.h"
#include "jemmo_image.h"
#include "jemmo_main.h"

BOOL RegClass(WNDPROC, LPCSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL get_module_directory(TCHAR *obuf, size_t osize);

int TestJPEG(LPSTR FileName);

void ErrorMessage(char *msg);

extern HINSTANCE hInst;
char szClassName[] = "WindowAppClass";
jvirt_sarray_ptr m1;
extern HWND hStatusBar;
extern image *current_image;
extern HWND hwnd;


// Глобальные переменные-атрибуты загруженного изображения

unsigned int m_widthDW;

unsigned char *image_data;

/*
	WinMain
*/

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg; 

	if (!RegClass(WndProc, szClassName, COLOR_WINDOW)) return FALSE;
	hwnd = CreateWindow(szClassName,
						"Jemmo",
						WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
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
//	wc.style = wc.cbClsExtra = wc.cbWndExtra = 0;
	wc.cbClsExtra =0;
	wc.cbWndExtra = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(msg)
	{
	case WM_ERASEBKGND:		// Important!
		return 1;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			return 0;
			char str[255];
			current_image = jemmo_LoadImage("testimg.jpg");
			if (current_image == NULL) {
				MessageBox(NULL, "Error loading image", "Error", MB_ICONERROR);
			} else {
				sprintf(str, "Image size: %d x %d, size: %d KB", current_image->width,
						current_image->height, _msize(current_image->data)/1024);
			
				//MessageBox(NULL, str, "Jemmo", MB_ICONINFORMATION);
				jemmo_DrawImage(current_image);
				jemmo_UpdateWindowSize(hwnd);
			}
			return 0;

		}
	case WM_RBUTTONDOWN:
			jemmo_RButtonDown(hWnd, wParam, lParam);
			return 0;
	case WM_CREATE:
		{
			jemmo_AppInit();
			return 0;
		}

	case WM_SIZE:
		{
			SendMessage(hStatusBar, WM_SIZE, wParam, lParam);
			return 0;
		}
	case WM_PAINT:
		{
			jemmo_MainWindowRepaint();
			jemmo_DrawImage(current_image);
			UpdateWindow(hStatusBar);
			return 0;
		}
	case 0x020a:
		{
			MessageBox(NULL, "Wheel!", "test", MB_ICONINFORMATION);
			return 0;
		}
	case WM_VSCROLL:
		{
			int tmp = GetScrollPos(hWnd, SB_VERT);
			switch(LOWORD(wParam)) {
			case SB_LINEUP:
				tmp = max(0, tmp - 10);
			case SB_LINEDOWN:
				tmp += 10;
			}
			SetScrollPos(hWnd, SB_VERT, tmp, FALSE);
			return 0;
		}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MNU_NEXT:
			jemmo_NextImage();
			return 0;
		case MNU_PREV:
			jemmo_PreviousImage();
			return 0;
		}
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
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
