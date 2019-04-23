/***************************************************
 * @file		MacroDefination.h
 * @section		Common
 * @class		N/A
 * @brief		define global macros
 * @author		bqrmtao@gmail.com
 * @date		2018/01/24
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef __MACRO_DEFINATION_H__
#define __MACRO_DEFINATION_H__

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)

#include <windows.h>

#else

#include <unistd.h>

#define Sleep(x)	usleep((x) * 1000)
#define sprintf_s	sprintf

#endif	// WIN32, W64, WINCE

#endif	// __MACRO_DEFINATION_H__
