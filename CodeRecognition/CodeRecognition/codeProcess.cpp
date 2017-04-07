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
//Ԥ�����Ҷ�ͼ���ں�������outputΪ��ͨ��ͼ������ʾ
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
	
	//������ 2*2�ľ�������
	Mat element1 = getStructuringElement(MORPH_RECT, Size(2, 2),Point(0, 0));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2), Point(1, 1));
	erode(gray, gray, element1);
	dilate(gray, gray, element2);

	//ת��Ϊ3ͨ���Ҷ�ͼ������ʾ
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

bool SortByX(const vector<Point> &v1, const vector<Point> &v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{
	return v1[0].x < v2[0].x;//��������  
}

void splitCode(Mat& grayCode, Mat& identifyingCode, Mat& gray1, Mat& gray2, Mat& gray3, Mat& gray4, Mat& letter1, Mat& letter2, Mat& letter3, Mat& letter4){
	//�������� ��ΪѰ������ʱ����ԭͼ���Ӱ�죬����Ҫ����
	vector<vector<Point>>contours;
	Mat tempGrayCode(code_height, code_width, CV_8UC1);
	threshold(grayCode, tempGrayCode, 48, 255, CV_THRESH_BINARY);

	/*
	mode:CV_RETR_EXTERNAL��ʾֻ���������
	method:CV_CHAIN_APPROX_SIMPLEѹ��ˮƽ���򣬴�ֱ���򣬶Խ��߷����Ԫ�أ�ֻ�����÷�����յ����꣬����һ����������ֻ��4����������������Ϣ
	*/
	findContours(tempGrayCode, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//��������
	sort(contours.begin(), contours.end(), SortByX);

	//������ʾ
	if (identifyingCode.data){
		drawContours(identifyingCode, contours, -1, Scalar(0, 0, 255), 1);
	}
	//������������С��Χ����
	Mat gray[4] = { gray1, gray2, gray3, gray4 };
	int j = 0;
	for (vector<vector<Point>>::iterator i = contours.begin(); i != contours.end(); i++){
		Rect r = boundingRect(Mat(*i));
		Mat temp = grayCode(r);
#if DotSizeShow
		cout << temp.cols*temp.rows << " ";
#endif
		//ȥ��С�Ľ���������i,j����ĵ�ĸ��� (*i).size�Ǳ߿��нǵ�ĸ���,����Ҫ�����
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

	//���Ƶ�3ͨ��ͼ����ʾ
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