//训练参数控制
#define K 100  //KNN的K
#define N 5   //测试样本的个数
#define TrainSize 100 //训练样本的个数，必须为4的倍数
#define Patterns 7	   //提取的特征个数

//验证码相关
#define Lines 1		//是否加入干扰线

//训练&测试
#define NOFRESH 0	//不更新训练集
#define DOTEST 1	//进行测试集的测试

/*边框大小参数*/
#define code_width  140 //验证码宽度
#define code_height  50 //验证码高度
#define letter_width 40 //分离出的单个字符宽度
#define window_width  900  //显示窗口宽度
#define window_height  700 //显示窗口高度
#define gap 3			   //图片间隔

//输出控制
#define DotSizeShow 0	  //是否输出每个字符的面积
#define MaxFourColor 0	  //是否输出最多的四种颜色的频率
#define PatternShow	0	  //是否输出提取到的特征

//创建一个兴趣区以便填充要显示的字符
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * ((_n)+2) + ((_n)+1)*code_height, ((_n) + 2)*(code_height + gap)), Range((_start), (_start) + (_width))); \
	_start += (_width);\
}while (0)

