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
#include <stdlib.h>
#include <map>
#include "constant.h"
using namespace std;
using namespace cv;

extern char characters[LetterNum];
map<long long, double*> parzenProbability;
double priorProbability[LetterNum];        //�������
long long pows[Patterns];
double getSecondNorm(double *pt, double*tr);
typedef struct node{
	double secondNorm;
	int lable;
}knnTrainNode,*knnTrain;
bool SortBySecondNorm(const knnTrainNode &v1, const knnTrainNode &v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{
	return v1.secondNorm < v2.secondNorm;//��������  
}
bool inInWindow(double *pt, double *tr){
	for (int i = 0; i < Patterns; i++){
		if (abs(pt[i] - tr[i]) >( 1 / (double)WindowSize)) return false;
	}
	return true;
}
int knn_parzen(double* pt, double trainSet[][Patterns], int* lables){
	vector<knnTrainNode> trains;
	int parzen[LetterNum];
	memset(parzen, 0, sizeof(parzen));
	for (int i = 0; i < KNN_N; i++){
		//parzen
		if (inInWindow(pt,trainSet[i])){
			parzen[lables[i]]++;
		}
		//knn
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
	int result = 0;
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
	result = characters[maxnum];
	//parzen
	maxnum = 0;
	for (int i = 1; i < LetterNum; i++){
		if (parzen[i]>parzen[maxnum]) maxnum = i;
	}
	result = result << 8;
	result += (int)characters[maxnum];
	return result;
}

double getSecondNorm(double *pt, double*tr){
	double sn=0;
	for (int i = 0; i < Patterns; i++){
		sn += (pt[i] - tr[i])*(pt[i] - tr[i]);
	}
	return sqrt(sn);
}

long long calParzenPosition(double* pt){
	long long pos = 0;
	for (int i = 0; i < Patterns; i++){
		int level = pt[i] * WindowSize;
		level = level < 10 ? level : 9;
		pos += level*pows[i];
	}
	return pos;
}

void generateParzenArgs(double trainSet[][Patterns],int* lables){
	memset(priorProbability, 0, sizeof(priorProbability));
	for (int i=0;i<Patterns;i++){
		pows[i]=  pow(WindowSize, i);
	}
#if NOFRESH
	if (freopen("parzen", "r", stdin) != NULL){
#if ShowProcess
		printf("Loading Parzen Agrs...\n");
#endif
		for (int i = 0; i<LetterNum; i++){
			cin >> priorProbability[i];
		}
		long long line;
		while (~scanf("%I64d",&line)){
			double *temp = (double*)malloc(LetterNum*sizeof(double));
			for (int j = 0; j<LetterNum; j++){
				cin >> temp[j];
			}
			parzenProbability[line] = temp;
		}
#if ShowProcess
		printf("Load Parzen Agrs Done\n");
#endif
		freopen("CON", "r", stdin);
	}
	else{
#endif
#if ShowProcess
		printf("Train parzen args start:\n");
		int rate = TrainSize / 50;
#endif
		//����ѵ����
		for (int i = 0; i < TrainSize; i++){
#if ShowProcess
			if (i%2000==0)
				cout << "Trainset Parzen "<< i << endl;
#endif
			long long pos = calParzenPosition(trainSet[i]);
			if (parzenProbability[pos]){
				parzenProbability[pos][lables[i]]++;
			}
			else{
				double *temp = (double*)malloc(LetterNum*sizeof(double));
				memset(temp, 0, LetterNum*sizeof(double));
				parzenProbability[pos] = temp;
				parzenProbability[pos][lables[i]]++;
			}
			priorProbability[lables[i]]++;
#if ShowProcess
			int dot = (i + 1) / rate;;
			if ((i + 1) % rate == 0){
				printf("Testing Accuracy");
				for (int k = 0; k < dot; k++){
					printf(".");
				}
				for (int k = dot; k < 50; k++){
					printf(" ");
				}
				printf("%2d%%\r", dot * 2);
			}
#endif
		}
		freopen("parzen","w",stdout);
		for (int i = 0; i < LetterNum; i++){
			priorProbability[i] /= TrainSize;
			cout << priorProbability[i]<<" ";
		}
		cout << endl;
		for (map<long long, double*>::iterator i = parzenProbability.begin(); i != parzenProbability.end(); i++){
			cout << i->first << " ";
			for (int j = 0; j < LetterNum; j++){
				i->second[j] /= TrainSize;
				cout << i->second[j] << " ";
			}
			cout << endl;
		}
		freopen("CON", "w", stdout);
#if ShowProcess
		cout << "Trainset Parzen Done" << endl;
#endif
#if NOFRESH
	}
#endif
}

char parzen(double* pt){
	long long pos = calParzenPosition(pt);
	double maxProbility = 0;
	int maxLable = 0;
	if (parzenProbability[pos]){
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
	}
#if showParzen
	cout << maxLable << " " << characters[maxLable]<<endl;
#endif
	return characters[maxLable];
}

void freeMap(){
	for (map<long long, double*>::iterator i = parzenProbability.begin(); i != parzenProbability.end(); i++){
		if (i->second) free(i->second);
	}
}