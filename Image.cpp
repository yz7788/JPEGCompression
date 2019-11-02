//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"
#include <iostream>
#define PI 3.141592653589793

using namespace std;

// Constructor and Desctructors
MyImage::MyImage() 
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
	QLevel = 0;
	DMode = 0;
	Latency = 0;
	CompressData = NULL;
}

MyImage::~MyImage()
{
	if ( Data )
		delete Data;
	if (CompressData) {
		for (int i = 0; i < Width * Height / 64 * 3; i++) {
			for (int j = 0; j < 8; j++) {
				delete CompressData[i][j];
			}
			delete CompressData[i];
		}
		delete CompressData;
	}
}


// Copy constructor
MyImage::MyImage( MyImage *otherImage)
{
	Height = otherImage->Height;
	Width  = otherImage->Width;
	Data   = new char[Width*Height*3];
	strcpy(otherImage->ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage->Data[i];
	}


}

// = operator overload
MyImage & MyImage::operator= (const MyImage &otherImage)
{
	Height = otherImage.Height;
	Width  = otherImage.Width;
	Data   = new char[Width*Height*3];
	strcpy( (char *)otherImage.ImagePath, ImagePath );

	for ( int i=0; i<(Height*Width*3); i++ )
	{
		Data[i]	= otherImage.Data[i];
	}
	
	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if ( IN_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	// Allocate Data structure and copy
	Data = new char[Width*Height*3];
	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;
}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0 )
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}
	
	// Create a valid output file pointer
	FILE *OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if ( OUT_FILE == NULL ) 
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char *Rbuf = new char[Height*Width]; 
	char *Gbuf = new char[Height*Width]; 
	char *Bbuf = new char[Height*Width]; 

	for (i = 0; i < Height*Width; i++)
	{
		Bbuf[i] = Data[3*i];
		Gbuf[i] = Data[3*i+1];
		Rbuf[i] = Data[3*i+2];
	}

	
	// Write data to file
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}
	
	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}


// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
// Compress + Decompress
bool MyImage::Compress()
{
	// Break each channel into 8*8 blocks
	int w_8 = Width / 8;
	int h_8 = Height / 8;
	char*** RBlock = new char** [w_8 * h_8];
	char*** GBlock = new char** [w_8 * h_8];
	char*** BBlock = new char** [w_8 * h_8];

	for (int i = 0; i < w_8 * h_8; i++) {
		RBlock[i] = new char* [8];
		GBlock[i] = new char* [8];
		BBlock[i] = new char* [8];
		for (int j = 0; j < 8; j++) {
			RBlock[i][j] = new char[8];
			GBlock[i][j] = new char[8];
			BBlock[i][j] = new char[8];
			for (int k = 0; k < 8; k++) {
				int index = (Width * 8) * int(i / 240) + 8 * (i % 240) + 1920 * j + k;
				RBlock[i][j][k] = *(Data + 3 * index);
				GBlock[i][j][k] = *(Data + 3 * index + 1);
				BBlock[i][j][k] = *(Data + 3 * index + 2);
			}
		}
	}

	// DCT
	float alpha[8];
	for (int i = 0; i < 8; i++) {
		if (i == 0) {
			alpha[i] = 1 / sqrt(2);
		}
		else {
			alpha[i] = 1;
		}
	}
	float cosans[8][8];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cosans[i][j] = cos((2 * i + 1) * j * PI / 16);
		}
	}
	float*** RDCT = new float** [w_8 * h_8];
	float*** GDCT = new float** [w_8 * h_8];
	float*** BDCT = new float** [w_8 * h_8];
	for (int i = 0; i < w_8 * h_8; i++) {
		RDCT[i] = new float* [8];
		GDCT[i] = new float* [8];
		BDCT[i] = new float* [8];
		for (int j = 0; j < 8; j++) {
			RDCT[i][j] = new float[8];
			GDCT[i][j] = new float[8];
			BDCT[i][j] = new float[8];
			for (int k = 0; k < 8; k++) {
				float sum_Y = 0;
				float sum_Cr = 0;
				float sum_Cb = 0;
				for (int x = 0; x < 8; x++) {
					for (int y = 0; y < 8; y++) {
						sum_Y = sum_Y + float(unsigned char(RBlock[i][x][y])) * cosans[x][j] * cosans[y][k];
						sum_Cr = sum_Cr + float(unsigned char(GBlock[i][x][y])) * cosans[x][j] * cosans[y][k];
						sum_Cb = sum_Cb + float(unsigned char(BBlock[i][x][y])) * cosans[x][j] * cosans[y][k];
					}
				}
				RDCT[i][j][k] = 0.25 * alpha[j] * alpha[k] * sum_Y;
				GDCT[i][j][k] = 0.25 * alpha[j] * alpha[k] * sum_Cr;
				BDCT[i][j][k] = 0.25 * alpha[j] * alpha[k] * sum_Cb;
			}
		}
	}
	for (int i = 0; i < w_8 * h_8; i++) {
		for (int j = 0; j < 8; j++) {
			delete RBlock[i][j];
			delete GBlock[i][j];
			delete BBlock[i][j];
		}
		delete RBlock[i];
		delete GBlock[i];
		delete BBlock[i];
	}
	delete RBlock;
	delete GBlock;
	delete BBlock;

	//Quantize
	int*** RQuant = new int** [w_8 * h_8];
	int*** GQuant = new int** [w_8 * h_8];
	int*** BQuant = new int** [w_8 * h_8];
	for (int i = 0; i < w_8 * h_8; i++) {
		RQuant[i] = new int* [8];
		GQuant[i] = new int* [8];
		BQuant[i] = new int* [8];
		for (int j = 0; j < 8; j++) {
			RQuant[i][j] = new int[8];
			GQuant[i][j] = new int[8];
			BQuant[i][j] = new int[8];
			for (int k = 0; k < 8; k++) {
				int power = pow(2, QLevel);
				RQuant[i][j][k] = round(RDCT[i][j][k] / power);
				GQuant[i][j][k] = round(GDCT[i][j][k] / power);
				BQuant[i][j][k] = round(BDCT[i][j][k] / power);
			}
			delete RDCT[i][j];
			delete GDCT[i][j];
			delete BDCT[i][j];
		}
		delete RDCT[i];
		delete GDCT[i];
		delete BDCT[i];
	}
	delete RDCT;
	delete GDCT;
	delete BDCT;

	CompressData = new int** [w_8 * h_8 * 3];
	for (int i = 0; i < w_8 * h_8; i++) {
		CompressData[3 * i] = new int* [8];
		CompressData[3 * i + 1] = new int* [8];
		CompressData[3 * i + 2] = new int* [8];
		for (int j = 0; j < 8; j++) {
			CompressData[3 * i][j] = new int [8];
			CompressData[3 * i + 1][j] = new int [8];
			CompressData[3 * i + 2][j] = new int [8];
			for (int k = 0; k < 8; k++) {
				CompressData[3 * i][j][k] = RQuant[i][j][k];
				CompressData[3 * i + 1][j][k] = GQuant[i][j][k];
				CompressData[3 * i + 2][j][k] = BQuant[i][j][k];
			}
			delete RQuant[i][j];
			delete GQuant[i][j];
			delete BQuant[i][j];
		}
		delete RQuant[i];
		delete GQuant[i];
		delete BQuant[i];
	}
	delete RQuant;
	delete GQuant;
	delete BQuant;

	for (int i = 0; i < Width * Height; i++) {
		Data[3 * i] = 255;
		Data[3 * i + 1] = 255;
		Data[3 * i + 2] = 255;
	}

	return true;
}
bool MyImage::DecodeSequential(const int i) {

	//Dequantize
	float** R_DCT = new float* [8];
	float** G_DCT = new float* [8];
	float** B_DCT = new float* [8];
	for (int j = 0; j < 8; j++) {
		R_DCT[j] = new float[8];
		G_DCT[j] = new float[8];
		B_DCT[j] = new float[8];
		for (int k = 0; k < 8; k++) {
			int power = pow(2, QLevel);
			R_DCT[j][k] = round(CompressData[3 * i][j][k] * power);
			G_DCT[j][k] = round(CompressData[3 * i + 1][j][k] * power);
			B_DCT[j][k] = round(CompressData[3 * i + 2][j][k] * power);
		}
	}

	//Inverse DCT
	int** R_Block = new int* [8];
	int** G_Block = new int* [8];
	int** B_Block = new int* [8];

	float alpha[8];
	for (int i = 0; i < 8; i++) {
		if (i == 0) {
			alpha[i] = 1 / sqrt(2);
		}
		else {
			alpha[i] = 1;
		}
	}
	float cosans[8][8];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cosans[i][j] = cos((2 * i + 1) * j * PI / 16);
		}
	}
	for (int j = 0; j < 8; j++) {
		R_Block[j] = new int[8];
		G_Block[j] = new int[8];
		B_Block[j] = new int[8];
		for (int k = 0; k < 8; k++) {
			float sum_Y = 0;
			float sum_Cr = 0;
			float sum_Cb = 0;
			for (int u = 0; u < 8; u++) {
				for (int v = 0; v < 8; v++) {
					float alpha_u, alpha_v;
					if (u == 0) {
						alpha_u = 1 / sqrt(2);
					}
					else {
						alpha_u = 1;
					}
					if (v == 0) {
						alpha_v = 1 / sqrt(2);
					}
					else {
						alpha_v = 1;
					}
					sum_Y = sum_Y + alpha[u] * alpha[v] * R_DCT[u][v] *cosans[j][u] * cosans[k][v];
					sum_Cr = sum_Cr + alpha[u] * alpha[v] * G_DCT[u][v] * cosans[j][u] * cosans[k][v];
					sum_Cb = sum_Cb + alpha[u] * alpha[v] * B_DCT[u][v] * cosans[j][u] * cosans[k][v];
				}
			}
			R_Block[j][k] = int(0.25 * sum_Y);
			G_Block[j][k] = int(0.25 * sum_Cr);
			B_Block[j][k] = int(0.25 * sum_Cb);
		}
	}

	//DeBlock
	for (int j = 0; j < 8; j++) {
		for (int k = 0; k < 8; k++) {
			int index = 3 * ((Width * 8) * int(i / 240) + 8 * (i % 240) + 1920 * j + k);
			Data[index] = R_Block[j][k];
			Data[index + 1] = G_Block[j][k];
			Data[index + 2] = B_Block[j][k];
		}
	}
	return true;
}

bool MyImage::DecodeSpectral(const int n) {
	int w_8 = Width / 8;
	int h_8 = Height / 8;

	//Dequantize & Reserved the first n DCT coefficients
	int*** Y_DCT = new int** [w_8 * h_8];
	int*** Cr_DCT = new int** [w_8 * h_8];
	int*** Cb_DCT = new int** [w_8 * h_8];
	for (int i = 0; i < w_8 * h_8; i++) {
		Y_DCT[i] = new int* [8];
		Cr_DCT[i] = new int* [8];
		Cb_DCT[i] = new int* [8];
		for (int j = 0; j < 8; j++) {
			Y_DCT[i][j] = new int[8];
			Cr_DCT[i][j] = new int[8];
			Cb_DCT[i][j] = new int[8];
			for (int k = 0; k < 8; k++) {
				if (((j + k) * (j + k + 2) / 2 + ((k - j) * pow(-1, j + k)) / 2) <= n) {
					int power = pow(2, QLevel);
					Y_DCT[i][j][k] = round(CompressData[3 * i][j][k] * power);
					Cr_DCT[i][j][k] = round(CompressData[3 * i + 1][j][k] * power);
					Cb_DCT[i][j][k] = round(CompressData[3 * i + 2][j][k] * power);
				}
				else {
					Y_DCT[i][j][k] = 0;
					Cr_DCT[i][j][k] = 0;
					Cb_DCT[i][j][k] = 0;
				}
			}
		}
	}

	//Inverse DCT
	float alpha[8];
	for (int i = 0; i < 8; i++) {
		if (i == 0) {
			alpha[i] = 1 / sqrt(2);
		}
		else {
			alpha[i] = 1;
		}
	}
	float cosans[8][8];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cosans[i][j] = cos((2 * i + 1) * j * PI / 16);
		}
	}
	int*** Y_Block = new int** [w_8 * h_8];
	int*** Cr_Block = new int** [w_8 * h_8];
	int*** Cb_Block = new int** [w_8 * h_8];
	for (int i = 0; i < w_8 * h_8; i++) {
		Y_Block[i] = new int* [8];
		Cr_Block[i] = new int* [8];
		Cb_Block[i] = new int* [8];
		for (int j = 0; j < 8; j++) {
			Y_Block[i][j] = new int[8];
			Cr_Block[i][j] = new int[8];
			Cb_Block[i][j] = new int[8];
			for (int k = 0; k < 8; k++) {
				float sum_Y = 0;
				float sum_Cr = 0;
				float sum_Cb = 0;
				for (int u = 0; u < 8; u++) {
					for (int v = 0; v < 8; v++) {
						sum_Y = sum_Y + alpha[u] * alpha[v] * float(Y_DCT[i][u][v]) * cosans[j][u] * cosans[k][v];
						sum_Cr = sum_Cr + alpha[u] * alpha[v] * float(Cr_DCT[i][u][v]) * cosans[j][u] * cosans[k][v];
						sum_Cb = sum_Cb + alpha[u] * alpha[v] * float(Cb_DCT[i][u][v]) * cosans[j][u] * cosans[k][v];
					}
				}

				Y_Block[i][j][k] = int(0.25 * sum_Y);
				Cr_Block[i][j][k] = int(0.25 * sum_Cr);
				Cb_Block[i][j][k] = int(0.25 * sum_Cb);
				if (Y_Block[i][j][k] > 255) {
					Y_Block[i][j][k] = 255;
				}
				else if (Y_Block[i][j][k] < 0) {
					Y_Block[i][j][k] = 0;
				}

				if (Cr_Block[i][j][k] > 255) {
					Cr_Block[i][j][k] = 255;
				}
				else if (Cr_Block[i][j][k] < 0) {
					Cr_Block[i][j][k] = 0;
				}
			
				if (Cb_Block[i][j][k] > 255) {
					Cb_Block[i][j][k] = 255;
				}
				else if (Cb_Block[i][j][k] < 0) {
					Cb_Block[i][j][k] = 0;
				}
			}
		}
	}
	for (int i = 0; i < w_8 * h_8; i++) {
		for (int j = 0; j < 8; j++) {
			delete Y_DCT[i][j];
			delete Cr_DCT[i][j];
			delete Cb_DCT[i][j];
		}
		delete Y_DCT[i];
		delete Cr_DCT[i];
		delete Cb_DCT[i];
	}
	delete Y_DCT;
	delete Cr_DCT;
	delete Cb_DCT;

	//deBlock
	for (int i = 0; i < w_8 * h_8; i++) {
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 8; k++) {
				int index = 3 * ((Width * 8) * int(i / 240) + 8 * (i % 240) + 1920 * j + k);
				Data[index] = Y_Block[i][j][k];
				Data[index + 1] = Cr_Block[i][j][k];
				Data[index + 2] = Cb_Block[i][j][k];
			}
			delete Y_Block[i][j];
			delete Cr_Block[i][j];
			delete Cb_Block[i][j];
		}
		delete Y_Block[i];
		delete Cr_Block[i];
		delete Cb_Block[i];
	}
	delete Y_Block;
	delete Cr_Block;
	delete Cb_Block;

	return true;
}

bool MyImage::DecodeSuccessive(const int n) {
	int w_8 = Width / 8;
	int h_8 = Height / 8;

	//Dequantize & Rerseved 1st,... nth significant bit
	int*** Y_DCT = new int** [w_8 * h_8];
	int*** Cr_DCT = new int** [w_8 * h_8];
	int*** Cb_DCT = new int** [w_8 * h_8];
	for (int i = 0; i < w_8 * h_8; i++) {
		Y_DCT[i] = new int* [8];
		Cr_DCT[i] = new int* [8];
		Cb_DCT[i] = new int* [8];
		for (int j = 0; j < 8; j++) {
			Y_DCT[i][j] = new int[8];
			Cr_DCT[i][j] = new int[8];
			Cb_DCT[i][j] = new int[8];
			for (int k = 0; k < 8; k++) {
				int power = pow(2, QLevel);
				Y_DCT[i][j][k] = int(round(CompressData[3 * i][j][k] * power)) >> (7 - n) << (7 - n);
				Cr_DCT[i][j][k] = int(round(CompressData[3 * i + 1][j][k] * power)) >> (7 - n) << (7 - n);
				Cb_DCT[i][j][k] = int(round(CompressData[3 * i + 2][j][k] * power)) >> (7 - n) << (7 - n);
			}
		}
	}

	//Inverse DCT
	float alpha[8];
	for (int i = 0; i < 8; i++) {
		if (i == 0) {
			alpha[i] = 1 / sqrt(2);
		}
		else {
			alpha[i] = 1;
		}
	}
	float cosans[8][8];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cosans[i][j] = cos((2 * i + 1) * j * PI / 16);
		}
	}
	int*** Y_Block = new int** [w_8 * h_8];
	int*** Cr_Block = new int** [w_8 * h_8];
	int*** Cb_Block = new int** [w_8 * h_8];
	for (int i = 0; i < w_8 * h_8; i++) {
		Y_Block[i] = new int* [8];
		Cr_Block[i] = new int* [8];
		Cb_Block[i] = new int* [8];
		for (int j = 0; j < 8; j++) {
			Y_Block[i][j] = new int[8];
			Cr_Block[i][j] = new int[8];
			Cb_Block[i][j] = new int[8];
			for (int k = 0; k < 8; k++) {
				float sum_Y = 0;
				float sum_Cr = 0;
				float sum_Cb = 0;
				for (int u = 0; u < 8; u++) {
					for (int v = 0; v < 8; v++) {
						sum_Y = sum_Y + alpha[u] * alpha[v] * float(Y_DCT[i][u][v]) * cosans[j][u] * cosans[k][v];
						sum_Cr = sum_Cr + alpha[u] * alpha[v] * float(Cr_DCT[i][u][v]) * cosans[j][u] * cosans[k][v];
						sum_Cb = sum_Cb + alpha[u] * alpha[v] * float(Cb_DCT[i][u][v]) * cosans[j][u] * cosans[k][v];
					}
				}
				Y_Block[i][j][k] = int(0.25 * sum_Y);
				Cr_Block[i][j][k] = int(0.25 * sum_Cr);
				Cb_Block[i][j][k] = int(0.25 * sum_Cb);
				if (Y_Block[i][j][k] > 255) {
					Y_Block[i][j][k] = 255;
				}
				else if (Y_Block[i][j][k] < 0) {
					Y_Block[i][j][k] = 0;
				}

				if (Cr_Block[i][j][k] > 255) {
					Cr_Block[i][j][k] = 255;
				}
				else if (Cr_Block[i][j][k] < 0) {
					Cr_Block[i][j][k] = 0;
				}

				if (Cb_Block[i][j][k] > 255) {
					Cb_Block[i][j][k] = 255;
				}
				else if (Cb_Block[i][j][k] < 0) {
					Cb_Block[i][j][k] = 0;
				}
			}
		}
	}
	for (int i = 0; i < w_8 * h_8; i++) {
		for (int j = 0; j < 8; j++) {
			delete Y_DCT[i][j];
			delete Cr_DCT[i][j];
			delete Cb_DCT[i][j];
		}
		delete Y_DCT[i];
		delete Cr_DCT[i];
		delete Cb_DCT[i];
	}
	delete Y_DCT;
	delete Cr_DCT;
	delete Cb_DCT;

	//deBlock

	int* Y_Buf = new int[Width * Height];
	int* Cr_Buf = new int[Width * Height];
	int* Cb_Buf = new int[Width * Height];

	for (int i = 0; i < w_8 * h_8; i++) {
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 8; k++) {
				int index = (Width * 8) * int(i / 240) + 8 * (i % 240) + 1920 * j + k;
				Y_Buf[index] = Y_Block[i][j][k];
				Cr_Buf[index] = Cr_Block[i][j][k];
				Cb_Buf[index] = Cb_Block[i][j][k];
			}
			delete Y_Block[i][j];
			delete Cr_Block[i][j];
			delete Cb_Block[i][j];
		}
		delete Y_Block[i];
		delete Cr_Block[i];
		delete Cb_Block[i];
	}
	delete Y_Block;
	delete Cr_Block;
	delete Cb_Block;

	//set Data
	Data = new char[Width * Height * 3];
	for (int i = 0; i < Height * Width; i++)
	{
		Data[3 * i] = Y_Buf[i];
		Data[3 * i + 1] = Cr_Buf[i];
		Data[3 * i + 2] = Cb_Buf[i];
	}	
	delete Y_Buf;
	delete Cr_Buf;
	delete Cb_Buf;

	return true;
}
