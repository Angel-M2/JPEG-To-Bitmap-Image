#pragma once
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

//Convert all of the pixels in the current MCU from YCbCr to RGB 
void convertMCU(MCU_array& pixel)
{
	for (int i = 0; i < 64; ++i) //Loop through all of the pixels in current MCU
	{
		int r = pixel.y[i] + 1.402f * pixel.cr[i] + 128; //Calculate the amount of red for this pixel
		int g = pixel.y[i] - 0.344f * pixel.cb[i] - 0.714f * pixel.cr[i] + 128; //Calculate teh amount of green for this pixel
		int b = pixel.y[i] + 1.772f * pixel.cb[i] + 128; //Calculate the amount of red for this pixel

		if (r < 0) r = 0; //Make sure the value is within bounds
		if (r > 255) r = 255; //Make sure the value is within bounds
		pixel.r[i] = r; //Store the amount of red for current pixel

		if (g < 0) g = 0; //Make sure the value is within bounds
		if (g > 255) g = 255; //Make sure the value is within bounds
		pixel.g[i] = g; //Store the amount of green for current pixel

		if (b < 0) b = 0; //Make sure the value is within bounds
		if (b > 255) b = 255; //Make sure the value is within bounds
		pixel.b[i] = b; //Store the amount of blue for current pixel
	}
}

//Convert all of the pixels from the YCbCr color space to the RGB
void colorConversion(const JPEG_header* const header, MCU_array* const mcus)
{
	cout << "Performing Color Conversion." << endl;

	int MCUH = (header->jpeg_height + 7) / 8; //Define the height of the MCU
	int MCUW = (header->jpeg_width + 7) / 8; //Define the width of the MCU

	for (int i = 0; i < MCUH * MCUW; ++i) //Loop through all of the MCUS
	{
		convertMCU(mcus[i]); //Get the pixel data of each mcu in RGB
	}
}
