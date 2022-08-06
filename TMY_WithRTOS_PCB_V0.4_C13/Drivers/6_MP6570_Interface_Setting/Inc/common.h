#ifndef _COMMON_H_
#define _COMMON_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#define mul_32(x,y) ((unsigned int)(x&0x0000FFFF)*y+(((long long int)((x>>16)*y))<<16))


#endif
