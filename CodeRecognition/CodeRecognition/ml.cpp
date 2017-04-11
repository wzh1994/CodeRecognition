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
double parzenProbability[ParzenSize][LetterNum]; //离散概率密度
double priorProbability[LetterNum];        //先验概率
#if WindowSize==3
int threes[8] = { 1, 3, 9, 27, 81, 243, 729, 2187 };
#elif WindowSize==4
int fours[8] = { 1, 4, 16, 64, 256, 1024, 4096, 16384 };
#elif WindowSize==6
int six[8] = { 1, 6, 36, 216, 1296, 7776, 46656, 279936 };
#elif WindowSize==7
int seven[8] = { 1, 7, 49, 343, 2401, 16807, 117649, 823543 };
#elif WindowSize==8
int eight[8] = { 1, 8, 64, 512, 4096, 32768, 262144, 2097152 };
#endif
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
	for (int i = 0; i < KNN_N; i++){
		knnTrainNode temp;
		temp.secondNorm = getSecondNorm(pt, trainSet[i]);
		temp.lable = lables[i];
		trains.push_back(temp);
	}
#if ShowSecondNormal
	for (int i = 0; i < KNN_N; i++){
		cout << i << ":" << trains[i].secondNorm << " ";
		if (i % 10 == 9) cout << endl;
	}
	cout << endl << endl;
#endif
	//对训练集按照第二范数排序
	sort(trains.begin(), trains.end(), SortBySecondNorm);
#if ShowSecondNormal
	for (int i = 0; i < KNN_N; i++){
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

int calParzenPosition(double* pt){
	int pos = 0;
	for (int i = 0; i < Patterns; i++){
#if WindowSize==3
		if (3 * pt[i] < 1) pos += 0;
		else if (3 * pt[i] < 2) pos += threes[i];
		else pos += 2 * threes[i];
#elif WindowSize==4
		if (4 * pt[i] < 1) pos += 0;
		else if (4 * pt[i] < 2) pos += fours[i];
		else if (4 * pt[i] < 3) pos +=2* fours[i];
		else pos += 3 * fours[i];
#elif WindowSize==6
		if (6 * pt[i] < 1) pos += 0;
		else if (6 * pt[i] < 2) pos += six[i];
		else if (6 * pt[i] < 3) pos += 2 * six[i];
		else if (6 * pt[i] < 4) pos += 3 * six[i];
		else if (6 * pt[i] < 5) pos += 4 * six[i];
		else pos += 6 * six[i];
#elif WindowSize==7
		if (7 * pt[i] < 1) pos += 0;
		else if (7 * pt[i] < 2) pos += seven[i];
		else if (7 * pt[i] < 3) pos += 2 * seven[i];
		else if (7 * pt[i] < 4) pos += 3 * seven[i];
		else if (7 * pt[i] < 5) pos += 4 * seven[i];
		else if (7 * pt[i] < 6) pos += 5 * seven[i];
		else pos += 6 * seven[i];
#elif WindowSize==8
		if (8 * pt[i] < 1) pos += 0;
		else if (8 * pt[i] < 2) pos += eight[i];
		else if (8 * pt[i] < 3) pos += 2 * eight[i];
		else if (8 * pt[i] < 4) pos += 3 * eight[i];
		else if (8 * pt[i] < 5) pos += 4 * eight[i];
		else if (8 * pt[i] < 6) pos += 5 * eight[i];
		else if (8 * pt[i] < 7) pos += 6 * eight[i];
		else pos += 7 * eight[i];
#endif
	}
	return pos;
}

void generateParzenArgs(double trainSet[][Patterns],int* lables){
	memset(parzenProbability, 0, sizeof(parzenProbability));
	memset(priorProbability, 0, sizeof(priorProbability));
#if NOFRESH
	if (freopen("parzen", "r", stdin) != NULL){
		for (int i = 0; i<LetterNum; i++){
			cin >> priorProbability[i];
		}
		int line;
		while (~scanf("%d",&line)){
			for (int j = 0; j<LetterNum; j++){
				cin >> parzenProbability[line][j];
			}
		}
		freopen("CON", "r", stdin);
	}
	else{
#endif
		for (int i = 0; i < TrainSize; i++){
			int pos=calParzenPosition(trainSet[i]);
			parzenProbability[pos][lables[i]]++;
			priorProbability[lables[i]]++;
		}
		freopen("parzen","w",stdout);
		for (int i = 0; i < LetterNum; i++){
			priorProbability[i] /= TrainSize;
			cout << priorProbability[i]<<" ";
		}
		cout << endl;
		for (int i = 0; i < ParzenSize; i++){
			int flag = 0;
			for (int j = 0; j < LetterNum; j++){
				if (parzenProbability[i][j] != 0) flag = 1;
				parzenProbability[i][j] /= TrainSize;
			}
			if (flag){
				cout << i << " ";
				for (int j = 0; j < LetterNum; j++){
					cout << parzenProbability[i][j] << " ";
				}
				cout << endl;
			}
		}
		freopen("CON", "w", stdout);
#if NOFRESH
	}
#endif
}

char parzen(double* pt){
	int pos = calParzenPosition(pt);
	double maxProbility = 0;
	int maxLable = 0;
	for (int i = 0; i < LetterNum; i++){
		double probility = parzenProbability[pos][i] * priorProbability[i]; 
#if showParzen
		cout << probility << " ";
#endif
		if (probility>maxProbility){
			maxProbility = probility;
			maxLable = i;
		}
	}
#if showParzen
	cout << maxLable << " " << characters[maxLable]<<endl;
#endif
	return characters[maxLable];
}