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
double priorProbability[LetterNum];        //先验概率
long long pows[Patterns];
double getSecondNorm(double *pt, double*tr);
typedef struct node{
	double secondNorm;
	int label;
}knnTrainNode,*knnTrain;
bool SortBySecondNorm(const knnTrainNode &v1, const knnTrainNode &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1.secondNorm < v2.secondNorm;//升序排列  
}
bool inInWindow(double *pt, double *tr){
	for (int i = 0; i < Patterns; i++){
		if (abs(pt[i] - tr[i]) >( 0.5 / (double)WindowSize)) return false;
	}
	return true;
}
int knn_parzen(double* pt, double trainSet[][Patterns], int* labels){
	vector<knnTrainNode> trains;
	int parzen[LetterNum];
	memset(parzen, 0, sizeof(parzen));
	for (int i = 0; i < KNN_N; i++){
		//parzen
		if (inInWindow(pt,trainSet[i])){
			parzen[labels[i]]++;
		}
		//knn
		knnTrainNode temp;
		temp.secondNorm = getSecondNorm(pt, trainSet[i]);
		temp.label = labels[i];
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
	int labelNum[LetterNum];
	memset(labelNum, 0, sizeof(labelNum));
	//统计前K个第二范数的标签
	for (int i = 0; i < K; i++){
		labelNum[trains[i].label]++;
	}
	//找到最高频率的标签
	int maxnum = 0;
	int result = 0;
	for (int i = 1; i < LetterNum; i++){
#if ShowTopK
		cout << i << ":" << labelNum[i]<<" ";
		if (i % 10 == 9) cout << endl;
#endif
		if (labelNum[i]>labelNum[maxnum]) maxnum = i;
	}
#if ShowTopK
	//计算字符
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

void generateParzenArgs(double trainSet[][Patterns],int* labels){
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
		//读入训练集
		for (int i = 0; i < TrainSize; i++){
			long long pos = calParzenPosition(trainSet[i]);
			if (parzenProbability[pos]){
				parzenProbability[pos][labels[i]]++;
			}
			else{
				double *temp = (double*)malloc(LetterNum*sizeof(double));
				memset(temp, 0, LetterNum*sizeof(double));
				parzenProbability[pos] = temp;
				parzenProbability[pos][labels[i]]++;
			}
			priorProbability[labels[i]]++;
#if ShowProcess
			int dot = (i + 1) / rate;;
			if ((i + 1) % rate == 0){
				printf("Train parzen Args");
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
	int maxLabel = 0;
	if (parzenProbability[pos]){
		for (int i = 0; i < LetterNum; i++){
			double probility = parzenProbability[pos][i] * priorProbability[i];
#if showParzen
			cout << probility << " ";
#endif
			if (probility>maxProbility){
				maxProbility = probility;
				maxLabel = i;
			}
		}
	}
#if showParzen
	cout << maxLabel << " " << characters[maxLabel]<<endl;
#endif
	return characters[maxLabel];
}

void freeMap(){
	for (map<long long, double*>::iterator i = parzenProbability.begin(); i != parzenProbability.end(); i++){
		if (i->second) free(i->second);
	}
}
