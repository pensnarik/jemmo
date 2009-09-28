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
#include <setjmp.h>
#include <math.h>

#include "jpeglib/jpeglib.h"

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

BOOL RegClass(WNDPROC, LPCSTR, UINT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
unsigned char *read_jpeg(const char *filename, unsigned int *width, unsigned int *height);
void DrawImage();
unsigned char *MakeDwordAlignedBuf(unsigned char *dataBuf,
								   unsigned int widthPix,				// pixels!!
								   unsigned int height,
								   unsigned int *uiOutWidthBytes);		// bytes!!!
METHODDEF(void) my_error_exit (j_common_ptr cinfo);

void j_putGrayScanlineToRGB(BYTE *jpegline, 
							 int widthPix,
							 BYTE *outBuf,
							 int row);
void j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row);
BOOL VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height);
BOOL BGRFromRGB(BYTE *buf, UINT widthPix, UINT height);

int TestJPEG();

void ErrorMessage(char *msg);

HINSTANCE hInst;
char szClassName[] = "WindowAppClass";
jvirt_sarray_ptr m1;
HWND hwnd;

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

// ���������� ����������-�������� ������������ �����������

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

BOOL BGRFromRGB(BYTE *buf, UINT widthPix, UINT height)
{
	if (buf==NULL)
		return FALSE;

	UINT col, row;
	for (row=0;row<height;row++) {
		for (col=0;col<widthPix;col++) {
			LPBYTE pRed, pGrn, pBlu;
			pRed = buf + row * widthPix * 3 + col * 3;
			pGrn = buf + row * widthPix * 3 + col * 3 + 1;
			pBlu = buf + row * widthPix * 3 + col * 3 + 2;

			// swap red and blue
			BYTE tmp;
			tmp = *pRed;
			*pRed = *pBlu;
			*pBlu = tmp;
		}
	}
	return TRUE;
}

BOOL VertFlipBuf(BYTE  * inbuf, 
					   UINT widthBytes, 
					   UINT height)
{   
	BYTE  *tb1;
	BYTE  *tb2;

	if (inbuf==NULL)
		return FALSE;

	UINT bufsize;

	bufsize=widthBytes;

	tb1= (BYTE *)new BYTE[bufsize];
	if (tb1==NULL) {
		return FALSE;
	}

	tb2= (BYTE *)new BYTE [bufsize];
	if (tb2==NULL) {
		delete [] tb1;
		return FALSE;
	}
	
	UINT row_cnt;     
	ULONG off1=0;
	ULONG off2=0;

	for (row_cnt=0;row_cnt<(height+1)/2;row_cnt++) {
		off1=row_cnt*bufsize;
		off2=((height-1)-row_cnt)*bufsize;   
		
		memcpy(tb1,inbuf+off1,bufsize);
		memcpy(tb2,inbuf+off2,bufsize);	
		memcpy(inbuf+off1,tb2,bufsize);
		memcpy(inbuf+off2,tb1,bufsize);
	}	

	delete [] tb1;
	delete [] tb2;

	return TRUE;
}        


/*
	read_jpeg
*/

unsigned char *
read_jpeg(const char *filename, unsigned int *width, unsigned int *height)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	unsigned char *dataBuf;

	FILE * infile = NULL;		/* source file */

	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */
	char buf[250];
	*width=0;
	*height=0;

	if ((infile = fopen(filename, "rb")) == NULL) {
		sprintf(buf, "JPEG :\nCan't open %s\n", filename);
		MessageBox(NULL, buf, "msg", MB_ICONERROR);
		return NULL;
	}

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	
	int a = 0;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);

		if (infile!=NULL)
			fclose(infile);
		return NULL;
	}
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);

	(void) jpeg_read_header(&cinfo, TRUE);
	cinfo.scale_num = 1;
	cinfo.scale_denom =2;
	
	(void) jpeg_start_decompress(&cinfo);

	dataBuf=(unsigned char *) malloc(cinfo.output_width * 3 * cinfo.output_height);
	if (dataBuf==NULL) {

		MessageBox(NULL, "Out of memory", "msg", MB_ICONERROR);

		jpeg_destroy_decompress(&cinfo);
		
		fclose(infile);

		return NULL;
	}

	*width = cinfo.output_width;
	*height = cinfo.output_height;
	
	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		if (cinfo.out_color_components==3) {
			
			j_putRGBScanline(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		} else if (cinfo.out_color_components==1) {

			j_putGrayScanlineToRGB(buffer[0], 
								*width,
								dataBuf,
								cinfo.output_scanline-1);

		}

	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return dataBuf;
}
 

void j_putGrayScanlineToRGB(BYTE *jpegline, 
							 int widthPix,
							 BYTE *outBuf,
							 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) {

		BYTE iGray;

		// get our grayscale value
		iGray = *(jpegline + count);

		*(outBuf + offset + count * 3 + 0) = iGray;
		*(outBuf + offset + count * 3 + 1) = iGray;
		*(outBuf + offset + count * 3 + 2) = iGray;
	}
}

void j_putRGBScanline(BYTE *jpegline, 
					 int widthPix,
					 BYTE *outBuf,
					 int row)
{
	int offset = row * widthPix * 3;
	int count;
	for (count=0;count<widthPix;count++) 
	{
		*(outBuf + offset + count * 3 + 0) = *(jpegline + count * 3 + 0);
		*(outBuf + offset + count * 3 + 1) = *(jpegline + count * 3 + 1);
		*(outBuf + offset + count * 3 + 2) = *(jpegline + count * 3 + 2);
	}
}

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;

	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Always display the message. */
	MessageBox(NULL,buffer,"JPEG Fatal Error",MB_ICONSTOP);


	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
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
 

unsigned char *MakeDwordAlignedBuf(unsigned char *dataBuf,
								   unsigned int widthPix,				// pixels!!
								   unsigned int height,
								   unsigned int *uiOutWidthBytes)		// bytes!!!
{
	unsigned int uiWidthBytes; unsigned long dwNewsize;
	unsigned char *pNew; unsigned int uiInWidthBytes; unsigned int uiCount;

	if (dataBuf==NULL)
		return NULL;

	uiWidthBytes = WIDTHBYTES(widthPix * 24);

	dwNewsize=(DWORD)((DWORD)uiWidthBytes * 
							(DWORD)height);
	
	pNew=(unsigned char *)malloc(dwNewsize);
	if (pNew==NULL) {
		return NULL;
	}
	
	uiInWidthBytes = widthPix * 3;
	
	for (uiCount=0;uiCount < height;uiCount++) {
		unsigned char * bpInAdd;
		unsigned char * bpOutAdd;
		unsigned long lInOff;
		unsigned long lOutOff;

		lInOff=uiInWidthBytes * uiCount;
		lOutOff=uiWidthBytes * uiCount;

		bpInAdd= dataBuf + lInOff;
		bpOutAdd= pNew + lOutOff;

		memcpy(bpOutAdd,bpInAdd,uiInWidthBytes);
	}

	*uiOutWidthBytes=uiWidthBytes;
	return pNew;
}