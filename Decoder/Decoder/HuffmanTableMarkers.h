#pragma once //Done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

/* Huffman table markers 
* Step 1: First two byte is the length then the table information, the table info includes the most significant bit which tell us if it is AC or DC table while
*		  the least significant bits tell us the table ID which can be 0 to 3.
* Step 2: The next 16 bytes contain the number of codes, for example the second byte tells us how many codes are 2 bits long and so on.
* Step 3: Now we read the huffman values which is the sum of the previous 16 bytes.
*/
void HuffmanTableMarkers(ifstream& jpg_image, JPEG_header* const jpgHeader)
{ 
	cout << "Starting to read Huffman Table Markers." << endl;;
	int header_length = (jpg_image.get() << 8) + jpg_image.get(); //First byte gets us the length
	header_length = header_length - 2;

	while (header_length > 0)
	{
		byte table_info = jpg_image.get(); //Read table info byte
		byte table_index = table_info & 0x0F; //Get the table ID which is the least significant bits
		bool is_ac_table = table_info >> 4; //Using the most significant bits we can find out if it is AC or DC table, 0 equal false and true otherwise


		huffman_table_struct* huffman_table;

		if (is_ac_table) //If it is a AC table
		{ 
			huffman_table = &jpgHeader->huffman_AC_table[table_index]; //Use AC table created in header
		}
		else { //Else it is a DC table
			huffman_table = &jpgHeader->huffman_DC_table[table_index]; //Use DC table created in header
		}

		huffman_table->used = true;//Set flag equal to true once you have a table

		huffman_table->offset[0] = 0; //The first value is always goingt to be zero

		uint huffman_values = 0; //We need to count all the huffman values

		uint i = 1;
		while (i <= 16) {
			huffman_values = huffman_values + jpg_image.get(); //Keep track of the amount of huffman values while reading all the values which is the next 16 bytes
			huffman_table->offset[i] = huffman_values; //Set the offset 
			++i;
		}

		//At this point we are at step 3 so we have read all the 16 bytes that tell us how many codes each huffman values contains
		//So what is left is to read the huffman values
		uint j = 0;
		while (j < huffman_values) {
			huffman_table->huffman_value[j] = jpg_image.get(); //Get huffman values
			++j;
		}
		header_length = header_length - (17 + huffman_values);
	}
}


