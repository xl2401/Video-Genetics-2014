/*
This file is for static functions
*/

#pragma once
#include <string>
#include <sstream>
#include "cv.h"
#include "highgui.h"
#include <math.h>
using namespace std;  
using namespace cv;

static string RESOURCEPATH="C:\\Users\\Mengfei\\Desktop\\Video Evolution\\VideoEvolution\\";
static const int ACC_D = 343;//216;//# ACC features// 7*7*7
static const int COL=345;//218; 343+2
static const int ROW=20246;//7117//5839;//8867; the framlib size of 50 videos.//20246
static const int K = 62; //the number of nearnest frames 
static const float DIST_THRE = 5; 
static int labels[ROW];
static float ENTROPY = 2.4;
static const int VIDEO_THRE = 2 ; //the threshold of clustered videos.

//int to string
static string i2s(int i)
{
	string temp;
	stringstream ss;
	ss<<i;
	ss>>temp;
	return temp;
}
static string d2s(double dbl){
	ostringstream strs;
	strs << dbl;
	string str = strs.str();
	return str;
}

// return the smaller integer
static int min2(int a, int b){
	return a<b?a:b;
}

//2013-10-07
static double calEntro(Mat m, int freq){
	double result = 0;

	for(int i=0;i<m.rows;i++){
		float count = m.at<float>(i,0);
		if((int)count==0) continue;// P(w)=0
		float param, tmp;
		param = (float)count/freq;
		tmp = log10(param);
		result += -1*param*tmp;
	}
	//cout<<result<<endl;
	return result;
}


static double getEntropy(Mat m){

	vector<Mat> bgr_planes;
	split(m, bgr_planes );

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 } ;
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
	calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
	calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

	int frequency = m.rows*m.cols;
	double r_entro = calEntro(r_hist,frequency);
	double g_entro = calEntro(g_hist,frequency);
	double b_entro = calEntro(b_hist,frequency);

	return r_entro+g_entro+b_entro;

}

static double getEntropyForACC(double * ACC,int size){
	double count = 0;
	for(int i=0;i<size;i++){
		count+=ACC[i];
	}
	double result = 0;
	for(int i=0;i<size;i++){
		if(ACC[i]==0) continue;
		double freq = ACC[i]/count;
		result+=-1*freq*log10(freq);
	}
	return result;
}
