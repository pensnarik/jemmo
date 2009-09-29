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

/*
    Procedures and functions, working with Jemmo internal image format
   (decoded image)
*/

#include <windows.h>
#include "jemmo_image.h"
#include "jemmo_jpeg.h"

__source_format jemmo_GetImageFormat(const char *FileName)
{
	// stub
	return frtJFIF;
}

image *			jemmo_LoadImage(const char *FileName)
{
	if (jemmo_GetImageFormat(FileName) != frtJFIF) {
		return NULL;
	} else {
		/* JPEG */
		image *img = (image *) malloc(sizeof(_image));
		if(img == NULL) return NULL;
		unsigned int width, height;
		img->data = read_jpeg(FileName, &width, &height);
		img->width = width;
		img->height = height;
		img->zoom = 1;
		img->source_format = frtJFIF;
		img->size = width * 3 * height;
		BGRFromRGB(img->data, width, height);
		VertFlipBuf(img->data, width*3, height);
		return img;
	}
}

image *			jemmo_CloneImage(image *img)
{
	if (img == NULL) return NULL;
	image *tmp = (image *) malloc(sizeof(_image));
	if (tmp == NULL) return NULL;

	tmp->width = img->width;
	tmp->height = img->height;
	tmp->zoom = img->zoom;
	tmp->size = img->size;
	tmp->source_format = img->source_format;	// change to memcpy
	tmp->data = (unsigned char *) malloc(tmp->size);
	
	if (tmp->data == NULL) return NULL;
	memcpy(tmp->data, img->data, img->size);
	return tmp;
}
