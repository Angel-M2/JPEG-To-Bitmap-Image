#pragma once //Done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

//Dequantize based on quantization table
void DequantizeSingleMCUComponent(const quantization_table_struct& quantization_table, int* const MCU_component)
{
	for (uint i = 0; i < 64; ++i)//Loop through each value in the array
	{
		//All the values simply need to be multipled by the quantization table value in that same index to get the dequantized value
		MCU_component[i] = MCU_component[i] * quantization_table.q_table[i];
	}
}

//Dequantize each color channel in the MCU
void DequantizeMCU(const JPEG_header* const jpgHeader, MCU_array* const New_mcu)
{
	cout << "Performing dequantization of MCU." << endl;

	const uint MCU_height = (jpgHeader->jpeg_height + 7) / 8; //Get the MCU heigth
	const uint MCU_width = (jpgHeader->jpeg_width + 7) / 8; //Get the MCU width

	for (uint i = 0; i < MCU_height * MCU_width; ++i)//Loop for each MCU
	{
		for (uint j = 0;j < jpgHeader->jpeg_component_number;++j) //In each MCU loop through each color components
		{
			/*We now need to dequantize each MCU component one by one
			* we need to pass the quantization table that each color component is using thus we get the index of the table that the current color component is using
			* and then pass the MCU from main as well*/
			DequantizeSingleMCUComponent(jpgHeader->q_table[jpgHeader->colorComponents[j].quantization_table_index], New_mcu[i][j]);
		}
	}
}//End of function DequantizeMCU