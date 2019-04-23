/***************************************************
 * @file		CommonMethod.h
 * @section		Common
 * @class		N/A
 * @brief		N/A
 * @author		bqrmtao@gmail.com
 * @date		2017/11/13
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef __COMMON_METHOD_H__
#define __COMMON_METHOD_H__

#include <functional>
#include <string.h>
#include <vector>

#include "MacroDeclSpec.h"

/* enum */
//enum 
//{
//		,   ///< 
//};


/*
 * @brief	convert a value to string
 * @param	tInVal, input value
 * @param	ullFloatPrecision, precision if double or float
 * @return	a string
*/
template<typename T>
_DLL_EXPORT_ std::string ConvertValToString(T tInVal, size_t ullFloatPrecision = 0)
{
	char czFloatToStr[64] = { 0 };

	if (typeid(tInVal) == typeid(char))
	{
		sprintf_s(czFloatToStr, "%c", tInVal);
	}
	else if (typeid(tInVal) == typeid(unsigned char))
	{
		sprintf_s(czFloatToStr, "%uc", tInVal);
	}
	else if (typeid(tInVal) == typeid(short))
	{
		sprintf_s(czFloatToStr, "%hd", tInVal);
	}
	else if (typeid(tInVal) == typeid(unsigned short))
	{
		sprintf_s(czFloatToStr, "%uhd", tInVal);
	}
	else if (typeid(tInVal) == typeid(int))
	{
		sprintf_s(czFloatToStr, "%d", tInVal);
	}
	else if (typeid(tInVal) == typeid(unsigned int))
	{
		sprintf_s(czFloatToStr, "%u", tInVal);
	}
	else if (typeid(tInVal) == typeid(long))
	{
		sprintf_s(czFloatToStr, "%ld", tInVal);
	}
	else if (typeid(tInVal) == typeid(unsigned long))
	{
		sprintf_s(czFloatToStr, "%lu", tInVal);
	}
	else if (typeid(tInVal) == typeid(long long))
	{
		sprintf_s(czFloatToStr, "%lld", tInVal);
	}
	else if (typeid(tInVal) == typeid(unsigned long long))
	{
		sprintf_s(czFloatToStr, "%llu", tInVal);
	}
	else
	{
		// double can ensure 15 valid bits after dot
		if (ullFloatPrecision > 10)
		{
			ullFloatPrecision = 10;
		}

		char czFloatFormat[10] = { 0 };
		sprintf_s(czFloatFormat, "%%1.%uf", ullFloatPrecision);
		sprintf_s(czFloatToStr, czFloatFormat, tInVal);
	}

	return std::string(czFloatToStr);
}

/*
 * @brief	convert a hexadecimal string into unsigned int
 * @param	strHexVal
 * @return	unsigned int
*/
_DLL_EXPORT_ unsigned int ConvertHexStrToUint(std::string strHexVal);

/*
 * @brief	get the full path of the running program
 * @return	full path of the running program
*/
_DLL_EXPORT_ std::string GetInitialPath();

/*
 * @brief	filter image using C
 * @param	pImgPtr, pointer of input image
 * @param	usImgWidth, width of input image
 * @param	usImgHeight, height of input image
 * @param	pFilterKernel, pointer of input kernel, temporarily 3 * 3
 * @bug		only unsigned short supported
 * @return	
 */
template<typename T>
_DLL_EXPORT_ void FilterImage(const T* pImgPtr, unsigned short usImgHeight, unsigned short usImgWidth, const float* pFilterKernel)
{
	// kernel
	// k1, k2, k3
	// k4, k5, k6
	// k7, k8, k9
	float fKernelVal1 = *(pFilterKernel);
	float fKernelVal2 = *(pFilterKernel + 1);
	float fKernelVal3 = *(pFilterKernel + 2);
	float fKernelVal4 = *(pFilterKernel + 3);
	float fKernelVal5 = *(pFilterKernel + 4);
	float fKernelVal6 = *(pFilterKernel + 5);
	float fKernelVal7 = *(pFilterKernel + 6);
	float fKernelVal8 = *(pFilterKernel + 7);
	float fKernelVal9 = *(pFilterKernel + 8);

	T* pDstImg = new T[usImgWidth * usImgHeight * sizeof(T)];

	T* pTempPtr = pDstImg;
	
	T* pImgPtr1 = 0;
	T* pImgPtr2 = 0;
	T* pImgPtr3 = 0;
	T* pImgPtr4 = 0;
	T* pImgPtr5 = 0;
	T* pImgPtr6 = 0;
	T* pImgPtr7 = 0;
	T* pImgPtr8 = 0;
	T* pImgPtr9 = 0;

	uint32_t unPixelOffset = 0;

	float dTempSum = 0;

	for (uint16_t usRowIdx = 0; usRowIdx < usImgHeight; usRowIdx++)
	{
		// offset of pixel multiplied with k6
		unPixelOffset = usRowIdx * usImgWidth;
		pImgPtr6 = pImgPtr + unPixelOffset;

		// offset of pixel multiplied with k3 and k9
		pImgPtr3 = pImgPtr6 - (usRowIdx > 0 ? usImgWidth : 0);
		pImgPtr9 = pImgPtr6 + (usRowIdx < usImgHeight - 1 ? usImgWidth : 0);

		// k2, k5, k8
		pImgPtr2 = pImgPtr3;
		pImgPtr5 = pImgPtr6;
		pImgPtr8 = pImgPtr9;

		for (uint16_t usColIdx = 0; usColIdx < usImgWidth; usColIdx++)
		{
			pImgPtr1 = pImgPtr2;
			pImgPtr4 = pImgPtr5;
			pImgPtr7 = pImgPtr8;

			pImgPtr2 = pImgPtr3;
			pImgPtr5 = pImgPtr6;
			pImgPtr8 = pImgPtr9;

			if (usColIdx < usImgWidth - 1)
			{
				pImgPtr3++;
				pImgPtr6++;
				pImgPtr9++;
			}

			dTempSum = *pImgPtr1 * fKernelVal1 + \
				*pImgPtr2 * fKernelVal2 + \
				*pImgPtr3 * fKernelVal3 + \
				*pImgPtr4 * fKernelVal4 + \
				*pImgPtr5 * fKernelVal5 + \
				*pImgPtr6 * fKernelVal6 + \
				*pImgPtr7 * fKernelVal7 + \
				*pImgPtr8 * fKernelVal8 + \
				*pImgPtr9 * fKernelVal9;

			*(pTempPtr) = dTempSum > 0 ? (dTempSum > 65535 ? 65535 : (T)(dTempSum + 0.5)) : 0;
			pTempPtr++;
		}
	}

	memcpy((uint8_t*)pImgPtr, pDstImg, usImgHeight * usImgWidth * sizeof(T));

	delete[] pDstImg;
	pDstImg = nullptr;
}

/*
 * @brief	read data from disk
 * @param	strFileName
 * @param	pDataBuff: char*&
 * @param	unByteToWrite
 * @param	unByteToSkip
 * @return	result of reading data from disk
*/
_DLL_EXPORT_ bool ReadFromDisk(const std::string strFileName, char*&pDataBuff, size_t unByteToRead, const size_t unByteToSkip = 0);

/*
 * @brief	split string according pointed splitter
 * @param	string to be split
 * @param	splitter
 * @param	sub-string
 * @return	void
*/
_DLL_EXPORT_ void SplitString(const std::string& strSrc, const std::string& strSplitter, std::vector<std::string>& vecSplittedStr);

/*
 * @brief	write data to disk
 * @param	strFolderPath
 * @param	strFileName
 * @param	pDataBuff: char*&
 * @param	unByteToWrite
 * @param	unByteToSkip
 * @return	result of writing data to disk
*/
_DLL_EXPORT_ bool WriteToDisk(const std::string strFolderPath, const std::string strFileName, char*&pDataBuff, const size_t unByteToWrite, const size_t unByteToSkip = 0);

/*
 * @brief	
 * @param	
 * @return	
*/

//#ifdef __cplusplus
//	extern "C"
//	{
//#endif // __cplusplus
//
//
//
//#ifdef __cplusplus
//};
//#endif // __cplusplus

#endif	// __COMMON_METHOD_H__
