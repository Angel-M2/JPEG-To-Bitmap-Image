#pragma once //Done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

//This functions is used to ignore jpeg markers that are not important to converting image to bitmap
void skip_markers(ifstream& jpg_image)  //Called when a marker to be skipped is found ifstream& jpg_image, Header* const header
{
	cout << "Skipping JPEG Markers that are not needed." << endl;

	/*Read the next two bytes, this is the length of data in the marker
	The data in a JPEG is in big endian so to read the 2 byte data,
	we first left shift the first byte by 8 and add the next byte to that */
	uint header_length = (jpg_image.get() << 8) + jpg_image.get();

	uint i = 0;

	//Loop until the end of the marker
	while (i < (header_length - 2)) { //file_length includes the 2 byte we read above thus why we need to subtract 2 from length
		jpg_image.get();//The function below does nothing instead it skips the data thus discarding the results
						//We still need to skip the data even though we are not using it because we need to move on to the next marker in the jpeg image
		++i;
	}
}
