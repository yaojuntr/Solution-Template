/***************************************************
 * @file		MacroFunction.h
 * @section		Common
 * @class		N/A
 * @brief		macro of common functions
 * @author		bqrmtao@gmail.com
 * @date		2018/03/16
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef __MACRO_FUNCTION_H__
#define __MACRO_FUNCTION_H__

#include <math.h>

#define ABS(x)		((x) >= 0 ? (x) : -(x))
#define MIX(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define ROUND(x)	(((x) > 0) ? std::floor((x) + 0.5) : std::ceil((x) - 0.5))
#define SIGN(x)		((x) == 0 ? 0 : ((x) > 0 ? 1 : -1))

//#ifdef __cplusplus
//extern "C"
//{
//#endif // __cplusplus
//
//
//#ifdef __cplusplus
//};
//#endif // __cplusplus

#endif	// __MACRO_FUNCTION_H__