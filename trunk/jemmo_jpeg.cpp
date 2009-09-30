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

#include "jemmo_jpeg.h"

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
//	cinfo.scale_num = 1;
//	cinfo.scale_denom = 1;
	
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
 

void j_putGrayScanlineToRGB(BYTE *jpegline, int widthPix, BYTE *outBuf, int row)
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

void j_putRGBScanline(BYTE *jpegline, int widthPix, BYTE *outBuf, int row)
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

BOOL VertFlipBuf(BYTE * inbuf, UINT widthBytes, UINT height)
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

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message) (cinfo, buffer);
	MessageBox(NULL,buffer, "JPEG Fatal Error", MB_ICONSTOP);
	longjmp(myerr->setjmp_buffer, 1);
}

