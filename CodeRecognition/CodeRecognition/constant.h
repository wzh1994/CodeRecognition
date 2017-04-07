//训练参数控制
#define K 100  //KNN的K
#define N 10   //测试样本的个数
#define TrainSize 6000 //训练样本的个数，必须为4的倍数

//
#define NOFRESH 0
#define DOTEST 0

/*边框大小参数*/
#define code_width  120 //验证码宽度
#define code_height  50 //验证码高度
#define letter_width 40 //分离出的单个字符宽度
#define window_width  900  //显示窗口宽度
#define window_height  700 //显示窗口高度
#define gap 3			   //图片间隔

//输出控制
#define DotSizeShow 0	  //是否输出每个字符的面积

//创建一个兴趣区以便填充要显示的字符
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * _n + gap + _n*code_height, (_n + 1)*(code_height + gap)), Range(_start, _start + _width)); \
	_start += _width;\
}while (0)

