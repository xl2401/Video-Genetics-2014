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
	//            CV_INTER_NN - ����ڲ�ֵ,
	//             CV_INTER_LINEAR - ˫���Բ�ֵ (ȱʡʹ��)
	//             CV_INTER_AREA - ʹ�����ع�ϵ�ز�������ͼ����Сʱ�򣬸÷������Ա��Ⲩ�Ƴ��֡�
         /*��ͼ��Ŵ�ʱ�������� CV_INTER_NN ����..*/
	//             CV_INTER_CUBIC - ������ֵ.

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