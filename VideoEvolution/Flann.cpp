#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "cv.h"
#include "MyLib.h"
#include "Frame.h"

using namespace std;
using namespace cvflann;

extern string RESOURCEPATH;
extern const int ROW;
extern const int COL;
extern const int K;
extern const float DIST_THRE;
extern int labels[ROW];//cluster label


///@function - find the 'root' of a cluster
static int find(int u){ 
  return u == labels[u] ? u : labels[u] = find(labels[u]);
}
/* @function - use the smaller label for the two clusters to union
   u: the smaller one
   v: the larger one
*/
static void myunion(int u,int v){
	int pv = find(v);
	int pu = find(u);
	
	if(labels[pv]>=labels[pu]){
		labels[pv] = labels[pu];
	}
	else{
		labels[pu] = labels[pv];
	}
}

static void FLANN (vector<Frame> frameLib)
{
	///******** step 1: read ACC matrix from "ACC.txt" file ***********
	float **results = new float*[ROW];
	for(int i=0;i<ROW;i++){
		results[i] = new float[COL]; 
	}

	ifstream infile;
	infile.open ("histogramvector.txt");
	string line;
	int row=0;
	if (infile.is_open())
	{
		while ( infile.good() )
		{
			getline(infile,line);
			istringstream iss(line);
			int col=0;
			while(iss)
			{
				string word;
				iss >> word;
				if(word=="") break;
				float number = atof(word.c_str());
				results[row][col]=number;
				col++;
				if(col==COL) break;
			} 
			row++;
			if(row==ROW) break;
		}
	}
	infile.close();

	//************ step 2: insert matrix into Mat object for FLANN use. **********
	//=========== Using opencv::flann, cannot set distance parameters ============
	cv::Mat flann_m = cv::Mat(ROW,COL-2,CV_32F); // used for FLANN, data matrix
	for(int i=0;i<ROW;i++){
		for(int j=0;j<COL-2;j++){
			flann_m.at<float>(i,j)=results[i][j];
		}
	}

	//// do indexing
	Matrix<float> samplesMatrix((float*)flann_m.data, flann_m.rows, flann_m.cols);
	//Index<cvflann::ChiSquareDistance<float>> flann_index(samplesMatrix, cvflann::LinearIndexParams());
	Index<cvflann::L1<float>> flann_index(samplesMatrix, cvflann::KDTreeIndexParams());
	flann_index.buildIndex();

	cv::Mat1i ind(flann_m.rows, K);
	CV_Assert(ind.isContinuous());
	cvflann::Matrix<int> nresps((int*) ind.data, ind.rows, ind.cols);
	cvflann::Matrix<float> dist(new float[flann_m.rows*K], flann_m.rows, K);

	flann_index.knnSearch(samplesMatrix,nresps,dist,K,SearchParams(64));

	 //write distance to file
	string distance ="KNNdistance.txt"; 
	ofstream out_dist(distance,ios::trunc);
	for(int i=0;i<ROW;i++){
		for(int j=0;j<K;j++){
			out_dist<<dist[i][j]<<" ";
		}
		if(i!=ROW-1){
			out_dist<<endl;
		}

	}
	out_dist.close();

	//************** step 3: Union Find *****************
	for(int i=0;i<ROW;++i) labels[i] = i;

	for(int i=0;i<ROW;i++){ //i-th pivot frame
		cout<<i<<endl;
		for(int j=0;j<K;j++){ //j-th neighbor frame
			if(dist[i][j]< 15){ // (1) distance within the threshold
				int neighbor = nresps[i][j];
				//if(results[i][COL-2]!=results[neighbor][COL-2])// (2) not from the same video
				myunion(i,neighbor);
			}
		}
	}

	//********** step 4: save FLANN and FRM results to file ****************
	string path ="FLANN_index.txt"; 
	string path1 = "FLANN_dist.txt";
	ofstream out(path,ios::app);
	ofstream out1(path1,ios::app);
	if(!out || !out1){
		cout<<"Cannot open the file!"<<endl;
		return;
	}

	for(int i=0;i<ROW;i++){
		for(int j=0;j<K;j++){
			out<<nresps[i][j]<<" ";
			out1<<dist[i][j]<<" " ;
		}
		if(i!=ROW-1){
			out<<"\n";
			out1<<"\n";
		}
	}
	out.close();
	out1.close();

	string path_t = "Frame_Cluster.txt";
	ofstream out_t(path_t,ios::app);
	if(!out_t){
		cout<<"Cannot open the file!"<<endl;
		return;
	}
	for(int i=0;i<ROW;i++){
			out_t<<labels[i]<<" ";
	}

	out_t<<endl;
	out_t.close();


	//======================2013-10-22: print cluster======================
	map<int,vector<int>> cluster_frame;
	map<int,vector<int>>::iterator iter;

	for(int i=0;i<ROW;i++){
		iter = cluster_frame.find(labels[i]);
		
		if(iter!=cluster_frame.end()){
		//already exist
			vector<int> tmp = iter->second;
			tmp.push_back(i);
			cluster_frame.erase(labels[i]);
			cluster_frame.insert(pair<int,vector<int>>(labels[i],tmp));
		}
		else{
			//add a new item
			vector<int> tmp;
			tmp.push_back(i);
			cluster_frame.insert(pair<int,vector<int>>(labels[i],tmp));
		}
	}
	ofstream outcluster("ClusterSet",ios::app);
	if(!outcluster){
		cout<<"Cannot open the file!"<<endl;
		return;
	}
	map<int,vector<int>>::iterator iter2;
	for(iter2=cluster_frame.begin();iter2!=cluster_frame.end();iter2++){
		outcluster<<"Cluster"<<iter2->first<<": ";
		vector<int> tmp = iter2->second;
		for(int i=0;i<tmp.size();i++){
			if(i!=tmp.size()-1) outcluster<<tmp[i]<<"("<<frameLib[tmp[i]].getVID()<<"),";
			else outcluster<<tmp[i]<<"("<<frameLib[tmp[i]].getVID()<<")"<<endl;
		}
	}

	outcluster.close();

	

	for(int i=0;i<ROW;i++){
		delete[] results[i];
	}
	delete[] results;
}