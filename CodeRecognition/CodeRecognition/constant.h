//ѵ����������
#define K 100  //KNN��K
#define N 10   //���������ĸ���
#define TrainSize 6000 //ѵ�������ĸ���������Ϊ4�ı���

//
#define NOFRESH 0
#define DOTEST 0

/*�߿��С����*/
#define code_width  120 //��֤����
#define code_height  50 //��֤��߶�
#define letter_width 40 //������ĵ����ַ����
#define window_width  900  //��ʾ���ڿ��
#define window_height  700 //��ʾ���ڸ߶�
#define gap 3			   //ͼƬ���

//�������
#define DotSizeShow 0	  //�Ƿ����ÿ���ַ������

//����һ����Ȥ���Ա����Ҫ��ʾ���ַ�
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * _n + gap + _n*code_height, (_n + 1)*(code_height + gap)), Range(_start, _start + _width)); \
	_start += _width;\
}while (0)

