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
#if MaxFourColor
	int colorFre[5] = {0,0,0,0,0};
#endif
	for (i = 0; i < 256; i++)
	{
		int fre=cvRound(hist.at<float>(i));
		q[i] = fre < 75 ? 0 :fre>1000?0: 255;
#if MaxFourColor
		if (fre>colorFre[0]){
			colorFre[4] = colorFre[3];
			colorFre[3] = colorFre[2];
			colorFre[2] = colorFre[1];
			colorFre[1] = colorFre[0];
			colorFre[0] = fre;
		}
		else if (fre > colorFre[1]){
			colorFre[4] = colorFre[3];
			colorFre[3] = colorFre[2];
			colorFre[2] = colorFre[1];
			colorFre[1] = fre;
		}
		else if (fre > colorFre[2]){
			colorFre[4] = colorFre[3];
			colorFre[3] = colorFre[2];
			colorFre[2] = fre;
		}
		else if (fre > colorFre[3]){
			colorFre[4] = colorFre[3];
			colorFre[3] = fre;
		}
		else if (fre > colorFre[4]){
			colorFre[4] = fre;
		}
#endif
	}
#if MaxFourColor
	cout << colorFre[1] << " " << colorFre[2] << " " << colorFre[3] << " " << colorFre[4] << endl;
#endif
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

//�ָ�ͼ��gray��δ������С�ĵ�ͨ���Ҷ�ͼ������������ȡ��letter�ǵ����˴�С����ͨ��ͼ��������ʾ
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
	vector<vector<Point>> fill;
	for (vector<vector<Point>>::iterator i = contours.begin(); i != contours.end();){
		Rect r = boundingRect(Mat(*i));
		Mat temp = grayCode(r);
#if DotSizeShow
		cout <<i-contours.begin()<<":"<< temp.cols*temp.rows << " ";
#endif
		//ȥ��С�Ľ���������i,j����ĵ�ĸ��� (*i).size�Ǳ߿��нǵ�ĸ���,����Ҫ�����
		if (temp.cols*temp.rows < 100) {
			fill.push_back(*i);
			i=contours.erase(i);
			continue;
		}
		//��ֹi==contours.begin()ʱԽ�����
		else{
			i++;
		}
		if (identifyingCode.data){
			rectangle(identifyingCode, r, Scalar(0, 255, 0), 1);
		}
	}
#if DotSizeShow
	cout << endl;
#endif
	//�ñ���ɫ���Ҷ�ͼ��С����
	drawContours(grayCode, fill, -1, 0, -1);

	//��ȡ�ַ�����
	int j = 0;
	Mat temp[4];
	for (vector<vector<Point>>::iterator i = contours.begin(); i != contours.end();i++){
		Rect r = boundingRect(Mat(*i));
		temp[i - contours.begin()] = grayCode(r);
		temp[i - contours.begin()].copyTo(gray[i - contours.begin()]);
		resize(temp[i - contours.begin()], temp[i - contours.begin()], Size(letter_width, code_height), 0, 0);
		//resize(temp, gray[j++], Size(), fr, fr);
	}

	//���Ƶ�3ͨ��ͼ����ʾ
	if (letter1.data){
		Mat letter[4] = { letter1, letter2, letter3, letter4 };
		int row = gray1.rows;
		int col = gray1.cols;
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < row; j++){
				uchar *p = temp[i].ptr<uchar>(j);
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