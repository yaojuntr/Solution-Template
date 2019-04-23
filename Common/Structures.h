/***************************************************
 * @file		Structures.h
 * @section		Common
 * @class		N/A
 * @brief		define some structs
 * @author		bqrmtao@gmail.com
 * @date		2017/10/14
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/
 
#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

/** enum */
//enum TransformType
//{
//	Sigmoid,   ///< 
//};

struct ImageHeader
{
	bool			isSigned;
	unsigned char	ucPixelDepth;
	unsigned short	usImgHeight;
	unsigned short	usImgWidth;
};
 
#endif	// __STRUCTURES_H__