/***************************************************
 * @file		ReadImage
 * @section		Common
 * @class		N/A
 * @brief		read image from files
 * @author		bqrmtao@gmail.com
 * @date		2017/10/13
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef _READ_IMAGE_H_
#define _READ_IMAGE_H_

#include <string>

#include "MacroDefination.h"
#include "Structures.h"

/*
 * @brief	
 * @param	
 * @return	
*/
#ifdef _cplusplus
extern "C"
{
#endif // _cplusplus

	int EXPORT_DLL ReadImage(std::string strFileName, void* pDataBuf, ImageHeader *pImgHeader);

#ifdef _cplusplus
};
#endif // _cplusplus

#endif
