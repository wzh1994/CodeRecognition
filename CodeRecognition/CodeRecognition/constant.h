#define K 100
#define N 10
#define window_width  900
#define window_height  700
#define code_width  120
#define code_height  50
#define letter_width 40
#define gap 3
#define ADDRIO(_dst,_image,_width,_n,_start) do{\
	_start += gap;\
	_dst=_image(Range(gap * _n + gap + _n*code_height, (_n + 1)*(code_height + gap)), Range(_start, _start + _width)); \
	_start += _width;\
}while (0)

//控制一些中间过程的输出
#define DotSizeShow 0