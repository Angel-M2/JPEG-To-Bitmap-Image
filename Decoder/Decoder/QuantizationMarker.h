#pragma once //done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

void setQuantizationTable(ifstream& jpg_image, JPEG_header* const jpgHeader)
{
	cout << "Starting to read Quatization Markers (DQT)." << endl;

	int header_length = (jpg_image.get() << 8) + jpg_image.get(); //length needs to be int in this case because we want to know if it goes below zero in the while loop
	header_length = header_length - 2;

	//Loop to get information for each quantization table
	while (header_length > 0)
	{
		byte quantization_precision = jpg_image.get(); //Get the table precision

		header_length = header_length - 1;

		byte table_index = quantization_precision & 0x0F; //The table index is stored in the last 4 bits of the table precision

		jpgHeader->q_table[table_index].used = true; //This table is being used

		//The quantization tables can either be 16 bit or 64 bit
		if (quantization_precision >> 4 != 0)  //For a 16 bit table we check the most significant digits if it is not 0 then we have a 16 bit table
		{
			for (uint i = 0; i < 64; ++i) 
			{
				jpgHeader->q_table[table_index].q_table[zigZagMap[i]] = (jpg_image.get() << 8) + jpg_image.get(); //look in the zig zag map, read 2 byte
			}

			header_length -= 128;
		}
		else 
		{
			for (uint i = 0; i < 64; ++i) //If it is not 16 bit then it can only be 64 bit
			{
				jpgHeader->q_table[table_index].q_table[zigZagMap[i]] = jpg_image.get(); //look in the zig zag map, read 1 byte
			}

			header_length -= 64;
		}
	}
}
