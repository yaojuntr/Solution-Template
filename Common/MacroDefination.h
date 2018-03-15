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

#ifndef _MACRO_DEFINATION_H_
#define _MACRO_DEFINATION_H_

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)

#	define EXPORT_DLL	_declspec(dllexport)	// export
#	define IMPORT_DLL	_declspec(dllimport)	// import
//#	define CALLBACK_DLL	__stdcall
//#	define CDECL_DLL	__cdecl
//#	define STDCALL_DLL	__stdcall

#else
#	define LINUX_VERSION
#	define EXPORT_DLL
#	define IMPORT_DLL
//#	define CALLBACK_DLL
//#	define CDECL_DLL
//#	define STDCALL_DLL

typedef long long int __int64;

#endif

#endif
