#pragma once //Used so function does not repeat itself
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

//for 4 byte 
void write_4_byte(ofstream& bitmap_file, const uint bit)
{
	bitmap_file.put((bit >> 0) & 0xFF);
	bitmap_file.put((bit >> 8) & 0xFF);
	bitmap_file.put((bit >> 16) & 0xFF);
	bitmap_file.put((bit >> 24) & 0xFF);
}

//for 2 byte
void write_2_byte(ofstream& bitmap_file, const uint bit)
{
	bitmap_file.put((bit >> 0) & 0xFF);
	bitmap_file.put((bit >> 8) & 0xFF);
}

void write_jpg_to_bmp(const JPEG_header* const header, const MCU_array* const mcus, const string& filename)
{
	cout << "Converting JPEG image to a Bitmap image." << endl;

	ofstream bitmap_file = ofstream(filename, ios::out | ios::binary); //Create the file which will store the bitmap image

	if (!bitmap_file.is_open()) //Check if file was opened correctly 
	{
		cout << "The Bitmap image file was not able to be opened." << endl;
		return;
	}

	const uint MCU_height = (header->jpeg_height + 7) / 8;
	const uint MCU_width = (header->jpeg_width + 7) / 8;
	const uint padding_size = header->jpeg_width % 4;
	const uint bitmap_file_size = 14 + 12 + header->jpeg_height * header->jpeg_width * 3 + padding_size * header->jpeg_height;

	//Set the Bitmap Header
	bitmap_file.put('B'); //Signature
	bitmap_file.put('M'); //Signature
	write_4_byte(bitmap_file, bitmap_file_size); //Write the file size
	write_4_byte(bitmap_file, 0); //Unused values
	write_4_byte(bitmap_file, 0x1A); //Offset from beginning of file to the beginning of the bitmap data
	//Info Header
	write_4_byte(bitmap_file, 12); //Size of the bitmap info header
	write_2_byte(bitmap_file, header->jpeg_width); //Pixel width of bitmap
	write_2_byte(bitmap_file, header->jpeg_height); //Pixel height of bitmap
	write_2_byte(bitmap_file, 1); //Number of planes
	write_2_byte(bitmap_file, 24); //Number of bits per pixel

	//Now write the rows, for this we need to start from max value and work down or else the image will be upside down
	//The same goes for the colors instead of RGB we do BGR
	for (uint y = header->jpeg_height - 1; y < header->jpeg_height; --y)
	{ 
		const uint row = y / 8;
		const uint pixel_row = y % 8;

		for (uint x = 0; x < header->jpeg_width; ++x)
		{
			const uint column = x / 8;
			const uint pixel_column = x % 8;
			const uint index = row * MCU_width + column;
			const uint pixel_index = pixel_row * 8 + pixel_column;

			bitmap_file.put(mcus[index].b[pixel_index]);
			bitmap_file.put(mcus[index].g[pixel_index]);
			bitmap_file.put(mcus[index].r[pixel_index]);
		}

		//for padding bytes
		for (uint i = 0; i < padding_size; ++i)
		{
			bitmap_file.put(0);
		}
	}

	bitmap_file.close();
}
