#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Frame.h"
#include "Video.h"
#include "AutoColorCorrelogram.cpp"
#include "histogram.cpp"
#include "Flann.cpp"
#include "MyLib.h"
#include "Frame.h"
#include "resize.cpp"

static int maxheight=15;
static int maxwidth=20;
using namespace std;   

///global variables
extern string RESOURCEPATH;
extern const int ROW;
extern const int COL;
extern const int K;
extern const int VIDEO_THRE;

vector<Video> videoLib;// store all videos
vector<Frame> frameLib;//store all frames of all videos
vector<Frame> resizedframeLib;//store all resized frames of videos.
vector<string> videonames;//store all videos by name
int VINDEX = 0; //video index
int FINDEX = 0; // frame index
int REALFINDEX=0;
int totalcount=0;

/// traverse the videos in the dataset folder
void FileTraverser(char path[],int level);
void FileTraverser1(char path[],int level);


void IntensityNormalization(Frame &f);
void VideoCluster();


/**
* @ function main function
*/
int main(int argc, char** argv)   
{   
	cout<<"***************"<<endl;
	cout<<"Video Evolution"<<endl;
	cout<<"***************"<<endl;
	cout<<endl;

//	****** Step 1: Generate the Frame Library ******
	char* datapath = argv[1];
	string path(datapath);
	cout<<"All videos are in folder:"<<path<<endl;
	FileTraverser1(datapath,0);


//resize the image and save resized ones 
	cout<<"resize the image and save the result"<<endl;
	for(unsigned int i=0;i<videoLib.size();i++)
	{
		Resizeimg(&videoLib[i],maxheight,maxwidth,resizedframeLib);
	}  

 //Intensity normalization
 	cout<<"Frame intensity normalization"<< endl;
	for(unsigned int i=0;i<resizedframeLib.size();i++){
		IntensityNormalization(resizedframeLib.at(i));
	}

	//cout cal histogram vector of each image
	ofstream outhistogram("histogramvector.txt",ios::trunc);
	outhistogram.close();
	for(unsigned int i=0;i<resizedframeLib.size();i++)
	{
 		calhistogram(resizedframeLib.at(i));
	}
	

	///***** Step 2: Do AutoColorCorrelogram algorithm *****
	/*cout<<"Step 2: Doing Auto Color Correlogram ..."<<endl;
	ofstream outacc("ACC.txt",ios::trunc);
	outacc.close();
	for(unsigned int i=0;i<frameLib.size();i++){
 		AutoColorCorrelogram(frameLib.at(i));
	}*/
	
	/// ***** Step 3: FLANN *****
	cout<<"Step 3: FLANN ..."<<endl;
	ofstream outflann("FLANN_index.txt",ios::trunc);
	outflann.close();
	ofstream outflann1("FLANN_dist.txt",ios::trunc);
	outflann1.close();
	ofstream outflann2("FRM.txt",ios::trunc);
	outflann2.close();
	ofstream outuf("Frame_Cluster.txt",ios::trunc);
	outuf.close();
	ofstream outcluster("ClusterSet.txt",ios::trunc);
	outcluster.close();
 	FLANN (resizedframeLib);
	
	/////// ***** Step 4: Video Clustering *****
	cout<<"Step 4: Video Cluster ..."<<endl;
 	ofstream outvcluster("Video_Relationship.txt",ios::trunc);
	outvcluster.close();
	ofstream outgraph("finalgraph.dot",ios::trunc);//graphviz input file
	outgraph.close();
	ofstream outfvcluster("Video_frame_Relationship.txt",ios::trunc);
	outfvcluster.close();
	VideoCluster();
	cout<<" end"<<endl;
	//system£¨"pause"£©;
}




/**
* @ function traverse all the videos
* @ para path The path of the folder containing vidoes
* @ para level Start at 0
*/
void FileTraverser(char path[],int level = 0)
{
	cout<<"Step 1: Generate frame library ..."<<endl;
	ofstream ovideo("VideoNames.txt",ios::trunc);//clear previous data
	ovideo.close();
	string videoname ="VideoNames.txt"; 
	ofstream out(videoname,ios::app);

	char find_path[128];
	sprintf_s(find_path, "%s*", path);

	WIN32_FIND_DATA FindFileData;//Call WIN32
	HANDLE hFind;
	BOOL bContinue = TRUE;

	hFind = FindFirstFile(find_path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
		return;
	while (bContinue)
	{
		if (_stricmp(FindFileData.cFileName, ".")||_stricmp(FindFileData.cFileName, ".."))
		{
			string tempname(FindFileData.cFileName);
			// retrieve video's absolute path
			if(FindFileData.dwFileAttributes!= FILE_ATTRIBUTE_DIRECTORY && tempname.find(".avi",0)!=-1 )
			{
				out<<tempname<<endl;
				string nosuffix = tempname.substr(0,tempname.find('.'));
				videonames.push_back(nosuffix);//store video names
				string temppath(find_path);
				temppath.erase(temppath.find(' *',0),1);//tempath = path + video name
				Video * v = new Video(temppath+tempname, tempname.substr(0,tempname.find(".avi")),VINDEX);
				VINDEX++;

				videoLib.push_back(*v);
				vector<Frame> list = v->GetFrameList();
				//put all frames into the frameLib
				for(unsigned  int i = 0; i<list.size();i++){
					frameLib.push_back(list.at(i));

				}
			}
		}
		bContinue = FindNextFile(hFind, &FindFileData);
	}
	out.close();
}


void FileTraverser1(char path[],int level = 0)
{
	cout<<"Step 1: Generate frame library ..."<<endl;
	ofstream ovideo("VideoNames_1.txt",ios::trunc);//clear previous data
	ovideo.close();
	string videoname ="VideoNames_1.txt"; 
	ofstream out(videoname,ios::app);

	char find_path[128];
	sprintf_s(find_path, "%s*", path);

	WIN32_FIND_DATA FindFileData;//Call WIN32
	HANDLE hFind;
	BOOL bContinue = TRUE;

	hFind = FindFirstFile(find_path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
		return;
	while (bContinue)
	{
		if (_stricmp(FindFileData.cFileName, ".")||_stricmp(FindFileData.cFileName, ".."))
		{
			string tempname(FindFileData.cFileName);
			// retrieve video's absolute path
			if(FindFileData.dwFileAttributes!= FILE_ATTRIBUTE_DIRECTORY && tempname.find(".avi_libjpeg_DC",0)!=-1 )
			//if(FindFileData.dwFileAttributes!= FILE_ATTRIBUTE_DIRECTORY && tempname.find(".avi_-g_0.mp4.mp4_libjpeg_DC",0)!=-1 )
			{
				out<<tempname<<endl;
				string nosuffix = tempname.substr(0,tempname.find('.'));
				videonames.push_back(nosuffix);//store video names
				string temppath(find_path);
				temppath.erase(temppath.find(' *',0),1);//tempath = path + video name
				Video * v = new Video();
				v->path=temppath;
				(*v).SetVideoName(nosuffix);
				(*v).SetVID(VINDEX);

				WIN32_FIND_DATA FindFileData2;// tranverse the thumbnails in the video file
				HANDLE hFind2;
				BOOL bContinue2=TRUE;
				string videopath=temppath+tempname+"\\*";
				hFind2=FindFirstFile(videopath.c_str(),&FindFileData2);
				if(hFind2==INVALID_HANDLE_VALUE)
					return;
				while(bContinue2)
				{
					if(_stricmp(FindFileData2.cFileName,".")||_stricmp(FindFileData2.cFileName, ".."))
					{
						string tempvideopath=videopath;
						tempvideopath.erase(tempvideopath.find("*",0),1);
						string imgname(FindFileData2.cFileName);
						if(FindFileData.dwFileAttributes!= FILE_ATTRIBUTE_DIRECTORY && imgname.find(".jpg",0)!=-1 )
						{
							REALFINDEX++;
							totalcount++;
							string imgtemp=tempvideopath+imgname;
							IplImage *img = cvLoadImage(imgtemp.c_str(), -1);
							Mat tmp=img;
							double entropy = getEntropy(tmp);
				
							if(entropy<ENTROPY) {
							bContinue2=FindNextFile(hFind2,&FindFileData2);//ignore this thumbnail.
							continue;
							}
							v->height=tmp.rows;
							v->width=tmp.cols;
							
								/*if(v->height>maxheight)
									maxheight=v->height;
								if(v->width>maxwidth)
									maxwidth=v->width;
*/

							Frame * f = new Frame(tmp,nosuffix,VINDEX,FINDEX,REALFINDEX);
							f->Iplmg=*img;
							(*v).AddFrameList(*f);
							FINDEX++;
						}

					}
					bContinue2=FindNextFile(hFind2,&FindFileData2);
				}
				v->frameno=FINDEX;
				
				FINDEX=0;
				REALFINDEX=0;
				VINDEX++;
				vector<Frame> list = v->GetFrameList();
				//put all frames into the frameLib
				for(unsigned  int i = 0; i<list.size();i++){
					frameLib.push_back(list.at(i));
					v->Setframemap(i,frameLib.size()-1);
				}
				videoLib.push_back(*v);

			}
		}
		bContinue = FindNextFile(hFind, &FindFileData);
	}
//	ROW=frameLib.size();
	out.close();
}
/*
* @function 
*/
void VideoCluster(){
	///****** Step 1: get transitive closure results ***********
	int frame_lables[ROW];
	ifstream infile;
	infile.open ("Frame_Cluster.txt");
	string line;
	if (infile.is_open()&&infile.good())
	{
		getline(infile,line);
		istringstream iss(line);
		int col=0;
		while(iss)
		{
			string word;
			iss >> word;
			if(word=="") break;
			frame_lables[col] = atoi(word.c_str());
			col++;
		} 
	}
	infile.close();

	/// ********* Step 2: video representations ***********
	for(int i=0;i<resizedframeLib.size();i++){
		int fid = (resizedframeLib.at(i)).getFID();
		int realfid=(resizedframeLib.at(i)).getrealFID();
		int vid = resizedframeLib.at(i).getVID();
		int framefid=videoLib.at(vid).framemap[fid];
		int label = frame_lables[framefid];
		videoLib.at(vid).SetMap(label);
		videoLib.at(vid).SetMapwithfid(label,realfid);
	}

	/// ********* Step 3: video cluster ***********
	string timeline="videodatepairnew.txt";
	ifstream timelinefile;
	timelinefile.open(timeline);
	string line1;

	map<string,int> videodate;// key:videoname, value: video post year

	if (timelinefile.is_open())
	{
		while ( timelinefile.good() )
		{
			getline(timelinefile,line1);
			istringstream iss(line1);
			while(iss)
			{
				string word;
				iss >> word;
				if(word=="") break;
				int comma = word.find(",");
				string vname = word.substr(0,comma);
				int day = atoi(word.substr(comma+1, 4).c_str());
				videodate.insert(pair<string,int>(vname,day));
			}//end while 
		}
	}
	timelinefile.close();

	map<int,vector<int>> vtimeline;// key:year, value: list of vid
	map<int,vector<int>>::iterator iter;
	for(int i=0;i<videoLib.size();i++){
		string key = videoLib.at(i).GetVideoName();
	//	int a = key.find_last_of('\\');
		//int b = key.find(".avi");
	//	key = key.substr(a+1,b-a-1);


		int vindex = videoLib.at(i).GetVID();
		int day = 0;
		map<string,int>::iterator viter;
		viter = videodate.find(key);
		if(viter!=videodate.end()){
			day=viter->second;

		}

		iter = vtimeline.find(day);

		if(iter!= vtimeline.end()){
			vector<int> tmp = iter->second;
			tmp.push_back(vindex);
			vtimeline.erase(day);
			vtimeline.insert(pair<int,vector<int>>(day,tmp));
		}
		else{
			vector<int> tmp;
			tmp.push_back(vindex);
			vtimeline.insert(pair<int,vector<int>>(day,tmp));
		}

	}


	//Generate Graphviz input file
    string path_graph = "finalgraph.dot";
	ofstream outgraph(path_graph,ios::app);
	outgraph<<"graph G{"<<endl;
	outgraph<<"ranksep=.75; size =\"50, 50\";"<<endl;
	outgraph<<"{"<<endl;
	outgraph<<"node [shae=plaintext, fontsize=16];"<<endl;
	outgraph<<"past";

	map<int,vector<int>>::iterator iter2;
	for(iter2=vtimeline.begin();iter2!=vtimeline.end();iter2++){
		outgraph<<" -- "<<i2s(iter2->first);
	}

	outgraph<<" -- future;}"<<endl;
	outgraph<<"node [shape=box];"<<endl;

	map<int,vector<int>>::iterator iter3;

	for(iter3=vtimeline.begin();iter3!=vtimeline.end();iter3++){
		int year = iter3->first;
		vector<int> yearvideo = iter3->second;
		outgraph<<"{ rank=same;"<<year<<";";
		for(int t=0; t<yearvideo.size();t++){
			int ID = yearvideo.at(t);
			outgraph<<"\""<<videonames.at(ID)<<"\";";
		}
		outgraph<<"}"<<endl;
	}

	for(int i=0;i<videoLib.size();i++){
		Video v1 = videoLib.at(i);
		for(int j=i+1;j<videoLib.size();j++){
			Video v2 = videoLib.at(j);
			//compare
			int share =0;// how many frames v1 and v2 share
			map<int,int>::iterator iter1;
			map<int,int>::iterator iter2;
			vector<int> sharedclusters;
			for ( iter1 = v1.clustered_frame.begin( ); iter1 != v1.clustered_frame.end( ); iter1++ ){
				iter2=v2.clustered_frame.find(iter1->first);
				//find the same label
				if(iter2!=v2.clustered_frame.end()){
					share++;
					sharedclusters.push_back(iter1->first);
				}
			}
			
			if(share>VIDEO_THRE){
				//related(i,j)
				string path_vcluster = "Video_Relationship.txt";
				ofstream out_vcluster(path_vcluster,ios::app);
				if(!out_vcluster){
					cout<<"Cannot open the file!"<<endl;
					return;
				}

				out_vcluster<<i2s(i)<<" "<<i2s(j)<<" ||share->"<<i2s(share)<<endl;
				out_vcluster.close();

				outgraph<<"\""<<videonames.at(i)<<"\"--\""<<videonames.at(j)<<"\" [label="<<i2s(share)<<"]"<<endl;
			}
			if(share>VIDEO_THRE){
				string path_vfcluster="Video_frame_Relationship.txt";
				ofstream out_vfcluster(path_vfcluster,ios::app);
				if(!out_vfcluster)
				{
					cout<<"Cannot open the file!"<<endl;
					return;
				}
				out_vfcluster<<"video "<<videonames.at(i)<<" with video "<<videonames.at(j)<<" "<<endl;
				for(int i=0;i<sharedclusters.size();i++)
				{
					out_vfcluster<<"sharecluster "<<sharedclusters[i]<<endl;
					//out_vfcluster<<"cluster "<<sharedclusters[i];
					out_vfcluster<<"video :"<<v1.GetVideoName()<<" ";
					int previous1;
					int previous2;
					int count2=0;
					for(int j=0;j<v1.cluster_frame_fid[sharedclusters[i]].size();j++)
					{
						if(j!=0)
						{
							if(v1.cluster_frame_fid[sharedclusters[i]][j]-v1.cluster_frame_fid[sharedclusters[i]][j-1]==1)
								continue;
							else
							{
									if(previous1!=v1.cluster_frame_fid[sharedclusters[i]][j-1])
									out_vfcluster<<"~ "<<v1.cluster_frame_fid[sharedclusters[i]][j-1]<<" ";
									out_vfcluster<<v1.cluster_frame_fid[sharedclusters[i]][j]<<" ";
									previous1=v1.cluster_frame_fid[sharedclusters[i]][j];
							}
						}
						else
						{
						previous1=v1.cluster_frame_fid[sharedclusters[i]][j];
						out_vfcluster<<v1.cluster_frame_fid[sharedclusters[i]][j]<<" ";
						}
					}
					out_vfcluster<<endl;
					out_vfcluster<<"video :"<<v2.GetVideoName()<<" ";
					for(int j=0;j<v2.cluster_frame_fid[sharedclusters[i]].size();j++)
					{
						if(j!=0)
							{
								if(v2.cluster_frame_fid[sharedclusters[i]][j]-v2.cluster_frame_fid[sharedclusters[i]][j-1]==1)
									continue;
								else
								{
										if(previous1!=v2.cluster_frame_fid[sharedclusters[i]][j-1])
										out_vfcluster<<"~"<<v2.cluster_frame_fid[sharedclusters[i]][j-1]<<" ";
										out_vfcluster<<v2.cluster_frame_fid[sharedclusters[i]][j]<<" ";
										previous1=v2.cluster_frame_fid[sharedclusters[i]][j];
								}
							}
						else
						{
							previous1=v2.cluster_frame_fid[sharedclusters[i]][j];
							out_vfcluster<<v2.cluster_frame_fid[sharedclusters[i]][j]<<" ";
						}
					}
					out_vfcluster<<endl; 	
				}
				cout<<endl;
				cout<<endl;
				out_vfcluster.close();

			}

		}
	}
	outgraph<<"}";
	outgraph.close();
}

/* Frame color normalization

*/
void IntensityNormalization(Frame &f)
{
	int rows = f.img.rows;
	int cols = f.img.cols;
	double maxMag = 0;
	double * frameoutR;
	frameoutR = new double[rows*cols];
	double * frameoutG;
	frameoutG = new double[rows*cols];
	double * frameoutB;
	frameoutB = new double[rows*cols];
	double * frameout;
	frameout = new double[rows*cols];

	for(int i=0;i<rows*cols;i++){
		*(frameoutR+i)=0;
		*(frameoutG+i)=0;
		*(frameoutB+i)=0;
	}
	//caculate every pixel in R,G,B panel 's intensity.
	for(int i=0; i<rows;i++){
		for(int j=0; j<cols;j++){
			*(frameoutR+i*cols+j) = pow((double((f.img).at<Vec3b>(i,j).val[2]))/255,2);//f.img is MAT 
			*(frameoutG+i*cols+j) = pow((double((f.img).at<Vec3b>(i,j).val[1]))/255,2);
			*(frameoutB+i*cols+j) = pow((double((f.img).at<Vec3b>(i,j).val[0]))/255,2);
		}
	}
	for(int i=0; i<rows;i++){
		for(int j=0; j<cols;j++){
			*(frameout+i*cols+j)=*(frameoutR+i*cols+j)+ *(frameoutG+i*cols+j)+ *(frameoutB+i*cols+j);
			*(frameout+i*cols+j) = sqrt(*(frameout+i*cols+j));
		}
	}
	//caculate the max frame intensity in all frame intensities.
	for(int i=0; i<rows;i++){
		for(int j=0; j<cols;j++){
			maxMag = max(*(frameout+i*cols+j), maxMag);
		}
	}
	//normalization
	for(int i=0; i<rows;i++){
		for(int j=0; j<cols;j++){
			*(frameoutR+i*cols+j) = *(frameoutR+i*cols+j)/maxMag;
			*(frameoutG+i*cols+j) = *(frameoutG+i*cols+j)/maxMag;
			*(frameoutB+i*cols+j) = *(frameoutB+i*cols+j)/maxMag;
		}
	}
//put the nomarlized frame intesities into the image.
	for(int i=0; i<rows;i++){
		for(int j=0; j<cols;j++){
			(f.img).at<Vec3b>(i,j).val[2] = uchar(floor(*(frameoutR+i*cols+j)*255));// convert double to unchar.
			(f.img).at<Vec3b>(i,j).val[1] = uchar(floor(*(frameoutG+i*cols+j)*255));
			(f.img).at<Vec3b>(i,j).val[0] = uchar(floor(*(frameoutB+i*cols+j)*255));
		}
	}
	//delete all temp datasets
	for(int i=0;i<rows*cols;i++){
		frameoutR[i]=NULL;
	}
	delete[] frameoutR;
	frameoutR=NULL;

	for(int i=0;i<rows*cols;i++){
		frameoutG[i]=NULL;
	}
	delete[] frameoutG;
	frameoutG=NULL;

	for(int i=0;i<rows*cols;i++){
		frameoutB[i]=NULL;
	}

	delete[] frameoutB;
	frameoutB=NULL;

	for(int i=0;i<rows*cols;i++){
		frameout[i]=NULL;}

	delete[] frameout;
	frameout=NULL;

}
