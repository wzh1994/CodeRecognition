#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>
#include <ctime>
#include <windows.h>
#include "constant.h"

using namespace std;
using namespace cv;

void generateCode(char *code, Mat& codeShow, Mat& identifyingCode);
int splitCode(Mat& grayCode, Mat& identifyingCode, double pt[][Patterns],Mat& letter1, Mat& letter2, Mat& letter3, Mat& letter4);
int getLetter(double* pt, double trainSet[][Patterns], int* lables, PTArgs ptArg);
void preProcess(Mat& image, Mat& gray,Mat& output);
void generateTrainSet(double trainSet[][Patterns], int* lables, PTArgs ptArg);
void generateParzenArgs(double trainSet[][Patterns], int* lables);
void showPatterns(double* pt);
void makeTitle(Mat& Background);
RNG rng(GetTickCount());
int main(){
	/*生成训练集合*/
#if NOFRESH
	double trainSet[KNN_N][Patterns]; //训练集
	int lables[KNN_N];				  //标签集
#else
	double trainSet[TrainSize][Patterns]; //训练集
	int lables[TrainSize];				  //标签集
#endif
	PTArgsNode ptArg;
	generateTrainSet(trainSet, lables, &ptArg);
	generateParzenArgs(trainSet, lables);

#if DOTEST
	/*生成测试集*/
	Mat Background(window_height, window_width, CV_8UC3, Scalar(192, 192, 192));
	int n = 0;
	//Background为集成显示的图像，n为第n+1个测试验证码

	makeTitle(Background);
	do{
		Mat identifyingCode(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
		Mat codeShow(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
		char code[5];
		int start = 0;
		//生成并显示待验证的验证码图片
		generateCode(code, codeShow,identifyingCode);
		Mat imageRIO1;
		ADDRIO(imageRIO1,Background, code_width, n, start);
		identifyingCode.copyTo(imageRIO1);

		//预处理验证码
		Mat imageRIO2;
		ADDRIO(imageRIO2, Background, code_width, n, start);
		Mat gray(code_height,code_width,CV_8UC1);
		preProcess(identifyingCode,gray, imageRIO2);

		//分割预处理后的验证码图片
		Mat imageRIO3, imageRIO4, imageRIO5,imageRIO6;
		ADDRIO(imageRIO3, Background, letter_width, n, start);
		ADDRIO(imageRIO4, Background, letter_width, n, start);
		ADDRIO(imageRIO5, Background, letter_width, n, start);
		ADDRIO(imageRIO6, Background, letter_width, n, start);
		double pt[4][Patterns];
		if (!splitCode(gray, imageRIO2, pt, imageRIO3, imageRIO4, imageRIO5, imageRIO6)){
			n--;
			continue;
		}

		//识别验证码
		Mat imageRIO7;
		ADDRIO(imageRIO7, Background, code_width, n, start);
		int result[4];
		result[0] = getLetter(pt[0], trainSet, lables, &ptArg);
		result[1] = getLetter(pt[1], trainSet, lables, &ptArg);
		result[2] = getLetter(pt[2], trainSet, lables, &ptArg);
		result[3] = getLetter(pt[3], trainSet, lables, &ptArg);
		char resultLetter[10];
		for (int i = 0; i < 4; i++){
			resultLetter[i] = (result[i] >> 8);
			resultLetter[i+5] = result[i]%256;
		}
		resultLetter[4] = '|';
		resultLetter[9] = '\0';

		//显示识别结果
		Mat resultShow(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
		putText(resultShow, resultLetter, Point2d(0, 28), 4, 0.6, Scalar(0, 0, 0), 1, 8);
		resultShow.copyTo(imageRIO7);

		//输出正确的验证码
		Mat imageRIO8;
		ADDRIO(imageRIO8, Background, code_width, n, start);
		codeShow.copyTo(imageRIO8);
	} while (++n<TestSize);

	namedWindow("MainWindow", 1);
	imshow("MainWindow",Background);
#endif
	waitKey();
}

void makeTitle(Mat& Background){
	int start = 0;
	Mat rio1, rio2, rio3, rio4,rio5;
	ADDRIO(rio1, Background, code_width, -1, start);
	putText(rio1, "CodeImage", Point2d(0, 4 * code_height / 5), 4,
		0.7, Scalar(0, 0, 0), 1, 8);
	ADDRIO(rio2, Background, code_width, -1, start);
	putText(rio2, "Extraction", Point2d(0, 4 * code_height / 5), 4,
		0.7, Scalar(0, 0, 0), 1, 8);
	ADDRIO(rio3, Background, letter_width*4+gap*3, -1, start);
	putText(rio3, "Split", Point2d(0, 4 * code_height / 5), 4,
		1, Scalar(0, 0, 0), 1, 8);
	ADDRIO(rio4, Background, code_width, -1, start);
	putText(rio4, "Result", Point2d(0, 4 * code_height / 5), 4,
		1, Scalar(0, 0, 0), 1, 8);
	ADDRIO(rio5, Background, code_width, -1, start);
	putText(rio5, "Code", Point2d(0, 4 * code_height / 5), 4,
		1, Scalar(0, 0, 0), 1, 8);
}