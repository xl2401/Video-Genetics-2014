#pragma once
#include <vector>
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "MyLib.h"

using namespace std;
using namespace cv;

extern const int ACC_D;

class Frame
{
private:
	string videoName;//corresponding video
	int vid;//corresponding video ID
	int fid;//frame ID
	int realfid;
	int label;// label in frame clustering
	
public:	
	Mat img;//Mat for each frame
	IplImage Iplmg;
	int height;
	int width;
	double ACC[ACC_D];
	vector<double>hvector;
	Frame(Mat img, string vname, int vid, int fid,int realfid);
	~Frame(void);

	int getVID();
	int getFID();
	int getrealFID();
	string getvideoName()
	{
	return videoName;
	}
};

