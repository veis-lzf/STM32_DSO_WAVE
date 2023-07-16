#ifndef __SYSTYPE_H__
#define __SYSTYPE_H__

#include <stdint.h>

/* 定义可移植数据类型 */
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef int8_t
typedef signed char int8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef int16_t
typedef signed short int16_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef int32_t
typedef signed int int32_t;
#endif


#endif /* __SYSTYPE_H__ */
