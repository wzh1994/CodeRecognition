//ѵ����������
#define K 100  //KNN��K
#define N 5   //���������ĸ���
#define TrainSize 10000 //ѵ�������ĸ���������Ϊ4�ı���
#define TestSize 5     //���������ĸ���
#define Patterns 7	   //��ȡ����������
#define Standardize 0  //��׼��
#define Averaging 1	   //ƽ����	

//��֤�����
#define Lines 0		//�Ƿ���������

//ѵ��&����
#define NOFRESH 0	//ʹ���Ѿ����ɺõ�ѵ����
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
#define ShowTopK 1		   //��ʾǰK����ַ�

//����һ����Ȥ���Ա����Ҫ��ʾ���ַ�
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * ((_n)+2) + ((_n)+1)*code_height, ((_n) + 2)*(code_height + gap)), Range((_start), (_start) + (_width))); \
	_start += (_width);\
}while (0)

