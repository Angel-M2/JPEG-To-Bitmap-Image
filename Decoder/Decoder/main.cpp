#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
#include "ReadJPEGimage.h"
#include "HuffmanDecoder.h"
#include "IDCT.h"
#include "Dequantization.h" 
#include "ColorConversion.h"
#include "write_jpg_to_bmp.h"
using namespace std;

int main() 
{
	string imagename = "sample.jpg";
	const string file(imagename);
	JPEG_header* jpgHeader = ReadJPEGimage(file);
	//printHeader(header);

	MCU_array* New_mcu = HuffmanDecoding(jpgHeader);
	if (New_mcu == nullptr)
	{ 
		cout << "Error opening MCU" << endl;
		delete jpgHeader;
	}
	
	//Dequantize MCU
	DequantizeMCU(jpgHeader, New_mcu);

	//Inverse Discrete Cosine Transform
	IDCT(jpgHeader, New_mcu);

	//Color Conversion
	colorConversion(jpgHeader, New_mcu);

	//Write bmp file
	const size_t pos = file.find_last_of('.');
	const string bmpFile = (pos == string::npos) ? (file + ".bmp") : (file.substr(0, pos) + ".bmp");
	write_jpg_to_bmp(jpgHeader, New_mcu, bmpFile);

	delete jpgHeader;
	return 0;
}