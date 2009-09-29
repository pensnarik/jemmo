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


#ifndef _INC_JEMMO_JPEG
#define _INC_JEMMO_JPEG

#include <setjmp.h>
#include <windows.h>
#include <stdio.h>
#include "jpeglib/jpeglib.h"

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

unsigned char *read_jpeg(const char *filename, unsigned int *width, unsigned int *height);
unsigned char *MakeDwordAlignedBuf(unsigned char *dataBuf, unsigned int widthPix, unsigned int height, unsigned int *uiOutWidthBytes);
void j_putGrayScanlineToRGB(BYTE *jpegline, int widthPix, BYTE *outBuf, int row);
void j_putRGBScanline(BYTE *jpegline, int widthPix, BYTE *outBuf, int row);
BOOL VertFlipBuf(BYTE  * inbuf, UINT widthBytes, UINT height);
BOOL BGRFromRGB(BYTE *buf, UINT widthPix, UINT height);
METHODDEF (void) my_error_exit (j_common_ptr cinfo);

#endif //_INC_JEMMO_JPEG