// vibe.cpp : Defines the entry point for the console application.
//

// lx_Picture.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "highgui.h"
#include "hanshu.h"
#include <iostream>

using namespace std;
using namespace cv;

int nFrmNum = 0;//记录帧数
int Width;//记录帧的宽度
int Height;//记录帧的高度
int FrameRate;//记录视频帧率

int main(int argc, char* argv[])
{
	IplImage* pFrame=NULL;CvMat* pFrameMat = NULL;//pFrame对象
	IplImage* pAfter=NULL;CvMat* pAfterMat=NULL;//保存pFrame对应的灰度图像
	IplImage* segMap=NULL;CvMat* segMat=NULL;//保存处理后的信息

	//要读取的视频文件和保存的视频文件路径
	char* openfile="E:\\浙大实习\\Input\\AVSS_PV_Easy_Divx_Xvid.avi";
	char* outfile="E:\\View004.avi";
	char filename[100];//保存的图像位置和名称

	//打开视频文件
	CvCapture* pCapture=cvCreateFileCapture(openfile);
	if(pCapture==NULL) {
		cout<<"video file open error!"<<endl;
		return -1;
	}

	//获取视频相关信息，帧率和大小
	double fps=cvGetCaptureProperty(pCapture,CV_CAP_PROP_FPS);
	CvSize size=cvSize((int)cvGetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_WIDTH)*2,
		(int)cvGetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_HEIGHT));

	//创建输出视频文件
	CvVideoWriter* Save_result=NULL;
	Save_result=cvCreateVideoWriter(outfile,CV_FOURCC('X','V','I','D'),fps,size,1);
	IplImage* dstImg=cvCreateImage(size,IPL_DEPTH_8U,3);//创建要保存的图像

	//创建窗口
	cvNamedWindow("video",CV_WINDOW_AUTOSIZE);

	//创建一个随机数生成器
	RNG rng(0xFFFFFFFF);
	//定义字体
	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX_SMALL ,1, 1, 0, 1, 8);

	//逐帧读取视频并进行处理
	while(pFrame = cvQueryFrame( pCapture )){
		
		nFrmNum++;

		//如果是第一帧，申请内存并进行初始化
		if(nFrmNum==1){
			segMap = cvCreateImage(cvSize(pFrame->width, pFrame->height), 
				IPL_DEPTH_8U,1);
			segMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pAfter=cvCreateImage(cvSize(pFrame->width, pFrame->height), 
				IPL_DEPTH_8U,1);
			pAfterMat=cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//转化成单通道图像再处理
			cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
			cvConvert(pAfter, pAfterMat);
			//
			Initialize(pAfterMat,rng);
		}
		else {
			cvCvtColor(pFrame,pAfter,CV_BGR2GRAY);
			cvConvert(pAfter,pAfterMat);
			update(pAfterMat,segMat,rng,nFrmNum);//更新背景
			cvConvert(segMat,segMap);

			//载入原图像到目标图像
			cvSetImageROI(dstImg, cvRect(0, 0, pFrame->width, pFrame->height));
			cvCopy(pFrame, dstImg);
			cvResetImageROI(dstImg);
			//将segMap转换成三通道图像存在pFrame中
			cvCvtColor(segMap,pFrame,CV_GRAY2BGR);
			//载入检测后的图像到目标图像
			cvSetImageROI(dstImg, cvRect(pFrame->width, 0, pFrame->width*2, pFrame->height));
			cvCopy(pFrame, dstImg);
			cvResetImageROI(dstImg);
			//显示提示文字
			cvPutText(dstImg,"Input Video",cvPoint(0,pFrame->height-5),&font,CV_RGB(255,0,0));
			cvPutText(dstImg,"Vibe Segmentation",cvPoint(pFrame->width,pFrame->height-5),&font,CV_RGB(255,0,0));

			//保存视频和输出
			cvWriteFrame(Save_result,dstImg);

			/*输出图片
			if(nFrmNum<11)
				sprintf(filename,"E:\\Result\\Vibe\\AVSS_PV_Easy_Divx_Xvid\\000%d_Vibe.jpg",nFrmNum-1);
			else if(nFrmNum<101)
				sprintf(filename,"E:\\Result\\Vibe\\AVSS_PV_Easy_Divx_Xvid\\00%d_Vibe.jpg",nFrmNum-1);
			else if(nFrmNum<1001)
				sprintf(filename,"E:\\Result\\Vibe\\AVSS_PV_Easy_Divx_Xvid\\0%d_Vibe.jpg",nFrmNum-1);
			else if(nFrmNum<10001)
				sprintf(filename,"E:\\Result\\Vibe\\AVSS_PV_Easy_Divx_Xvid\\%d_Vibe.jpg",nFrmNum-1);
			cvSaveImage(filename,dstImg);
			*/

			cvShowImage("video",dstImg);

			if(cvWaitKey(5)>=0) break;
		}
	}
	cvReleaseImage(&pFrame);cvReleaseMat(&pFrameMat);
	cvReleaseImage(&pAfter);cvReleaseMat(&pAfterMat);
	cvReleaseImage(&segMap);cvReleaseMat(&segMat);
	cvReleaseVideoWriter(&Save_result);
	cvReleaseImage(&dstImg);
	cvDestroyWindow("video");
	return 0;
}


