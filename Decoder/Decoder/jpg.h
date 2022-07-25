#pragma once
#ifndef JPG_H
#define JPG_H
#define PI   3.14159265358979323846264338327950288

#include <vector>

typedef unsigned char byte;
typedef unsigned int uint;

//JPEG markers taken from internet
const byte SOF0 = 0xC0;
const byte SOF1 = 0xC1;
const byte SOF2 = 0xC2;
const byte SOF3 = 0xC3;

const byte SOF5 = 0xC5;
const byte SOF6 = 0xC6;
const byte SOF7 = 0xC7;

const byte SOF9 = 0xC9;
const byte SOF10 = 0xCA;
const byte SOF11 = 0xCB;

const byte SOF13 = 0xCD;
const byte SOF14 = 0xCE;
const byte SOF15 = 0xCF;

//define Huffman tables
const byte DHT = 0xC4;

const byte JPG = 0xC8;

const byte DAC = 0xCC;

const byte RST0 = 0xD0;
const byte RST1 = 0xD1;
const byte RST2 = 0xD2;
const byte RST3 = 0xD3;
const byte RST4 = 0xD4;
const byte RST5 = 0xD5;
const byte RST6 = 0xD6;
const byte RST7 = 0xD7;

const byte SOI = 0xD8;
const byte EOI = 0xD9;
const byte SOS = 0xDA;
const byte DQT = 0xDB;
const byte DNL = 0xDC;
const byte DRI = 0xDD;
const byte DHP = 0xDE;
const byte EXP = 0xDF;

const byte APP0 = 0xE0;
const byte APP1 = 0xE1;
const byte APP2 = 0xE2;
const byte APP3 = 0xE3;
const byte APP4 = 0xE4;
const byte APP5 = 0xE5;
const byte APP6 = 0xE6;
const byte APP7 = 0xE7;
const byte APP8 = 0xE8;
const byte APP9 = 0xE9;
const byte APP10 = 0xEA;
const byte APP11 = 0xEB;
const byte APP12 = 0xEC;
const byte APP13 = 0xED;
const byte APP14 = 0xEE;
const byte APP15 = 0xEF;

const byte JPG0 = 0xF0;
const byte JPG1 = 0xF1;
const byte JPG2 = 0xF2;
const byte JPG3 = 0xF3;
const byte JPG4 = 0xF4;
const byte JPG5 = 0xF05;
const byte JPG6 = 0xF6;
const byte JPG7 = 0xF7;
const byte JPG8 = 0xF08;
const byte JPG9 = 0xF9;
const byte JPG10 = 0xFA;
const byte JPG11 = 0xFB;
const byte JPG12 = 0xFC;
const byte JPG13 = 0xFD;
const byte COM = 0xFE;
const byte TEM = 0x01;

struct quantization_table_struct //Stores the quantization table information
{
	uint q_table[64] = { 0 }; //Array that will store the tables 
	bool used = false; //Checks if the table has been used
};

struct huffman_table_struct 
{
	byte offset[17] = { 0 };  //Stores the starting index of each huffman value
	byte huffman_value[162] = { 0 }; //Stores all the huffman values
	uint huffman_code[162] = { 0 }; //Stores the code of the huffman table, it is uint since it can be larger than 8-bits

	bool used = false;
};

struct jpegColorComponent //Stores the information of each color component
{
	byte h_sampling_factor = 1;
	byte v_smapling_factor = 1;

	byte quantization_table_index = 0;

	byte huffman_DC_table_id = 0;
	byte huffman_AC_table_id = 0;
	bool used = false;

	
};

struct MCU_array //Stores the color components RGB
{
	//union allow us to give multiple names for one memory space
	union 
	{
		int y[64] = { 0 }; 
		int r[64];
	};

	union 
	{
		int cb[64] = { 0 };
		int g[64];
	};
	union 
	{
		int cr[64] = { 0 };
		int b[64];
	};

	int* operator[](uint i) //overload operator so we can address all r,g,b all at once
	{ 
		switch (i) {
		case 0:
			return y;
		case 1:
			return cb;
		case 2:
			return cr;
		default:
			return nullptr;
		}
	}
};

struct JPEG_header 
{
	quantization_table_struct q_table[5]; //We need 2 tables for most JPEG images, but incase an image needs extra tables the size is put to 5
	huffman_table_struct huffman_DC_table[4]; //A jpeg can have tables 0,1,2,3 so make the size equal 4
	huffman_table_struct huffman_AC_table[4]; //Again jpeg tables can be 0,1,2,3 so size needs to be 4

	uint jpeg_height = 0; //height of jpeg image
	uint jpeg_width = 0; //height of jpeg image
	byte jpeg_component_number = 0; //numer of components in jpeg image
	uint restart_interval = 0;

	jpegColorComponent colorComponents[3]; //JPEG contains only 3 color components so we use array size of 3

	bool valid = true;

	//For SOS
	byte start_selection = 0;
	byte end_selection = 63;
	byte s_approximation_high = 0;
	byte s_approximation_low = 0;
	bool zeroBased = false;
	std::vector<byte> huffmanData; //Use vector since we do not know the size
};

//Zig zag map will enable us to print the Quantization Table but it will display it so that top left hand contains the smaller values and
//The bottom right hand has the higher values
const byte zigZagMap[] = 
{
	0,    1  , 8,  16,   9,   2,   3,  10,
	17,  24,  32,  25,  18,  11,   4,   5,
	12,  19,  26,  33,  40,  48,  41,  34,
	27,  20,  13,   6,   7,  14,  21,  28,
	35,  42,  49,  56,  57,  50,  43,  36,
	29,  22,  15,  23,  30,  37,  44,  51,
	58,  59,  52,  45,  38,  31,  39,  46,
	53,  60,  61,  54,  47,  55,  62,  63
};//we can double check to see if it is correct
#endif // !JPG_H


