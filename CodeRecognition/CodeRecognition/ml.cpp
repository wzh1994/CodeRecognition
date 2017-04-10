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
double parzenProbability[ParzenSize][LetterNum]; //��ɢ�����ܶ�
double priorProbability[LetterNum];        //�������
#if WindowSize==3
int threes[8] = { 1, 3, 9, 27, 81, 243, 729, 2187 };
#elif WindowSize==4
int fours[8] = { 1, 4, 16, 64, 256, 1024, 4096, 16384 };
#endif
double getSecondNorm(double *pt, double*tr);
typedef struct node{
	double secondNorm;
	int lable;
}knnTrainNode,*knnTrain;
bool SortBySecondNorm(const knnTrainNode &v1, const knnTrainNode &v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{
	return v1.secondNorm < v2.secondNorm;//��������  
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
	//��ѵ�������յڶ���������
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
	//ͳ��ǰK���ڶ������ı�ǩ
	for (int i = 0; i < K; i++){
		lableNum[trains[i].lable]++;
	}
	//�ҵ����Ƶ�ʵı�ǩ
	int maxnum = 0;
	for (int i = 1; i < LetterNum; i++){
#if ShowTopK
		cout << i << ":" << lableNum[i]<<" ";
		if (i % 10 == 9) cout << endl;
#endif
		if (lableNum[i]>lableNum[maxnum]) maxnum = i;
	}
#if ShowTopK
	//�����ַ�
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
		for (int i = 0; i<ParzenSize; i++){
			for (int j = 0; j<LetterNum; j++){
				cin >> parzenProbability[i][j];
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
			for (int j = 0; j < LetterNum; j++){
				parzenProbability[i][j] /= TrainSize;
				cout << parzenProbability[i][j] << " ";
			}
			cout << endl;
		}
		freopen("CON", "w", stdout);
#if NOFRESH
	}
#endif
}

char parzen(double* pt){
	int pos = calParzenPosition(pt);
	double maxProbility = 0;
	int maxLable = -1;
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