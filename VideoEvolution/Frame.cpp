#pragma once
#include "cv.h"
#include "highgui.h"
#include "Frame.h"
#include <vector>

Frame::Frame(Mat img,string vname, int vid, int fid, int realfid)
{
	this->img=img;
	this->videoName=vname;
	this->vid = vid;
	this->fid = fid;
	this->label=fid;
	this->height=this->img.rows;
	this->width=this->img.cols;
	this->realfid=realfid;

	/*namedWindow("image", CV_WINDOW_AUTOSIZE);
				imshow("image", this->img);
				waitKey();*/
}
Frame::~Frame(void)
{
}

int Frame::getFID(){
	return this->fid;
}

int Frame::getVID(){
	return this->vid;
}

int Frame::getrealFID()
{
	return this->realfid;
}

