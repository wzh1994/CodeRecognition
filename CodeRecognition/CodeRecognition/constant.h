//ѵ����������
#define K 300            //KNN��K������С��KNN_N
#define KNN_N     20000  //KNNʹ�õ�ѵ����,����С��TrainSize
#define TrainSize 100000 //ѵ�������ĸ���������Ϊ4�ı���
#define TestSize 10     //���������ĸ���
#define Patterns 8	   //��ȡ����������
#define Standardize 1   //��׼��
#define ZScore 0	    //������=��ԭ����-��ֵ��/��׼��
#define MinMax 1       //������=��ԭ����-��Сֵ��/������ֵ-��Сֵ��
#define Decimal_Scaling // x'=x/(10*j)
#define WindowSize 6    //parzen����С

#if WindowSize==3
#define ParzenSize 6561
#elif WindowSize==4
#define ParzenSize 65536
#elif WindowSize==6
#define ParzenSize 1679616
#elif WindowSize==7
#define ParzenSize 5764801
#elif WindowSize==8
#define ParzenSize 16777216
#endif


//��֤�����
#define Lines 0		  //�Ƿ���������
#define Complax 2	  //��֤���ַ����Ӷ�
#if Complax==1
#define LetterNum 2   //��֤����ַ�����
#elif Complax==2
#define LetterNum 10
#elif Complax==3
#define LetterNum 58 
#endif
#define FontComplax 1	  //��֤�����帴�Ӷ�
#if FontComplax==1
#define FontNum 2   //��֤����ַ�����
#elif FontComplax==2
#define FontNum 3
#elif FontComplax==3
#define FontNum 5
#endif


//ѵ��&����
#define NOFRESH 1	//ʹ���Ѿ����ɺõ�ѵ����
#define DOTEST 1	//���ɲ��Լ�������

/*�߿��С����*/
#define code_width  150 //��֤����
#define code_height  50 //��֤��߶�
#define letter_width 40 //������ĵ����ַ����
#define window_width  900  //��ʾ���ڿ��
#define window_height  700 //��ʾ���ڸ߶�
#define gap 3			   //ͼƬ���

//�������
#define DotSizeShow 0	   //�Ƿ����ÿ���ַ������
#define MaxFourColor 0	   //�Ƿ��������������ɫ��Ƶ��
#define PatternShow	0	   //�Ƿ������ȡ��������
#define ShowTestPattern 0  //������Լ�������
#define ShowSecondNormal 0 //����ڶ�����
#define ShowTopK 0		   //��ʾǰK����ַ�
#define showParzen 0       //��ʾparzen���ĺ������
#define showResult 0       //��ʾ���

//����һ����Ȥ���Ա����Ҫ��ʾ���ַ�
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * ((_n)+2) + ((_n)+1)*code_height, ((_n) + 2)*(code_height + gap)), Range((_start), (_start) + (_width))); \
	_start += (_width);\
}while (0)

//����������׼���Ĳ���
typedef struct ptNode{
	double means[Patterns];				  //������ֵ
	double sDeviation[Patterns];		  //������׼��
	double maxPattern[Patterns];          //���������ֵ
	double minPattern[Patterns];          //��������Сֵ
	double scalePattern[Patterns];        //����������
}PTArgsNode,*PTArgs;
