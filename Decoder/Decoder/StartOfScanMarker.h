#pragma once //done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

/* The follows steps are taken in the function below in order to read the start of scan markers:
* Step 1: First two bytes are length of the marker.
* Step 2: Next byte is the number of color components.
*	Step 2.1: Each color component has one byte that is the component ID.
*	Step 2.2: The next byte is DC and AC table ID, where the first 4 bits are the DC ID and the last 4 bits are the AC table ID.
*	Step 2.3: Repeat step 2.1 and 2.2 based on number of color components found in step 2.
* Step 5: One byte for start of selection.
* Step 6: One byte for end of selection.
* Step 7: Last byte is for successive approximation high and low.
*/
void StartOfScanMarker(ifstream& jpg_image, JPEG_header* const jpgHeader)
{ 
	cout << "Starting to read Start of Scan Markers." << endl;

	//Since we have used the color component struct flag we need to reset the flag to false before we can continue
	for (uint i = 0; i < jpgHeader->jpeg_component_number; ++i)
	{
		jpgHeader->colorComponents[i].used = false;
	}

	uint marker_length = (jpg_image.get() << 8) + jpg_image.get(); //Two bytes give us the length of the marker

	byte color_component = jpg_image.get(); //Get the number of color component in the marker


	for (uint i = 0; i < color_component; ++i) //Loop through each color component
	{
		byte color_componentID = jpg_image.get(); //Frist we get the component ID

		if (jpgHeader->zeroBased) 
		{
			color_componentID = color_componentID + 1;
		}

		jpegColorComponent* component = &jpgHeader->colorComponents[color_componentID - 1];

		component->used = true; //This component is being used

		byte table_id = jpg_image.get(); //Read the AC and DC table ID

		component->huffman_DC_table_id = table_id >> 4; //first for bits are DC
		component->huffman_AC_table_id = table_id & 0x0F; //Last bits are AC
	}//End of for loop

	jpgHeader->start_selection = jpg_image.get(); //Get start of selection should be 0
	jpgHeader->end_selection = jpg_image.get(); //Get end of selection should be 63

	byte s_approximation = jpg_image.get(); //Get the successive approximation byte

	jpgHeader->s_approximation_high = s_approximation >> 4; //most significant bits are the high approximation
	jpgHeader->s_approximation_low = s_approximation & 0x0F;//least significant bits are the low approximation
}//End of function
