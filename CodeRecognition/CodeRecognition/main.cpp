#include <iostream>
#include<core/core.hpp>  
#include<highgui/highgui.hpp>  
#include<opencv.hpp>
#include<imgproc\imgproc.hpp>

#include "constant.h"

using namespace std;
using namespace cv;

void generateCode(char *code, Mat& codeShow, Mat& identifyingCode);
void splitCode(Mat& grayCode, Mat& identifyingCode, Mat& gray1, Mat& gray2, Mat& gray3, Mat& gray4, Mat& letter1, Mat& letter2, Mat& letter3, Mat& letter4);
char getLetter(Mat& rio);
void preProcess(Mat& image, Mat& gray,Mat& output);
void generateTrainSet(char* trains);
int main(){
	/*����ѵ������*/
	char trains[6000];//ѵ���� �ṹ����
	generateTrainSet(trains);

#if DOTEST
	/*���ɲ��Լ�*/
	Mat Background(window_height, window_width, CV_8UC3, Scalar(192, 192, 192));
	int n = 0;
	//BackgroundΪ������ʾ��ͼ��nΪ��n+1��������֤��

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
		Mat grayLetter1(code_height,letter_width,CV_8UC1);
		Mat grayLetter2(code_height, letter_width, CV_8UC1);
		Mat grayLetter3(code_height, letter_width, CV_8UC1);
		Mat grayLetter4(code_height, letter_width, CV_8UC1);
		ADDRIO(imageRIO3, Background, letter_width, n, start);
		ADDRIO(imageRIO4, Background, letter_width, n, start);
		ADDRIO(imageRIO5, Background, letter_width, n, start);
		ADDRIO(imageRIO6, Background, letter_width, n, start);

		splitCode(gray, imageRIO2,grayLetter1, grayLetter2, grayLetter3, grayLetter4, imageRIO3, imageRIO4, imageRIO5, imageRIO6);
		
		//ʶ����֤��
		Mat imageRIO7;
		ADDRIO(imageRIO7, Background, code_width, n, start);
		char result[5];
		result[0] = getLetter(grayLetter1);
		result[1] = getLetter(grayLetter2);
		result[2] = getLetter(grayLetter3);
		result[3] = getLetter(grayLetter4);
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
		for (int i = 0; i < 10000; i++){
			for (int j = 0; j < 10000; j++);
		}
	} while (++n<N);

	namedWindow("MainWindow", 1);
	imshow("MainWindow",Background);
#endif
	waitKey();
}