#pragma once //Done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

//Helper class to read 
class HelperClassBitReader 
{
private:
	uint next_byte = 0;
	uint next_bit = 0;
	const vector<byte>& data;

public:
	HelperClassBitReader(const vector<byte>& d) :
		data(d)
	{}

	//read one bit
	int read_1_bit() 
	{
		if (next_byte >= data.size())
		{
			return -1;
		}

		int bit = (data[next_byte] >> (7 - next_bit)) & 1; //Bits will be read from most sig to least sig

		next_bit = next_bit+ 1;

		if (next_bit == 8)
		{
			next_bit = 0;
			next_byte = next_byte + 1;
		}

		return bit;
	}//End of read_1_bit

	//read multiple bits
	int read_multiple_bits(const uint length)
	{ 
		int bits = 0;

		for (uint i = 0; i < length; ++i) 
		{
			int bit = read_1_bit();

			if (bit == -1) {
				bits = -1;
				break;
			}

			bits = (bits << 1) | bit;
		}

		return bits;
	}//End of read_multiple_bits

	//This function is used to move to the 0 bit of the next byte when we have zero bits remaining
	void fix_position()
	{
		if (next_byte >= data.size())
		{
			return;
		}

		if (next_bit != 0) {//If we are at bit zero already then we do not need to fix the position
			next_bit = 0;
			next_byte = next_byte + 1;
		}
	}
};//End of HelperClassBitReader


//This function will return the huffman value for the bit read 
byte getHuffmanValue(HelperClassBitReader& helper_function, const huffman_table_struct& huffman_table)
{ 
	uint huffman_value = 0;

	for (uint i = 0; i < 16; ++i) 
	{
		int bit = helper_function.read_1_bit();
		if (bit == -1)
		{
			return -1;
		}

		huffman_value = (huffman_value << 1) | bit;

		//Lopp so we can compare running_code with the actual value stored in the array, if not we keep reading a new bit
		for (uint j = huffman_table.offset[i]; j < huffman_table.offset[i + 1];++j)
		{
			if (huffman_value == huffman_table.huffman_code[j])
			{
				return huffman_table.huffman_value[j];
			}
		}
	}
	return -1; //Return -1 if there is no match
}//End of getHuffmanValue

bool DecodeOneMCU(HelperClassBitReader& helper_function, int* const MCU_component, int& previous_DC_value, const huffman_table_struct& DC_table, const huffman_table_struct& AC_table)
{ 
	//get the DC value for this MCU component
	byte length = getHuffmanValue(helper_function, DC_table);

	int dc_coeff = helper_function.read_multiple_bits(length);

	if (length != 0 && dc_coeff < (1 << (length - 1)))
	{
		dc_coeff = dc_coeff - ((1 << length) - 1);
	}

	MCU_component[0] = dc_coeff + previous_DC_value; //DC coefficient needs to be added with the previous DC value
	previous_DC_value = MCU_component[0];

	uint i = 1;
	while (i < 64) //Proccess all the AC table values
	{
		byte value = getHuffmanValue(helper_function, AC_table);

		if (value == 0x00) //If we encounter this value we need to fill the rest of the values with zeros
		{
			while (i < 64)
			{
				MCU_component[zigZagMap[i]] = 0; //We go through the zigzagmap
				++i;
			}
			return true;
		}

		//If the value is not 0x00 we separare the value into 2 four bits
		byte skip_zeros = value >> 4; //Most sig bits are the number of zeros that need to be skipped
		byte coeffLength = value & 0x0F; //Least sig bits is the coeff length
		dc_coeff = 0;

		if (value == 0xF0) //If we get this value it means we need to skip 16 zeros
		{
			skip_zeros = 16;
		}

		for (uint j = 0;j < skip_zeros; ++j, ++i) //We place the number of to skip zeors into the MCU component
		{
			//i is the position of the MCU component, while j is the number of zeros we have copied
			MCU_component[zigZagMap[i]] = 0;
		}

		//After zeros we set the coefflength
		if (coeffLength != 0) 
		{
			dc_coeff = helper_function.read_multiple_bits(coeffLength); //read the bit

			if (dc_coeff < (1 << (coeffLength - 1)))
			{
				dc_coeff = dc_coeff - ((1 << coeffLength) - 1);
			}

			MCU_component[zigZagMap[i]] = dc_coeff; //Save the coeffient into the MCU
			i = i+ 1; //increment i
		}
	}

	return true;
}//End of function DecodeOneMCU

//This function will set the huffman data codes
void getHuffmanCode(huffman_table_struct& huffman_table)
{
	uint huffman_code = 0;

	for (uint i = 0; i < 16; ++i)
	{
		/*offset array gives us starting index of the current code, we then loop until the start index of the next code
		at this point we have all the current code so we move on to the next one*/
		for (uint j = huffman_table.offset[i]; j < huffman_table.offset[i + 1]; ++j) 
		{
			huffman_table.huffman_code[j] = huffman_code; //We now have to save the huffman code into an array
			huffman_code = huffman_code + 1; //The next step in getting the codes is to add one to the huffman code
		}

		huffman_code <<= 1; //When we are going to start working on a new huffman code we have to add a zero the code
	}
}//End of getHuffmanCode


//When decoding the huffman data we take in the JPEG header pointer and return a pointer to the start of the MCU
MCU_array* HuffmanDecoding(JPEG_header* const jpgHeader)
{ 
	cout << "Decoding Huffman Data." << endl;

	//Get the mcu height and width by adding 7 and then dividing by 8
	const uint mcuHeight = (jpgHeader->jpeg_height + 7) / 8;
	const uint mcuWidth = (jpgHeader->jpeg_width + 7) / 8;

	MCU_array* New_mcu = new (nothrow) MCU_array[mcuHeight * mcuWidth]; //Allocate memory for the mcu

	//For all the huffman tables if there are initialized we then need to get the codes of each table
	for (uint i = 0; i < 4;++i)
	{
		if (jpgHeader->huffman_DC_table[i].used)
		{
			getHuffmanCode(jpgHeader->huffman_DC_table[i]);
		}

		if (jpgHeader->huffman_AC_table[i].used)
		{
			getHuffmanCode(jpgHeader->huffman_AC_table[i]);
		}
	}//end of for loop

	//Create instance of HelperClassBitReader class
	HelperClassBitReader helper_function(jpgHeader->huffmanData);
	int previous_DC_value[3] = { 0 }; 

	for (uint i = 0; i < mcuHeight * mcuWidth; ++i) //The MCU will be the new height * width so we loop for that amount
	{
		//This should happen after 4 MCU
		if (jpgHeader->restart_interval != 0 && i % jpgHeader->restart_interval == 0)  //Check if there is a restart interval if there is we need to reset dc values
		{
			previous_DC_value[0] = 0;
			previous_DC_value[1] = 0;
			previous_DC_value[2] = 0;
			/*In order to keep reading the bits correctly we need a function that will fix the position and move us to the start of the next line we are going to begin reading*/
			helper_function.fix_position();
		}

		for (uint j = 0; j < jpgHeader->jpeg_component_number; ++j)
		{
			/*process one channel of MCU
			We need to address the r,g,b components thus use the overloaded operator function in the struct*/
			if (!DecodeOneMCU(helper_function, New_mcu[i][j], previous_DC_value[j], jpgHeader->huffman_DC_table[jpgHeader->colorComponents[j].huffman_DC_table_id], jpgHeader->huffman_AC_table[jpgHeader->colorComponents[j].huffman_AC_table_id]))
			{
				delete[] New_mcu; //There was an error so we delete the MCU created
				return nullptr; //Return NULL if there was an error
			}
		}
	}

	return New_mcu; //After each MCU compoent is decoded we return the complete MCU array
} //End of HuffmanDecoding
