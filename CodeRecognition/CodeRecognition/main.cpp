#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>

#include "constant.h"

using namespace std;
using namespace cv;

void generateCode(char *code, Mat& codeShow, Mat& identifyingCode);
int splitCode(Mat& grayCode, Mat& identifyingCode, double pt[][Patterns],Mat& letter1, Mat& letter2, Mat& letter3, Mat& letter4);
char getLetter(double* pt, double trainSet[][Patterns], int* lables, PTArgs ptArg);
void preProcess(Mat& image, Mat& gray,Mat& output);
void generateTrainSet(double trainSet[][Patterns], int* lables, PTArgs ptArg);
void showPatterns(double* pt);
void makeTitle(Mat& Background);
int main(){
	/*����ѵ������*/
	double trainSet[TrainSize][Patterns]; //ѵ����
	int lables[TrainSize];				  //��ǩ��
	PTArgsNode ptArg;

	generateTrainSet(trainSet, lables, &ptArg);

#if DOTEST
	/*���ɲ��Լ�*/
	Mat Background(window_height, window_width, CV_8UC3, Scalar(192, 192, 192));
	int n = 0;
	//BackgroundΪ������ʾ��ͼ��nΪ��n+1��������֤��

	makeTitle(Background);
	do{
		Mat identifyingCode(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
		Mat codeShow(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
		char code[5];
		int start = 0;
		//���ɲ���ʾ����֤����֤��ͼƬ
		generateCode(code, codeShow,identifyingCode);
		Mat imageRIO1;
		ADDRIO(imageRIO1,Background, code_width, n, start);
		identifyingCode.copyTo(imageRIO1);

		//Ԥ������֤��
		Mat imageRIO2;
		ADDRIO(imageRIO2, Background, code_width, n, start);
		Mat gray(code_height,code_width,CV_8UC1);
		preProcess(identifyingCode,gray, imageRIO2);

		//�ָ�Ԥ��������֤��ͼƬ
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

		//ʶ����֤��
		Mat imageRIO7;
		ADDRIO(imageRIO7, Background, code_width, n, start);
		char result[5];
		result[0] = getLetter(pt[0], trainSet, lables, &ptArg);
		result[1] = getLetter(pt[1], trainSet, lables, &ptArg);
		result[2] = getLetter(pt[2], trainSet, lables, &ptArg);
		result[3] = getLetter(pt[3], trainSet, lables, &ptArg);
		result[4] = '\0';

		//��ʾʶ����
		Mat resultShow(code_height, code_width, CV_8UC3, Scalar(255, 255, 255));
		putText(resultShow, result, Point2d(0, 28), 4, 0.8, Scalar(0, 0, 0), 1, 8);
		resultShow.copyTo(imageRIO7);

		//�����ȷ����֤��
		Mat imageRIO8;
		ADDRIO(imageRIO8, Background, code_width, n, start);
		codeShow.copyTo(imageRIO8);

		//�ȴ�
		for (int i = 0; i < 2000; i++){
			for (int j = 0; j < 2000; j++);
		}
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