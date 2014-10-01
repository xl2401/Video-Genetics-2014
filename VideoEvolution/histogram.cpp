#pragma once
#include "Frame.h"
#include "MyLib.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "math.h"


extern string RESOURCEPATH;

static void savehistogramresult(Frame & f)
{
	string path ="histogramvector.txt"; 
	ofstream out(path,ios::app);
	if(!out){
		cout<<"Cannot open the file!"<<endl;
		return;
	}
	int n=f.hvector.size();
	for(unsigned int i=0;i<n;i++){
		out<<f.hvector[i]<<" ";
		//cout<<f.ACC[i]<<endl;
	}
	out<<i2s(f.getVID())<<" "<<i2s(f.getFID())<<endl;//don't process the last line "\n", delete manually!
	out.close();
}
static void calhistogram(Frame &f)
{
	int rows = f.img.rows;
	int cols = f.img.cols;
	int numBinG=256;
	int numBinB=256;
	int * imgBin;//rows*cols matrix
	imgBin = new int[rows*cols*3];
	for(int i=0;i<rows*cols*3;i++){
		*(imgBin+i)=0;
	}
	for(int k=2;k>=0;k--)
	{
		for(int i=0; i<rows;i++){
			for(int j=0; j<cols;j++){
				uchar r = (f.img).at<Vec3b>(i,j).val[k];
				*(imgBin+i*cols+j)=r;
				double x=(double)r/255.0;
				f.hvector.push_back(x);
			}
		}
	}
	savehistogramresult(f);
}