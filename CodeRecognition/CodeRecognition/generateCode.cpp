#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>
#include <string>
#include <cstring>
#include <ctime>
#include <windows.h>
#include <cmath>
#include "constant.h"

using namespace std;
using namespace cv;
void preProcess(Mat& image, Mat& gray);
int splitCode(Mat& grayCode, double pt[][Patterns]);

char characters[58] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'n', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H','I', 'J', 'K', 'L', 'M', 'N','O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
int zt[5] = {0,2,3,4,5};  //五种字体
static Scalar randomColor(RNG& rng)
{
	int icolor = (unsigned)rng;
	return Scalar(icolor & 255, (icolor >> 8) & 255, (icolor >> 16) & 255);
}
void generateCode(char *code, Mat& codeShow, Mat& identifyingCode,int* lables){
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
		int temp = rng.uniform(0, 58);
		if (lables) lables[i] = temp;
		code[i] = characters[temp];
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
		
	//cout << code<<endl;
}
void generateCode(char *code, Mat& identifyingCode,int* lables){
	generateCode(code, Mat(), identifyingCode,lables);
}
void generateCode(char *code, Mat& codeShow, Mat& identifyingCode){
	generateCode(code, codeShow, identifyingCode, NULL);
}

//生成训练集
void generateTrainSet(double trainSet[][Patterns], int* lables, double * means, double * sDeviation){
#if NOFRESH
	if (freopen("train", "r", stdin) != NULL){
		for (int i=0;i<Patterns;i++){
			cin>>means[i]>>sDeviation[i];
		}
		for (int i=0;i<TrainSize;i++){
			for (int j=0;j<Patterns;j++){
				cin>>trainSet[i][j];
			}
			cin >> lables[i];
		}
		freopen("CON", "r", stdin);
	}
	else{
#endif
		freopen("train", "w", stdout);
		char code[5];
		int n = TrainSize/4;
		for (int i = 0; i < n; i++){
			//生成验证码
			Mat identifyingCode(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
			generateCode(code, identifyingCode, lables+i*4);

			//预处理
			Mat gray(code_height, code_width, CV_8UC1);
			preProcess(identifyingCode, gray);
			//分割和提取特征
			if (!splitCode(gray, trainSet + i * 4)){
				i--;
				for (int j = 0; j < 2000; j++){
					for (int k = 0; k < 2000; k++);
				}
				continue;
			}
			for (int j = 0; j < 2000; j++){
				for (int k = 0; k < 2000; k++);
			}
			
		}
		//计算每一列的均值和标准差
		for (int i = 0; i < TrainSize; i++){
			for (int j = 0; j < Patterns; j++){
				means[j] += trainSet[i][j];
			}
		}
		for (int i = 0; i < Patterns; i++){
			means[i] = means[i]/ TrainSize;
		}
		//计算每一列的标准差
		for (int i = 0; i < TrainSize; i++){
			for (int j = 0; j < Patterns; j++){
				sDeviation[j] += (trainSet[i][j] - means[j])*(trainSet[i][j] - means[j]);
			}
		}
		for (int i = 0; i < Patterns; i++){
			sDeviation[i] = sqrt(sDeviation[i]);
		}
#if	Standardize
		//列向量归一化
		for (int i = 0; i < TrainSize; i++){
			for (int j = 0; j < Patterns; j++){
				trainSet[i][j] = (trainSet[i][j] - means[j]) / sDeviation[j];
			}
		}
#endif
		//输出到文件
		for (int i = 0; i < Patterns; i++){
			cout << means[i]<<" "<<sDeviation[i] << " ";
		}
		cout << endl;
		for (int i = 0; i < TrainSize; i++){
			for (int j= 0; j < Patterns; j++){
				cout << trainSet[i][j] << " ";
			}
			cout << lables[i] << endl;
		}
		freopen("CON", "w", stdout);
#if NOFRESH
	}
#endif
}
