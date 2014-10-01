#pragma once
#include "cv.h"
#include "highgui.h"
#include "Video.h"
#include "vector"
#include <fstream>
#include <sstream>

extern int FINDEX;

Video::Video(string path, string name, int vid)
{
	this->vid = vid;
	this->height=0;
	this->width=0;
	Mat img;
	VideoCapture capture(path);
	if( !capture.isOpened() ){
		cout  << "Could not open reference " << path << endl;
	}
	int fps = (int)capture.get(CV_CAP_PROP_FPS); //frame per second
	int fno = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);//total number of frames in the video

	if(fps>30) fps/=2;
	for(int i=0;i<fno;i++){
		if(i%fps == 0){

			capture>>img;
			if( !img.empty() ){  

				Mat temp = img.clone();
				double entropy = getEntropy(temp);
				
				if(entropy<ENTROPY) {
					continue;
				}

				//imwrite("D:\\Entropyframes\\"+i2s(FINDEX)+".png",temp);

				//imwrite("D:\\Entropyframes\\"+d2s(entropy)+".png",temp);
				/*namedWindow("image", CV_WINDOW_AUTOSIZE);
				imshow("image", temp);
				waitKey();*/
				if(this->height==0){
					this->height=temp.rows;
					this->width=temp.cols;
				}
			//	Frame * f = new Frame(temp,name,this->vid,FINDEX,realfid);
				FINDEX++;
			//	this->framelist.push_back(*f);
			}else{
				//cout  << "This is the end of this video." << endl;
				break;
			}
		}
		else 
		{
			capture>>img;
		}
	}
	this->path=path; 
	this->frameno = framelist.size();
}

Video::~Video(void)
{
}

vector<Frame> Video::GetFrameList()
{
	return this->framelist;
}

string Video::GetVideoName()
{
	return this->name;
}

void Video::SetVideoName(string name)
{
	this->name=name;
}

void Video::SetMap(int fid){// here fid is the label
	map<int,int>::iterator iter;
	iter = this->clustered_frame.find(fid);

	if(iter!=this->clustered_frame.end()){
		//already exist
		this->clustered_frame[fid]++;
		//int count = iter->second;
		//this->clustered_frame.erase(fid);
		//this->clustered_frame.insert(pair<int,int>(fid,count+1));
	}
	else{
		//add a new item
		this->clustered_frame.insert(pair<int,int>(fid,1));
	}
}

int Video::GetVID(){
	return this->vid;
}