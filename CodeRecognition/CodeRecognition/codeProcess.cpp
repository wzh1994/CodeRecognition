#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>
#include <string>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <cmath>
#include "constant.h"
using namespace std;
using namespace cv;

void calcPattern(Mat& grayCode, vector<Point> contours, double* pt);

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
#if MaxFourColor
	//统计最高频率的5个灰度
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

//分割图像，letter是调整了大小的三通道图，用于显示
void splitCode(Mat& grayCode, Mat& identifyingCode, double pt[][Patterns],Mat& letter1, Mat& letter2, Mat& letter3, Mat& letter4){
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
	vector<vector<Point>> fill;
	for (vector<vector<Point>>::iterator i = contours.begin(); i != contours.end();){
		Rect r = boundingRect(Mat(*i));
		Mat temp = grayCode(r);
#if DotSizeShow
		cout <<i-contours.begin()<<":"<< temp.cols*temp.rows << " ";
#endif
		//去除小的椒盐噪声和i,j上面的点的干扰 (*i).size是边框中角点的个数,所以要求面积
		if (temp.cols*temp.rows < 100) {
			fill.push_back(*i);
			i=contours.erase(i);
			continue;
		}
		//防止i==contours.begin()时越界出错
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
	//用背景色填充灰度图的小区域
	drawContours(grayCode, fill, -1, 0, -1);

	//提取字符区域
	Mat temp[4];
	for (vector<vector<Point>>::iterator i = contours.begin(); i != contours.end();i++){
		Rect r = boundingRect(Mat(*i));
		temp[i - contours.begin()] = grayCode(r);
		calcPattern(temp[i - contours.begin()], *i, pt[i - contours.begin()]);
		resize(temp[i - contours.begin()], temp[i - contours.begin()], Size(letter_width, code_height), 0, 0, INTER_CUBIC);
	}
#if PatternShow
	for (int i = 0; i < 4; i++){
		cout << i << ":";
		for (int j = 0; j < Patterns; j++){
			cout << setiosflags(ios::fixed) << setprecision(8) << pt[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
#endif

	//复制到3通道图以显示
	if (letter1.data){
		Mat letter[4] = { letter1, letter2, letter3, letter4 };
		int row = temp[1].rows;
		int col = temp[1].cols;
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
void splitCode(Mat& grayCode, double pt[][Patterns]){
	splitCode(grayCode, Mat(), pt, Mat(), Mat(), Mat(), Mat());
}

void calcPattern(Mat& grayCode, vector<Point> contours, double* pt){
	int n = 0;
	//计算矩
	Moments mu = moments(contours, false);
	pt[n++] = mu.m00;	//特征1:轮廓面积
	pt[n++] = mu.mu11;  //特征2:中心矩
	pt[n++] = arcLength(contours, true); //特征3：轮廓长度
	double hu[8] = { 0 };
	HuMoments(mu, hu);
	pt[n++] = hu[0];  //特征4：hu矩1
	pt[n++] = hu[1];  //特征5：hu矩2

	//直方图计算
	int col = grayCode.cols;
	int row = grayCode.rows;
	uchar *p;
	int colHist[letter_width]; //水平分布直方图
	int rowHist[code_height];  //垂直分布直方图
	memset(rowHist, 0, sizeof(rowHist));
	memset(colHist, 0, sizeof(colHist));
	for (int i = 0; i < row; i++){
		p = grayCode.ptr<uchar>(i);
		for (int j = 0; j < col; j++){
			if (p[j]>100){
				rowHist[i]++;
				colHist[j]++;
			}
		}
	}
	//水平分布直方图方差
	int sum=0;
	double ave = 0;
	double accum = 0;
	for (int i = 0; i < row; i++){
		sum += rowHist[i];
	}
	ave = sum / (double)row;
	for (int i = 0; i < row; i++){
		accum += (rowHist[i] - ave)*(rowHist[i] - ave);
	}
	pt[n++] = sqrt(accum);  //特征6：水平分布直方图标准差

	//垂直分布直方图方差
	sum = 0;
	ave = 0;
	accum = 0;
	for (int j = 0; j < col; j++){
		sum += colHist[j];
	}
	ave = sum / (double)col;
	for (int j = 0; j < col; j++){
		accum += (colHist[j] - ave)*(colHist[j] - ave);
	}
	pt[n++] = sqrt(accum);  //特征7：水平分布直方图标准差
	
}

char getLetter(double pt[]){
	
	return '0';
}