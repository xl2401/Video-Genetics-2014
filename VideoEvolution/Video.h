#pragma once
#include <vector>
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Frame.h"
#include <map>

using namespace std;
using namespace cv;

class Video
{
private:
	vector<Frame> framelist;
	vector<Frame> resizedlist;
	string name;
	int vid;
	
public:	
	string path;
	map<int,int> clustered_frame; // video representation map<frame_label, count>
	map<int, vector<int>> cluster_frame_fid;
	map<int, int >framemap;
	map<int,int> realfid;
	int frameno; //all the frames extracted from video (1 frame per second)
	int height;
	int width;
	Video(){};
	Video(string path, string name, int vid);
	Video(string path, int vid);
	~Video(void);

	string GetVideoName();
	void SetVideoName(string name);
	void SetVID(int n)
	{
		this->vid=n;
	}
	vector<Frame> GetFrameList();
	void AddFrameList(Frame f)
	{
		this->framelist.push_back(f);
	}
	void AddresizedFrameList(Frame f)
	{
		this->resizedlist.push_back(f);
	}
	void SetMap(int fid);
	void SetMapwithfid(int label, int fid)
	{
		this->cluster_frame_fid[label].push_back(fid);
	}
	void Setframemap(int resizedlistfid,int framefid)
	{
		this->framemap[resizedlistfid]=framefid;
	}
	void Setrealfid(int fid, int realfid)
	{
		this->realfid[fid]=realfid;
	}
	int GetVID();
};

