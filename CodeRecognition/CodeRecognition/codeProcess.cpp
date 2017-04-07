#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>
#include <string>
#include <cstdio>
#include "constant.h"
using namespace std;
using namespace cv;
//预处理，灰度图用于后续处理，output为三通道图用于显示
void preProcess(Mat& image, Mat& gray, Mat& output){
	cvtColor(image, gray, CV_BGR2GRAY);
	Mat hist;
	Mat lut(1, 256, CV_8U);
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	calcHist(&gray, 1, 0,Mat(),hist,1,&histSize,&histRange,true,false);
	int i;
	uchar* p,*q;
	p = hist.ptr<uchar>(0);
	q = lut.ptr<uchar>(0);
	for (i = 0; i < 256; i++)
	{
		int fre=cvRound(hist.at<float>(i));
		q[i] = fre < 25 ? 0 :fre>1000?0: 255;
	}
	LUT(gray, lut, gray);
	
	//开运算 2*2的矩形掩码
	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 2),Point(0, 0));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(1, 1));
	erode(gray, gray, element1);
	dilate(gray, gray, element2);

	//转换为3通道灰度图用于显示
	int row = gray.rows;
	int col = gray.cols;
	if (output.data){
		for (i = 0; i < row; i++){
			p = gray.ptr<uchar>(i);
			q = output.ptr<uchar>(i);
			for (int j = 0; j < col; j++){
				q[3 * j] = q[3 * j + 1] = q[3 * j + 2] = p[j];
			}
		}
	}
}
void preProcess(Mat& image, Mat& gray){
	preProcess(image, gray, Mat());
}

bool SortByX(const vector<Point> &v1, const vector<Point> &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1[0].x < v2[0].x;//升序排列  
}

void splitCode(Mat& grayCode, Mat& identifyingCode, Mat& gray1, Mat& gray2, Mat& gray3, Mat& gray4, Mat& letter1, Mat& letter2, Mat& letter3, Mat& letter4){
	//轮廓查找 因为寻找轮廓时候会对原图造成影响，故需要备份
	vector<vector<Point>>contours;
	Mat tempGrayCode(code_height, code_width, CV_8UC1);
	threshold(grayCode, tempGrayCode, 48, 255, CV_THRESH_BINARY);

	/*
	mode:CV_RETR_EXTERNAL表示只检测外轮廓
	method:CV_CHAIN_APPROX_SIMPLE压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
	*/
	findContours(tempGrayCode, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//轮廓排序
	sort(contours.begin(), contours.end(), SortByX);

	//轮廓显示
	if (identifyingCode.data){
		drawContours(identifyingCode, contours, -1, Scalar(0, 0, 255), 1);
	}
	//各个轮廓的最小包围矩形
	Mat gray[4] = { gray1, gray2, gray3, gray4 };
	int j = 0;
	for (vector<vector<Point>>::iterator i = contours.begin(); i != contours.end(); i++){
		Rect r = boundingRect(Mat(*i));
		Mat temp = grayCode(r);
#if DotSizeShow
		cout << temp.cols*temp.rows << " ";
#endif
		//去除小的椒盐噪声和i,j上面的点的干扰 (*i).size是边框中角点的个数,所以要求面积
		if (temp.cols*temp.rows < 90) {
			i=contours.erase(i);
			i--;
			continue;
		}
		resize(temp, gray[j++], Size(letter_width, code_height), 0, 0);
		if (identifyingCode.data){
			rectangle(identifyingCode, r, Scalar(0, 255, 0), 1);
		}
	}
#if DotSizeShow
	cout << endl;
#endif

	//复制到3通道图以显示
	if (letter1.data){
		Mat letter[4] = { letter1, letter2, letter3, letter4 };
		int row = gray1.rows;
		int col = gray1.cols;
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < row; j++){
				uchar *p = gray[i].ptr<uchar>(j);
				uchar *q = letter[i].ptr<uchar>(j);
				for (int k = 0; k < col; k++){
					q[3 * k] = q[3 * k + 1] = q[3 * k + 2] = p[k];
				}
			}
		}
	}
}
void splitCode(Mat& grayCode,  Mat& gray1, Mat& gray2, Mat& gray3, Mat& gray4){
	splitCode(grayCode, Mat(), gray1, gray2, gray3, gray4, Mat(), Mat(), Mat(), Mat() );
}

void extractPattern(Mat& rio, double* hog){
	
}

char getLetter(Mat& rio){
	double hog[9];
	extractPattern(rio,hog);
	return '0';
}