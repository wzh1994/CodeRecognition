//ѵ����������
#define K 100  //KNN��K
#define N 5   //���������ĸ���
#define TrainSize 100 //ѵ�������ĸ���������Ϊ4�ı���
#define Patterns 7	   //��ȡ����������

//��֤�����
#define Lines 1		//�Ƿ���������

//ѵ��&����
#define NOFRESH 0	//������ѵ����
#define DOTEST 1	//���в��Լ��Ĳ���

/*�߿��С����*/
#define code_width  140 //��֤����
#define code_height  50 //��֤��߶�
#define letter_width 40 //������ĵ����ַ����
#define window_width  900  //��ʾ���ڿ��
#define window_height  700 //��ʾ���ڸ߶�
#define gap 3			   //ͼƬ���

//�������
#define DotSizeShow 0	  //�Ƿ����ÿ���ַ������
#define MaxFourColor 0	  //�Ƿ��������������ɫ��Ƶ��
#define PatternShow	0	  //�Ƿ������ȡ��������

//����һ����Ȥ���Ա����Ҫ��ʾ���ַ�
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * ((_n)+2) + ((_n)+1)*code_height, ((_n) + 2)*(code_height + gap)), Range((_start), (_start) + (_width))); \
	_start += (_width);\
}while (0)

