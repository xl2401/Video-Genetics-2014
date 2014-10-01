#pragma once
#include "Frame.h"
#include "MyLib.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "math.h"


extern string RESOURCEPATH;
extern const int ROW;
extern const int ACC_D;
/*
The 1_ACC.txt store AutoColorCorrelogram results.
With first 32 columns(32 results), 33 column(video ID), and 34 column(frame ID)
*/
static void saveACCResults(Frame &f){
	string path ="ACC.txt"; 
	ofstream out(path,ios::app);
	if(!out){
		cout<<"Cannot open the file!"<<endl;
		return;
	}
	for(unsigned int i=0;i<ACC_D;i++){
		out<<f.ACC[i]<<" ";
		//cout<<f.ACC[i]<<endl;
	}
	out<<i2s(f.getVID())<<" "<<i2s(f.getFID())<<endl;//don't process the last line "\n", delete manually!
	out.close();
}

/*
  7 Bins, non-linear
*/
static int getDigit(int value, int slot){
	int digit = 0;
	if(value<slot){
		slot = ceil((double)slot/2);
		digit = floor((double)value/slot);
	}
	else{
		digit = floor((double)value/slot)+1;
	}
	return digit;
}

// ACC algorithm
static void AutoColorCorrelogram(Frame &f){
	int rows = f.img.rows;
	int cols = f.img.cols;
	int numBinR = 7;//6;
	int numBinG = 7;//6;
	int numBinB = 7;//6;
	int numBin = 6;
	int sizeBinR = ceil((double)256/numBin);
	int sizeBinG = ceil((double)256/numBin);
	int sizeBinB = ceil((double)256/numBin);

	///step 1: turn original frame (RGB) into 5 bits matrix
	int * imgBin;//rows*cols matrix
	imgBin = new int[rows*cols];
	for(int i=0;i<rows*cols;i++){
		*(imgBin+i)=0;
	}

	// 7*7*7 version
	for(int i=0; i<rows;i++){
		for(int j=0; j<cols;j++){
			uchar r = (f.img).at<Vec3b>(i,j).val[2];
			uchar g = (f.img).at<Vec3b>(i,j).val[1];
			uchar b = (f.img).at<Vec3b>(i,j).val[0];
			
			int rdigit = getDigit(r,sizeBinR);
			int gdigit = getDigit(g,sizeBinG);
			int bdigit = getDigit(b,sizeBinB);

			*(imgBin+i*cols + j)=rdigit*numBinG*numBinB + gdigit*numBinB + bdigit ;
		}
	}


	/// step 2: find number of same neighbors
	int * colorCount;//actually we just use (rows-6)*(cols-6) matrix
	colorCount = new int[rows*cols];
	for(int i=0;i<rows*cols;i++){
		*(colorCount+i)=0;
	}
	//ignore outter 3 layers
	for(int i=3; i<rows-3;i++){
		for(int j=3; j<cols-3;j++){
			int count =0;
			int pivot = *(imgBin+i*cols+j);

			for(int r=i-3;r<=i+3;r++){
				if(*(imgBin+r*cols + j-3)==pivot) count++;
				if(*(imgBin+r*cols + j+3)==pivot) count++;
			}
			for(int c=j-2;c<=j+2;c++){
				if(*(imgBin+(i-3)*cols + c)==pivot) count++;
				if(*(imgBin+(i+3)*cols + c)==pivot) count++;
			}
			*(colorCount+i*cols+j)=count;
		}
	}

	///step 3: generate feature vector
	double ACC[ACC_D];
	for(int i =0;i<ACC_D;i++){
		ACC[i]=0;//initialization
	}

	/*double * ACC;
	ACC = new double[ACC_D];
	for(int i=0;i<ACC_D;i++){
		*(ACC+i)=0;
	}*/
	for(int i=3;i<rows-3;i++){
		for(int j=3; j<cols-3;j++){
			ACC[*(imgBin+i*cols+j)]+=*(colorCount+i*cols+j);
		}
	}
	// 2013-10-22
	// entropy for ACC
	/*double entropy = getEntropyForACC(ACC, ACC_D);
	ofstream out("ACC_Entropy",ios::app);
	if(!out){
		cout<<"Cannot open the file!"<<endl;
		return;
	}
	out<<i2s(f.getFID())<<":"<<d2s(entropy)<<endl;
	out.close();*/


	///step 4: resize(TODO) or normilization by dividing (rows-3)*(cols-3)*24
	//int norm = (rows-3)*(cols-3)*24;
	int norm = (rows-3)*(cols-3);
	for(int i=0;i<ACC_D;i++){
		ACC[i]/=norm;
	}

	///step\ 5: store the feature vector to the frame
 	for(int i=0;i<ACC_D;i++){
		f.ACC[i]=ACC[i];

	}
	///step 6: save result to file
	saveACCResults(f);

	//delete[] ACC;
	for(int i=0;i<rows*cols;i++){
		imgBin[i]=NULL;
	}
	delete[] imgBin;
	imgBin=NULL;


	for(int i=0;i<rows*cols;i++){
		colorCount[i]=NULL;
	}
	delete[] colorCount;
	colorCount=NULL;

	/*delete[] ACC;
	ACC=NULL;*/

}


