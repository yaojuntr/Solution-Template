/***************************************************
 * @file		MacroDeclSpec.h
 * @section		Common
 * @class		N/A
 * @brief		define macro of _declspec
 * @author		bqrmtao@gmail.com
 * @date		2018/01/24
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef __DECL_SPEC_H__
#define __DECL_SPEC_H__

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)

#	define _DLL_EXPORT_	_declspec(dllexport)	// export
#	define _DLL_IMPORT_	_declspec(dllimport)	// import
//#	define _DLL_CALLBACK_	__stdcall
//#	define _DLL_CDECL_	__cdecl
//#	define _DLL_STDCALL_	__stdcall

#else

#	define _DLL_EXPORT_
#	define _DLL_IMPORT_
//#	define _DLL_CALLBACK_
//#	define _DLL_CDECL_
//#	define _DLL_STDCALL_

typedef long long int __int64;

#endif	// WIN32, W64, WINCE

#endif	// __DECL_SPEC_H__
