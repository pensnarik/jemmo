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

#ifndef INC_JEMMO_IMAGE
#define INC_JEMMO_IMAGE

typedef enum _source_format {
	frtJFIF,
	frtPNG,
	frtBMP
} __source_format;

typedef struct _image {
	int width;
	int height;
	__source_format source_format;
	float zoom;
	unsigned int size;
	unsigned char *data;
	unsigned char *aligned_data;
} image;

__source_format jemmo_GetImageFormat(const char *FileName);
image *			jemmo_LoadImage(const char *FileName);
unsigned char * jemmo_LoadJpegImage();
image *			jemmo_CloneImage(image *img);
unsigned char  *MakeDwordAlignedBuf(unsigned char *dataBuf, unsigned int widthPix, unsigned int height, unsigned int *uiOutWidthBytes);


#endif //INC_JEMMO_IMAGE