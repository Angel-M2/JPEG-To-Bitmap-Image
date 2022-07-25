#pragma once 
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
using namespace std;

/* For the start of frame marker (SOF), there is an order we must follow when reading the bytes in the marker as shown below:
*	Step 1: The first two bytes read are the length of the header, this is read using big endian.
*	Step 2: The next one byte is the precision.
*	Step 3: Next two byte is the height.
*	Step 4: following two byte is the width.
*	Step 5: Next one byte read is the number of color components 
*		Step 5.1: For each color component one byte is for the component ID. 
*		Step 5.2: Followed by one byte for horizontal sampling factor and vertical sampling factor, horizontal smapling facort is first 4 bits 
*				  vertical sampling factor is the last 4 bits.
*		Step 5.3: The last one byte is the quantization table ID.
*/

//Function will read the start of frame markers 
void sofMarker(ifstream& jpg_image, JPEG_header* const jpgHeader) //Pass the image and and a pointer to header so we can store values
{ 
	cout << "Starting to read Start Of Frame Marker." << endl;

	uint header_length = (jpg_image.get() << 8) + jpg_image.get(); //Get the first two bytes in big endian

	byte jpg_precision = jpg_image.get(); //Following byte is the precision byte

	if (jpg_precision != 8) //The precision of a JPEG image has to be 8 or else it is an invalid JPEG
	{
		cout << "Precision value of the image is not 8 therefore not a JPEG." << endl;
		jpgHeader->valid = false; //Set flag to false 
		return;
	}

	jpgHeader->jpeg_height = (jpg_image.get() << 8) + jpg_image.get(); //Two bytes are the height of the image
	jpgHeader->jpeg_width = (jpg_image.get() << 8) + jpg_image.get(); //Next two bytes are the width of the image


	jpgHeader->jpeg_component_number = jpg_image.get(); //We now get the number color of components of the image

	//Check if color components are vaild; only vaild numbers are 1,2,3

	if (jpgHeader->jpeg_component_number == 0) //JPEG image must contain at least 1 color component
	{
		cout << "Color components of JPEG can not be 0." << endl;
		jpgHeader->valid = false;
		return;
	}

	/*At this step we now have the color components so we need to loop through each component and get the
	component ID, horizontal sampling factor, vertical sampling factor, and  quantization table ID*/
	for (uint i = 0; i < jpgHeader->jpeg_component_number; ++i) //Loop through each color component
	{
		byte color_componentID = jpg_image.get(); //First byte is the color component ID

		/*Color component IDs can be 1,2,3 or 0,1,2 since we are only going to handle cases where it is 1,2,3 we need to check if the
		ID starts from 0 if it does we can fix it by adding one thus we will always end up with color components 1,2,3*/

		if (color_componentID == 0) //Check if ID is 0
		{ 
			jpgHeader->zeroBased = true; 
		}

		if (jpgHeader->zeroBased) //If ID starts at zero add 1 to each ID number
		{
			color_componentID = color_componentID + 1;
		}

		jpegColorComponent* color_component = &jpgHeader->colorComponents[color_componentID - 1]; //We subtract 1 because the index of the arrays start at 0


		color_component->used = true; //This color component has been used

		//Now we get the next byte will contain horizontal and vertical sampling factor.
		byte samplingFactor = jpg_image.get(); 

		color_component->h_sampling_factor = samplingFactor >> 4; //First 4 bits are horizontal
		color_component->v_smapling_factor = samplingFactor & 0x0F; //Last 4 bits are the vertical

		color_component->quantization_table_index = jpg_image.get(); //Get the finally byte which is the quantization table ID

	} //End of for loop

}//End of function