#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>
#include <string>
#include <ctime>
#include <windows.h>
#include "constant.h"

using namespace std;
using namespace cv;
void preProcess(Mat& image, Mat& gray);
void splitCode(Mat& grayCode, Mat& gray1, Mat& gray2, Mat& gray3, Mat& gray4);

char characters[56] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'n', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
int zt[5] = {0,2,3,4,5};
static Scalar randomColor(RNG& rng)
{
	int icolor = (unsigned)rng;
	return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}
void generateCode(char *code, Mat& codeShow, Mat& identifyingCode){
	int lineType = 8;
	RNG rng(GetTickCount());
	//验证码背景
	int channels = identifyingCode.channels();
	int nRows = identifyingCode.rows ;
	int nCols = identifyingCode.cols*channels;
	if (identifyingCode.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	int i, j;
	uchar* p;
	for (i = 0; i < nRows; ++i)
	{
		p = identifyingCode.ptr<uchar>(i);
		for (j = 0; j < nCols; j+=3)
		{
			int rdm = rng.uniform(0,10);
			if (rdm>6){
				p[j] = rng.uniform(0, 255);
				p[j+1] = rng.uniform(0, 255);
				p[j+2] = rng.uniform(0, 255);
			}
		}
	}
#if Lines
	//干扰线
	int lineNumber = rng.uniform(5,9);
	while (lineNumber--){
		ellipse(identifyingCode,
			Point(rng.uniform(-10, code_width + 10), rng.uniform(-10, code_height + 10)),
			Size(rng.uniform(code_width / 2, code_width), rng.uniform(code_height / 3, code_height)),
			rng.uniform(-90, 90), 0, 360, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 1, 8);
	}
#endif
	Point org;
	//验证码的x轴位置
	int x[4];
	x[0] = rng.uniform(code_width * 1 / 50, code_width * 1 / 20);
	x[1] = rng.uniform(x[0] + code_width / 5, code_width*3 /10 );
	x[2] = rng.uniform(x[1] + code_width / 5, code_width * 11 / 20);
	x[3] = rng.uniform(x[2] + code_width / 5, code_width * 3 / 4);
	//生成待识别的验证码图片
	for (int i = 0; i < 4; i++){
		code[i] = characters[rng.uniform(0, 56)];
		code[i + 1] = '\0';
		string ccode(code+i);
		org.x = x[i];
		org.y = rng.uniform(code_height * 3 / 5, code_height * 5 / 6);
		putText(identifyingCode, ccode, org, zt[rng.uniform(0,5)],
			rng.uniform(18, 20)*0.06 + 0.01, randomColor(rng), 2, 8);
	}

	//生成验证码文字图片
	if (codeShow.data){
		putText(codeShow, code, Point2d(0, 4 * code_height / 5), 4,
			1.3, Scalar(0, 0, 0), 2, 8);
	}
		
	cout << code<<endl;
}
void generateCode(char *code, Mat& identifyingCode){
	generateCode(code, Mat(), identifyingCode);
}

//生成训练集
void generateTrainSet(char* trains){
#if NOFRESH
	if (freopen("train", "r", stdin) != NULL){
	
	}
	else{
#endif
		freopen("train", "w", stdout);
		char code[5];

		//生成验证码
		Mat identifyingCode(code_height, code_width, CV_8UC3,Scalar(255,255,255));
		generateCode(code, identifyingCode);

		//预处理
		Mat gray(code_height, code_width, CV_8UC1);
		preProcess(identifyingCode, gray);
		freopen("CON", "w", stdout);

		//分割
		Mat grayLetter1(code_height,letter_width,CV_8UC1);
		Mat grayLetter2(code_height, letter_width, CV_8UC1);
		Mat grayLetter3(code_height, letter_width, CV_8UC1);
		Mat grayLetter4(code_height, letter_width, CV_8UC1);
		splitCode(gray, grayLetter1, grayLetter2, grayLetter3, grayLetter4);
		
		//特征提取

#if NOFRESH
	}
#endif
}
