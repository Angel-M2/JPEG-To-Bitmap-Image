#pragma once
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
#include "Dequantization.h"
#include "ColorConversion.h"
using namespace std;

//Calculates the sums of each pixel in the current mcu
void IDCTmcus(int* const currentMCU)
{
	int result[64] = { 0 }; //Array to store all of the sums

	for (int y = 0; y < 8; ++y) //Loop through all of the pixels in the current mcu
	{
		for (int x = 0; x < 8; ++x) //Loop through all of the pixels in the current mcu
		{
			double sum = 0; //For calculating the sum of each pixel

			for (int i = 0; i < 8; ++i) //Loop through all DCT coefficients
			{
				for (int j = 0; j < 8; ++j) //Loop through all DCT coefficients
				{
					double n = 1;
					double m = 1;

					if (i == 0) //For the first i
					{
						n = 1 / sqrt(2); //Determine m as
					}

					if (j == 0) //For the first j
					{
						m = 1 / sqrt(2); //Determine n as
					}

					sum += n * m * currentMCU[i * 8 + j] * cos((2 * x + 1) * j * PI / 16) * cos((2 * y + 1) * i * PI / 16); //Calculate the total sum each time
				}
			}

			sum /= 4; //Divide the sum by 4
			result[y * 8 + x] = (int)sum; //Store the sum in the result array
		}
	}

	for (int i = 0; i < 64; ++i) //Loop through all of the MCUs
	{
		currentMCU[i] = result[i]; //Store all of the values from the result array to the currentMCU array
	}
}

// Perform IDCT on all MCUs
void IDCT(const JPEG_header* const header, MCU_array* const mcus)
{
	cout << "Performing Inverse Discrete Cosine Transform." << endl;
	const int MCUH = (header->jpeg_height + 7) / 8; //Define the height of the MCU
	const int MCUW = (header->jpeg_width + 7) / 8; //Define the width of the MCU

	for (int i = 0; i < MCUH * MCUW; ++i) //Loop through every MCU
	{
		for (int j = 0; j < header->jpeg_component_number; ++j) //Loop through every pixel in each MCU
		{
			//DequantizeSingleMCUComponent(header->quantizationTables[header->colorComponents[j].quantizationTableID], mcus[i][j]); //Dequantize the current MCU
			IDCTmcus(mcus[i][j]); //Perform IDCT on the current MCU
		}
	}
}