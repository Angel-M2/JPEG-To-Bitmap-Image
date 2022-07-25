#pragma once //Done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

/*Restart interval markers are needed to stay synchronized during data scans.
* The restart interval function is to reset DC value to zero very 4 MCU
* Steps to read restart interval function:
* Step 1: First two bytes contain the length of the marker which must equal to 4.
* Step 2: The following two bytes contain the restart interval that is a 16 bit uint.
*/
void RestartInterval(ifstream& jpg_image, JPEG_header* const jpgHeader)
{ 
	cout << "Starting to read Restart Interval Markers" << endl;

	uint header_length = (jpg_image.get() << 8) + jpg_image.get(); //Get the length of the marker

	jpgHeader->restart_interval = (jpg_image.get() << 8) + jpg_image.get(); //Get tje restart inteval value

	//Now the length mius the number of bits read must equal zero if not this is an error
	if (header_length - 4 != 0)
	{
		cout << "Length of restart interval is not 4." << endl;
		jpgHeader->valid = false;
	}
}//End of function