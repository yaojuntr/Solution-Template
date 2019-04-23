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

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
#include <filesystem>
namespace sf = std::tr2::sys;
#else
#include <experimental/filesystem>
namespace sf = std::experimental::filesystem;
#endif

#include <fstream>

#include "ErrorMsg.h"
#include "Logger.h"
#include "ReadImage.h"

using namespace std;

/*
 * @brief	read normal image
 * @param	strFileName
 * @param	pDataBuf
 * @param	usImgHeight
 * @param	usImgWidth
 * @param	ucPixelDepth
 * @return	process result
*/
int ReadImage(std::string strFileName, void* pDataBuf, ImageHeader *pImgHeader)
{
	return OK;
}
