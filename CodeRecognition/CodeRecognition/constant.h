//训练参数控制
#define K 300            //KNN的K，必须小于KNN_N
#define KNN_N     20000  //KNN使用的训练集,必须小于TrainSize
#define TrainSize 300000 //训练样本的个数，必须为4的倍数
#define TestSize 10     //测试样本的个数
#define Patterns 8	   //提取的特征个数
#define Standardize 1   //标准化
#define ZScore 0	    //新数据=（原数据-均值）/标准差
#define MinMax 1       //新数据=（原数据-极小值）/（极大值-极小值）
#define Decimal_Scaling // x'=x/(10*j)
#define WindowSize 4    //3或者4

#if WindowSize==3
#define ParzenSize 6561
#elif WindowSize==4
#define ParzenSize 65536
#endif


//验证码相关
#define Lines 0		  //是否加入干扰线
#define Complax 2	  //验证码字符复杂度
#if Complax==1
#define LetterNum 2   //验证码的字符数量
#elif Complax==2
#define LetterNum 10
#elif Complax==3
#define LetterNum 58 
#endif
#define FontComplax 1	  //验证码字体复杂度
#if FontComplax==1
#define FontNum 2   //验证码的字符数量
#elif FontComplax==2
#define FontNum 3
#elif FontComplax==3
#define FontNum 5
#endif


//训练&测试
#define NOFRESH 1	//使用已经生成好的训练集
#define DOTEST 1	//生成测试集并测试

/*边框大小参数*/
#define code_width  150 //验证码宽度
#define code_height  50 //验证码高度
#define letter_width 40 //分离出的单个字符宽度
#define window_width  900  //显示窗口宽度
#define window_height  700 //显示窗口高度
#define gap 3			   //图片间隔

//输出控制
#define DotSizeShow 0	   //是否输出每个字符的面积
#define MaxFourColor 0	   //是否输出最多的四种颜色的频率
#define PatternShow	0	   //是否输出提取到的特征
#define ShowTestPattern 0  //输出测试集的特征
#define ShowSecondNormal 0 //输出第二范数
#define ShowTopK 0		   //显示前K多个字符
#define showParzen 0       //显示parzen窗的后验概率
#define showResult 0       //显示结果

//创建一个兴趣区以便填充要显示的字符
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * ((_n)+2) + ((_n)+1)*code_height, ((_n) + 2)*(code_height + gap)), Range((_start), (_start) + (_width))); \
	_start += (_width);\
}while (0)

//描述特征标准化的参数
typedef struct ptNode{
	double means[Patterns];				  //特征均值
	double sDeviation[Patterns];		  //特征标准差
	double maxPattern[Patterns];          //特征的最大值
	double minPattern[Patterns];          //特征的最小值
	double scalePattern[Patterns];        //特征的量级
}PTArgsNode,*PTArgs;
