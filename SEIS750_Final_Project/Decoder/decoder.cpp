/*
 * Steganography Project - Decode data packed into an image
 * SEIS750 - Final Project
 * Ross Anderson
 * Spring 2012
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/Angel.h>
#include <IL/il.h>
#include <iostream>
#include <fstream>
using namespace std;

#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")



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
	int rBitsToUnpack = 1;
	int gBitsToUnpack = 1;
	int bBitsToUnpack = 1;

	if (argc > 1)
	{
		strcpy(imageFilePath, argv[1]);
	}
	else
	{
		strcpy(imageFilePath, "encoded.png");
	}

	if (argc > 2)
	{
		strcpy(textFilePath, argv[2]);
	}
	else
	{
		strcpy(textFilePath, "output.txt");
	}

	if (argc > 3)
	{
		rBitsToUnpack = atoi(argv[3]);
	}

	if (argc > 4)
	{
		gBitsToUnpack = atoi(argv[4]);
	}

	if (argc > 3)
	{
		bBitsToUnpack = atoi(argv[5]);
	}


	ILuint ilTexID[2];
	ilInit(); /* Initialization of OpenIL */
	ilGenImages(2, ilTexID); /* Generation of three image names for OpenIL image loading */

	// Have to do this since the encoded images are using this
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	// Load Image to Decode
	ilBindImage(ilTexID[0]); /* Binding of IL image name */	
	loadTexFile(imageFilePath);

	//Note how we depend on OpenIL to supply information about the file we just loaded in
	int texWidth = ilGetInteger(IL_IMAGE_WIDTH);
	int texHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	int imagePlaneCount = ilGetInteger(IL_IMAGE_BPP);

	/* Do the decoding, pixel for pixel here */
	unsigned char *encoding;
	encoding = ilGetData();

	char *bitArray;
	int length = texWidth*texHeight*3*4;
	bitArray = new char[length*8];
   
	int bitCount = 0;
	for (int i = 0; i < texWidth*texHeight*imagePlaneCount; i+=imagePlaneCount)
	{
		for (int j = rBitsToUnpack-1; j >= 0; j--)
		{
			bitArray[bitCount++] = (encoding[i] >> j) & 0x01;
		}

		for (int j = gBitsToUnpack-1; j >= 0; j--)
		{
			bitArray[bitCount++] = (encoding[i+1] >> j) & 0x01;
		}

		for (int j = bBitsToUnpack-1; j >= 0; j--)
		{
			bitArray[bitCount++] = (encoding[i+2] >> j) & 0x01;
		}
	}

	length *= 2; // Since we are packing two bits into each color, need to adjust the cutoff.

	char *output;
	output = new char[length/8];
	for (int i = 0; i < length/8; i++)
	{
		char tmp = 0;

		for (int j = i*8; j < (i*8) + 8; j++)
		{
			tmp <<= 1;
			tmp |= bitArray[j] & 0x01;
		}
		output[i] = tmp;
	}

	// Output decoded data to file
	ofstream fout; 
	fout.open(textFilePath);
	
	int charCount = 0;
	for (int i = 0; i < length/8; i++)
	{
		fout << output[i];

		// Quit when complete
		if (output[i] == 0)
		{
			break;
		}
	}

	// Check for errors (Usually just during debug)
	ILenum error = ilGetError();	
	if (error != 0)
	{
		cout << "Error code: " << error << " thrown." << endl;
	}

    ilDeleteImages(2, ilTexID); //we're done with OpenIL, so free up the memory

	return 0;
}