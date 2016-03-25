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

int nFrmNum = 0;//��¼֡��
int Width;//��¼֡�Ŀ��
int Height;//��¼֡�ĸ߶�
int FrameRate;//��¼��Ƶ֡��

int main(int argc, char* argv[])
{
	IplImage* pFrame=NULL;CvMat* pFrameMat = NULL;//pFrame����
	IplImage* pAfter=NULL;CvMat* pAfterMat=NULL;//����pFrame��Ӧ�ĻҶ�ͼ��
	IplImage* segMap=NULL;CvMat* segMat=NULL;//���洦������Ϣ

	//Ҫ��ȡ����Ƶ�ļ��ͱ������Ƶ�ļ�·��
	char* openfile="E:\\���ʵϰ\\Input\\AVSS_PV_Easy_Divx_Xvid.avi";
	char* outfile="E:\\View004.avi";
	char filename[100];//�����ͼ��λ�ú�����

	//����Ƶ�ļ�
	CvCapture* pCapture=cvCreateFileCapture(openfile);
	if(pCapture==NULL) {
		cout<<"video file open error!"<<endl;
		return -1;
	}

	//��ȡ��Ƶ�����Ϣ��֡�ʺʹ�С
	double fps=cvGetCaptureProperty(pCapture,CV_CAP_PROP_FPS);
	CvSize size=cvSize((int)cvGetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_WIDTH)*2,
		(int)cvGetCaptureProperty(pCapture,CV_CAP_PROP_FRAME_HEIGHT));

	//���������Ƶ�ļ�
	CvVideoWriter* Save_result=NULL;
	Save_result=cvCreateVideoWriter(outfile,CV_FOURCC('X','V','I','D'),fps,size,1);
	IplImage* dstImg=cvCreateImage(size,IPL_DEPTH_8U,3);//����Ҫ�����ͼ��

	//��������
	cvNamedWindow("video",CV_WINDOW_AUTOSIZE);

	//����һ�������������
	RNG rng(0xFFFFFFFF);
	//��������
	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX_SMALL ,1, 1, 0, 1, 8);

	//��֡��ȡ��Ƶ�����д���
	while(pFrame = cvQueryFrame( pCapture )){
		
		nFrmNum++;

		//����ǵ�һ֡�������ڴ沢���г�ʼ��
		if(nFrmNum==1){
			segMap = cvCreateImage(cvSize(pFrame->width, pFrame->height), 
				IPL_DEPTH_8U,1);
			segMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
			pAfter=cvCreateImage(cvSize(pFrame->width, pFrame->height), 
				IPL_DEPTH_8U,1);
			pAfterMat=cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

			//ת���ɵ�ͨ��ͼ���ٴ���
			cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
			cvConvert(pAfter, pAfterMat);
			//
			Initialize(pAfterMat,rng);
		}
		else {
			cvCvtColor(pFrame,pAfter,CV_BGR2GRAY);
			cvConvert(pAfter,pAfterMat);
			update(pAfterMat,segMat,rng,nFrmNum);//���±���
			cvConvert(segMat,segMap);

			//����ԭͼ��Ŀ��ͼ��
			cvSetImageROI(dstImg, cvRect(0, 0, pFrame->width, pFrame->height));
			cvCopy(pFrame, dstImg);
			cvResetImageROI(dstImg);
			//��segMapת������ͨ��ͼ�����pFrame��
			cvCvtColor(segMap,pFrame,CV_GRAY2BGR);
			//��������ͼ��Ŀ��ͼ��
			cvSetImageROI(dstImg, cvRect(pFrame->width, 0, pFrame->width*2, pFrame->height));
			cvCopy(pFrame, dstImg);
			cvResetImageROI(dstImg);
			//��ʾ��ʾ����
			cvPutText(dstImg,"Input Video",cvPoint(0,pFrame->height-5),&font,CV_RGB(255,0,0));
			cvPutText(dstImg,"Vibe Segmentation",cvPoint(pFrame->width,pFrame->height-5),&font,CV_RGB(255,0,0));

			//������Ƶ�����
			cvWriteFrame(Save_result,dstImg);

			/*���ͼƬ
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


