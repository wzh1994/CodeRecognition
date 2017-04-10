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
#include <ml/ml.hpp>
#include "constant.h"
using namespace std;
using namespace cv;

extern char characters[LetterNum];
double getSecondNorm(double *pt, double*tr);
typedef struct node{
	double secondNorm;
	int lable;
}knnTrainNode,*knnTrain;
bool SortBySecondNorm(const knnTrainNode &v1, const knnTrainNode &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1.secondNorm < v2.secondNorm;//升序排列  
}
char knn(double* pt, double trainSet[][Patterns], int* lables){
	vector<knnTrainNode> trains;
	for (int i = 0; i < TrainSize; i++){
		knnTrainNode temp;
		temp.secondNorm = getSecondNorm(pt, trainSet[i]);
		temp.lable = lables[i];
		trains.push_back(temp);
	}
#if ShowSecondNormal
	for (int i = 0; i < TrainSize; i++){
		cout << i << ":" << trains[i].secondNorm << " ";
		if (i % 10 == 9) cout << endl;
	}
	cout << endl << endl;
#endif
	//对训练集按照第二范数排序
	sort(trains.begin(), trains.end(), SortBySecondNorm);
#if ShowSecondNormal
	for (int i = 0; i < TrainSize; i++){
		cout <<i<<":"<< trains[i].secondNorm<<" ";
		if (i % 10 == 9) cout << endl;
	}
	cout << endl << endl;
#endif
	int lableNum[LetterNum];
	memset(lableNum, 0, sizeof(lableNum));
	//统计前K个第二范数的标签
	for (int i = 0; i < K; i++){
		lableNum[trains[i].lable]++;
	}
	//找到最高频率的标签
	int maxnum = 0;
	for (int i = 1; i < LetterNum; i++){
#if ShowTopK
		cout << i << ":" << lableNum[i]<<" ";
		if (i % 10 == 9) cout << endl;
#endif
		if (lableNum[i]>lableNum[maxnum]) maxnum = i;
	}
#if ShowTopK
	//计算字符
	cout <<endl<< maxnum << ":" << characters[maxnum] << " "<<endl<<endl;
#endif
	return characters[maxnum];
}

double getSecondNorm(double *pt, double*tr){
	double sn=0;
	for (int i = 0; i < Patterns; i++){
		sn += (pt[i] - tr[i])*(pt[i] - tr[i]);
	}
	return sqrt(sn);
}