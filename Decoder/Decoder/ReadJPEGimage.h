#pragma once //Done
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "jpg.h"
#include "startOfFrameMarker.h"
#include "QuantizationMarker.h"
#include "HuffmanTableMarkers.h"
#include "StartOfScanMarker.h"
#include "RestartInterval.h"
#include "skipMarkers.h"
using namespace std;

JPEG_header* ReadJPEGimage(const string& jpgImage) //function to read jpg image it will read the jpg markers
{ 
	//open the file
	ifstream imageFile = ifstream(jpgImage, ios::in | ios::binary); //Make it binary so it does not default to text

	if (!imageFile.is_open()) //Check if file can be opened
	{
		cout << "Error the file could not be opened." << endl;
		return nullptr;
	}

	JPEG_header* jpgHeader = new (nothrow) JPEG_header; //Call a struct to store the JPEG header information

	//Begin reading the markers
	byte last_marker = imageFile.get();
	byte current_marker = imageFile.get();

	if (last_marker != 0xFF || current_marker != SOI)
	{
		jpgHeader->valid = false;
		imageFile.close();
		return jpgHeader;
	}

	//Reading markers
	last_marker = imageFile.get();
	current_marker = imageFile.get();

	// Process JPEG makers in image
	while (jpgHeader->valid) //While no errors have occured
	{ 
		if (!imageFile)
		{
			cout << "Error JPEG image file was not read completely." << endl;
			jpgHeader->valid = false;
			imageFile.close();
			return jpgHeader;
		}

		if (last_marker != 0xFF)
		{
			cout << "There was no marker in the file." << endl;;
			jpgHeader->valid = false;
			imageFile.close();
			return jpgHeader;
		}

		//Process start of frame marker
		if (current_marker == SOF0)
		{
			sofMarker(imageFile, jpgHeader);
		}
		//Process quatization table markers
		else if (current_marker == DQT)
		{
			setQuantizationTable(imageFile, jpgHeader); 
		}
		//Process Huffman Table Markers
		else if (current_marker == DHT)
		{
			HuffmanTableMarkers(imageFile, jpgHeader); 
		}
		//Process SOS Markers
		else if (current_marker == SOS)
		{
			StartOfScanMarker(imageFile, jpgHeader); 
			break;
		}
		//Process DRI markers
		else if (current_marker == DRI)
		{
			RestartInterval(imageFile, jpgHeader); 
		}

		//The following markers do not have any use for us so they will be skipped
		else if ((current_marker >= JPG0 && current_marker <= JPG13) || current_marker == DNL || current_marker == DHP || current_marker == EXP || current_marker == COM || (current_marker >= APP0 && current_marker <= APP15))
		{
			skip_markers(imageFile); 
		}
		else if (current_marker == TEM)
		{
			//TEM has no size so we do not need to skip it
		}

		//Any 0xFF is allowed thus ignore it
		else if (current_marker == 0xFF)
		{
			current_marker = imageFile.get();
			continue;
		}
		//If we go in this else an error has occured
		else
		{
			cout << "An error as occured when processing the JPEG markers." << endl;
			jpgHeader->valid = false;
			imageFile.close();
			return jpgHeader;
		}

		//Get the next mark type to be processed
		last_marker = imageFile.get(); 
		current_marker = imageFile.get();
	}

	//Read huffman coding bitsteam this should start after reading Start of Scan Marker
	if (jpgHeader->valid) 
	{
		cout << "Processing Huffman Coding Bitstream." << endl;
		current_marker = imageFile.get();

		while (true) //This will be an infinite loop which will end either when the bitstream as been read or an error occured
		{
			last_marker = current_marker; //This is the previous byte
			current_marker = imageFile.get(); //Read the next byte

			//If marker is found
			if (last_marker == 0xFF)
			{
				if (current_marker == EOI) //Check if we are at the end of the JPEG image
				{
					break; //break if it is actually the end of image
				}
				else if (current_marker == 0x00)//If the marker is 0x00 it means to put 0xFF into the file
				{
					jpgHeader->huffmanData.push_back(last_marker);
					current_marker = imageFile.get();
				}
				//If there is a restart marker we need to ignore it to do so we get the next byte and at the start of the loop we set last_marker to this byte and then get another current byte
				else if (current_marker >= RST0 && current_marker <= RST7) 
				{
					current_marker = imageFile.get();
				}
				//In case we look at multiple 0xFF we just ignore it
				else if (current_marker == 0xFF) 
				{ 
					continue;
				}
				//If no conditions occur then an error as occured
				else 
				{
					cout << "Error occured during proccessing of Huffman Coding Bitstream." << endl;
					jpgHeader->valid = false;
					imageFile.get();
					return jpgHeader;
				}
			}
			//If the last marker is not equal to 0xFF we add this value to the Huffman Data
			else 
			{
				jpgHeader->huffmanData.push_back(last_marker);
			}
		}
	}
	imageFile.close(); //Close the file
	return jpgHeader;
}