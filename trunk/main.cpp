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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "jemmo_jpeg.h"
//#include "jpeglib/jpeglib.h"

BOOL RegClass(WNDPROC, LPCSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DrawImage();

int TestJPEG();

void ErrorMessage(char *msg);

HINSTANCE hInst;
char szClassName[] = "WindowAppClass";
jvirt_sarray_ptr m1;
HWND hwnd;

// Глобальные переменные-атрибуты загруженного изображения

unsigned int m_width;
unsigned int m_height;
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
			MessageBox(NULL, "Hello", "Caption", MB_ICONINFORMATION);
			TestJPEG();
		}
//	case WM_PAINT:
//		{
//			if (image_data != NULL)
//				DrawImage();
//		}
//	case WM_SIZE:
//		{
//			if (image_data != NULL)
//				DrawImage();
//		}

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int TestJPEG()
{
	unsigned int x, y; char msg[100];

	image_data = read_jpeg("./testimg2.jpg", &x, &y);
	wsprintf(msg, "Image size: %d x %d", x, y);
	m_width = x; m_height = y;
	MessageBox(NULL, msg, "OK!", MB_ICONINFORMATION);
	BGRFromRGB(image_data, x, y);
	VertFlipBuf(image_data, x*3, y);
	DrawImage();
	return 0;
}

void ErrorMessage(char *msg)
{
	MessageBox(NULL, msg, "Error", MB_ICONERROR);
}

/*
	DrawImage
*/

void DrawImage()
{
	HDC hDc; BITMAPINFOHEADER bmiHeader;
	unsigned char *tmp;
	unsigned int left; unsigned int top;
	char msg[255];

	if (image_data == NULL) {
		MessageBox(NULL, "image_data is null", "Error", MB_ICONERROR);
	} else {
		hDc = GetDC(hwnd);
		if (hDc != NULL) {
			RECT rect;
			GetClientRect(hwnd, &rect);

			left = (rect.right - rect.left - m_width) / 2;
			top  = (rect.top - rect.bottom - m_height) / 2;

			tmp = MakeDwordAlignedBuf(image_data, m_width, m_height, &m_widthDW);

			bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmiHeader.biWidth = m_width;
			bmiHeader.biHeight = m_height;
			bmiHeader.biPlanes = 1;
			bmiHeader.biBitCount = 24;
			bmiHeader.biCompression = BI_RGB;
			bmiHeader.biSizeImage = 0;
			bmiHeader.biXPelsPerMeter = 0;
			bmiHeader.biYPelsPerMeter = 0;
			bmiHeader.biClrUsed = 0;
			bmiHeader.biClrImportant = 0;
			
			wsprintf(msg, "Width: %d, Height: %d", m_width, m_height);
			MessageBox(NULL, msg, "Info", MB_ICONINFORMATION);

			StretchDIBits(hDc, 0, 0, m_width, m_height, 0, 0, m_width, m_height, tmp, (LPBITMAPINFO)&bmiHeader,
				DIB_RGB_COLORS, SRCCOPY);

			ReleaseDC(hwnd, hDc);
		} else {
			ErrorMessage("hDC is null");
		}
	} // if
}
