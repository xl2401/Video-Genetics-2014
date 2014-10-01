#pragma once
#include "Frame.h"
#include "MyLib.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "math.h"
#include "Video.h"

static Frame *resize(Frame &f,int height, int width)
{
	  IplImage *dst=0;
	  IplImage src=f.img;
	  CvSize dst_cvsize;
	    dst_cvsize.width=(int)(width);
       dst_cvsize.height=(int)(height);

	  dst=cvCreateImage(dst_cvsize,src.depth,src.nChannels);
	  cvResize(&src,dst,CV_INTER_NN);//
	  Mat tmp=dst;
	  Frame * f1 = new Frame(tmp,f.getvideoName(),f.getVID(),f.getFID(),f.getrealFID());
	  return f1;
	//            CV_INTER_NN - 最近邻插值,
	//             CV_INTER_LINEAR - 双线性插值 (缺省使用)
	//             CV_INTER_AREA - 使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现。
         /*当图像放大时，类似于 CV_INTER_NN 方法..*/
	//             CV_INTER_CUBIC - 立方插值.

}
static void Resizeimg(Video *video, int height, int width, vector<Frame> &resizedFrameLib)
{
	for( int i=0;i<video->GetFrameList().size();i++)
	{
		Frame *tmp=resize(video->GetFrameList()[i], height, width);
		video->AddresizedFrameList(*tmp);
		resizedFrameLib.push_back(*tmp);
	}
}