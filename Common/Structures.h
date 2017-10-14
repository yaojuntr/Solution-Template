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
 
#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

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

struct DicomInfo
{

};
 
#endif