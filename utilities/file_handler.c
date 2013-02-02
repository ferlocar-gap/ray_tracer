/* file_handler.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * This program writes an array of Color structures into a .bmp image and
 * manages all the file related operations of the ray tracer.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "memory_handler.h"
#include "../tracing/color.h"

// Structures and constants
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t LONG;

typedef struct tagBMPFileHeader
{
	DWORD bfSize; // bytes of entire file
	WORD bfReserved1; // nothing
	WORD bfReserved2; // nothing
	DWORD bfOffBits; // address of bitmap data in file
} BMPFileHeader;

typedef struct tagBMPInfoHeader
{
	DWORD biSize; // Size of this BMPInfoHeader
	int32_t biWidth;
	int32_t biHeight;
	WORD biPlanes; // 1 plane, so this equals 1
	WORD biBitCount; // 24 bits/pixel
	DWORD biCompression; // 0 = BMP
	DWORD biSizeImage; // size of actual bitmap image
	int32_t biXPelsPerMeter; // Horizontal resolution
	int32_t biYPelsPerMeter; // Vertical resolution
	DWORD biClrUsed; // 0
	DWORD biClrImportant; // 0 important colors (very old)
} BMPInfoHeader;

// Methods

/*
 * Writes an image with the 'image.bmp' name in the root of the program.
 * The image will have a resolution of 'width' * 'height'.
 *
 * pixels: Pixels of the image. Length of the array must be height * width.
 * height: Number of pixel rows on the image.
 * width: Number of pixel columns on the image.
 */
void create_image (Color * pixels, int height, int width)
{
    // Prepare image headers
	BMPFileHeader bitmap_file_header;
	BMPInfoHeader bitmap_info_header;
	int imagebytes=width*height*3;
	char magic[2]="BM";
	bitmap_file_header.bfSize = 2+sizeof(BMPFileHeader)+sizeof(BMPInfoHeader)+imagebytes;
	bitmap_file_header.bfReserved1 = 0;
	bitmap_file_header.bfReserved2 = 0;
	bitmap_file_header.bfOffBits = 2+sizeof(BMPFileHeader)+sizeof(BMPInfoHeader);
	bitmap_info_header.biSize = sizeof(BMPInfoHeader);
	bitmap_info_header.biWidth = width;
	bitmap_info_header.biHeight = height;
	bitmap_info_header.biPlanes = 1;
	bitmap_info_header.biBitCount = 24; // 24 bits/pixel
	bitmap_info_header.biCompression = 0; // Zero is the defaut Bitmap
	bitmap_info_header.biSizeImage = imagebytes;
	bitmap_info_header.biXPelsPerMeter = 2835; // 72 pixels/inch = 2834.64567 pixels per meter
	bitmap_info_header.biYPelsPerMeter = 2835;
	bitmap_info_header.biClrUsed = 0;
	bitmap_info_header.biClrImportant = 0;
	// Prepare image data
	char *imagedata = get_memory(imagebytes, NULL);
	int x,y;
	#define B imagedata[((y*width)+x)*3]
	#define G imagedata[((y*width)+x)*3+1]
	#define R imagedata[((y*width)+x)*3+2]
	int color_counter = 0;
	for(y=0;y<height;++y)
	{
		for(x=0;x<width;++x)
		{
			R= (char) round(255*pixels[color_counter].red);
			G= (char) round(255*pixels[color_counter].green);
			B= (char) round(255*pixels[color_counter].blue);
			color_counter++;
		}
	}
	// Write file
	FILE *f = fopen("image.bmp","wb");
	fwrite(magic, 2, 1, f);
	fwrite((void*)&bitmap_file_header, sizeof(BMPFileHeader), 1, f);
	fwrite((void*)&bitmap_info_header, sizeof(BMPInfoHeader), 1, f);
	fwrite(imagedata, bitmap_info_header.biSizeImage, 1, f);
	fclose(f);
	free(imagedata);
}

