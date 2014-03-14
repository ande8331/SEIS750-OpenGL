/*
 * Steganography Project - Encode and pack data into an existing image
 * SEIS750 - Final Project
 * Ross Anderson
 * Spring 2012
 **/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/Angel.h>
#include <IL/il.h>
#include <iostream>
#include <fstream>

#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")

using namespace std;



//Modified slightly from the OpenIL tutorials
ILuint loadTexFile(const char* filename){
	
	ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */


	/* Before calling ilInit() version should be checked. */
	  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	  {
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	  }
 
	  success = ilLoadImage(filename); /* Loading of image from file */
	if (success){ /* If no error occured: */
		//We need to figure out whether we have an alpha channel or not
		  if(ilGetInteger(IL_IMAGE_BPP) == 3){
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every color component into
		  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		  }else if(ilGetInteger(IL_IMAGE_BPP) == 4){
			  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		  }else{
			  success = false;
		  }
		if (!success){
		  /* Error occured */
		 printf("failed conversion to unsigned byte");
		 exit(1);
		}
	}else{
		/* Error occured */
	   printf("Failed to load image ");
	   printf(filename);
		exit(1);
	}
}


int main(int argc, char** argv)
{    
	char imageFilePath[256];
	char textFilePath[256];
	char outputFilePath[256];
	int rBitsToPack = 3;
	int gBitsToPack = 3;
	int bBitsToPack = 3;

	if ((argc > 1) && !(strstr(argv[1], "Encoder.exe")))	// Command line args are goofy when called from VS
	{
		strcpy(imageFilePath, argv[1]);
	}
	else
	{
		strcpy(imageFilePath, "butters.png");
	}

	if (argc > 2)
	{
		strcpy(textFilePath, argv[2]);
	}
	else
	{
		strcpy(textFilePath, "mlb.txt");
	}

	if (argc > 3)
	{
		strcpy(outputFilePath, argv[3]);
	}
	else
	{
		strcpy(outputFilePath, "encoded.png");
	}

	if (argc > 4)
	{
		rBitsToPack = atoi(argv[4]);
	}

	if (argc > 5)
	{
		gBitsToPack = atoi(argv[5]);
	}

	if (argc > 6)
	{
		bBitsToPack = atoi(argv[6]);
	}

	ILuint ilTexID[2];

	/* Initialization of OpenIL */
	ilInit(); 	
	ilGenImages(2, ilTexID); /* Generation of two image names for OpenIL image loading */

	// Have to do this when loading images, else they come out upside down and inverted across x axis
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	// Load the file to encode
	ilBindImage(ilTexID[0]); /* Binding of IL image name */
	loadTexFile(imageFilePath);

	//Note how we depend on OpenIL to supply information about the file we just loaded in
	int texWidth = ilGetInteger(IL_IMAGE_WIDTH);
	int texHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	int imagePlaneCount = ilGetInteger(IL_IMAGE_BPP);

	/* Do the encoding, pixel for pixel here */
	unsigned char *encoding;
	encoding = ilGetData();

	char *testString;
	int length = texWidth*texHeight*(rBitsToPack+gBitsToPack+bBitsToPack);
	testString = new char[length];
   
	// Read in data to encode
	ifstream fin; 
	fin.open(textFilePath);

	// This method seems to be throwing away the linefeeds, but it doesn't matter at this point.
	int charCount = 0;
	for (int i = 0; i < length; i++)
	{
		if (fin.eof())
		{
			break;
		}
		testString[i] = fin.get();
		charCount++;
	}

	if (charCount*8 >= length)
	{
		// Insufficient space
		cout << "Text to encode is too large.  Either reduce the text, increase the number of bits to encode or choose a larger image. (Maximum size for this image and bit combination is approximately: " << length/8/1024 << "KB." << endl;
		return 0;
	}
	testString[charCount-1] = 0;	// Overwrite the EOF

	// Take the data read in, bit shift it into position so it can easily be added bitwise to the image
	char *bitArray;   
	bitArray = new char[length];

	char lastValue;
	for (int i = 0; i < length; i++)
	{
		char value = testString[i/8];
		lastValue = value;
		value >>= 7-(i % 8);
		value &= 0x01;
		bitArray[i] = value;
	}

	char rBitMask = 0x00;
	for (int i = 0; i < rBitsToPack; i++)
	{
		rBitMask <<= 1;
		rBitMask |= 1;
	}
	char rEncodeMask = ~rBitMask;

	char gBitMask = 0x00;
	for (int i = 0; i < gBitsToPack; i++)
	{
		gBitMask <<= 1;
		gBitMask |= 1;
	}
	char gEncodeMask = ~gBitMask;

	char bBitMask = 0x00;
	for (int i = 0; i < bBitsToPack; i++)
	{
		bBitMask <<= 1;
		bBitMask |= 1;
	}
	char bEncodeMask = ~bBitMask;

	int bitCounter = 0;
	for (int i = 0; i < texWidth*texHeight*imagePlaneCount; i+=imagePlaneCount)
	{
	   if (bitCounter < charCount*8)
	   {
		   char t = 0;

		   for (int j = 0; j < rBitsToPack; j++)
		   {
			   t <<= 1;
			   t |= bitArray[bitCounter++] & 0x01;
		   }
		   encoding[i] = (encoding[i] & rEncodeMask) | (t & rBitMask);

		   for (int j = 0; j < gBitsToPack; j++)
		   {
			   t <<= 1;
			   t |= bitArray[bitCounter++] & 0x01;
		   }
		   encoding[i+1] = (encoding[i+1] & gEncodeMask) | (t & gBitMask);

		   for (int j = 0; j < bBitsToPack; j++)
		   {
			   t <<= 1;
			   t |= bitArray[bitCounter++] & 0x01;
		   }
		   encoding[i+2] = (encoding[i+2] & bEncodeMask) | (t & bBitMask);
	   }
	   else
	   {
		   // Buffer overrun situation
		   break;
	   }
	}

	ilBindImage(ilTexID[1]); /* Binding of IL image name */
	ilTexImage(texWidth, texHeight, 0, imagePlaneCount, GL_RGBA, GL_UNSIGNED_BYTE, encoding);

	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(outputFilePath);

	// Check for errors (Usually just during debug)
	ILenum error = ilGetError();	
	if (error != 0)
	{
		cout << "Error code: " << error << " thrown." << endl;
	}

    ilDeleteImages(2, ilTexID); //we're done with OpenIL, so free up the memory

	return 0;
}