#pragma once

#define SI_UNUSED(a) ((void)(a))

#define SI_GLOBAL_CONST extern const __declspec(selectany)

#define SI_STR(a) #a

#define SI__JOIN(a, b) a ## b
#define SI_JOIN(a, b) SI__JOIN(a, b)
	
#define SI_ARGS_COUNT_EXPAND(x) x
#define SI_ARGS_COUNT_INTERNAL(\
	 p0 , p1,  p2,  p3,  p4,  p5,  p6,  p7,  p8,  p9,\
	p10, p11, p12, p13, p14, p15, p16, p17, p18, p19,\
	p20, p21, p22, p23, p24, p25, p26, p27, p28, p29,\
	p30, p31, p32, p33, p34, p35, p36, p37, p38, p39,\
	p40, p41, p42, p43, p44, p45, p46, p47, p48, p49,\
	p50, p51, p52, p53, p54, p55, p56, p57, p58, p59,\
	p60, p61, p62, p63, p64, p65, p66, p67, p68, p69,\
	ARGS_COUNT, ...) ARGS_COUNT

// __VA_ARGS__の数を数えるためのマクロ.
#define SI_ARGS_COUNT(...) SI_ARGS_COUNT_EXPAND(SI_ARGS_COUNT_INTERNAL(\
	-1, ##__VA_ARGS__,\
	69, 68, 67, 66, 65, 64, 63, 62, 61, 60,\
	59, 58, 57, 56, 55, 54, 53, 52, 51, 50,\
	49, 48, 47, 46, 45, 44, 43, 42, 41, 40,\
	39, 38, 37, 36, 35, 34, 33, 32, 31, 30,\
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20,\
	19, 18, 17, 16, 15, 14, 13, 12, 11, 10,\
	 9,  8,  7,  6,  5,  4,  3,  2,  1,  0))
