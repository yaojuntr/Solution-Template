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

#ifndef _COMMON_METHOD_H_
#define _COMMON_METHOD_H_

#include <functional>
#include <math.h>
#include <string.h>
#include <vector>

#include "MacroDefination.h"

#define DIGITAL_LEN	64

#if !(defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
#define sprintf_s	sprintf
#endif

#define ABS(x)	((x) >= 0 ? (x) : -(x))
#define MIX(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#define ROUND(x) (((x) > 0) ? std::floor((x) + 0.5) : std::ceil((x) - 0.5))
#define SIGN(x) ((x) == 0 ? 0 : ((x) > 0 ? 1 : -1))

/* enum */
//enum 
//{
//		,   ///< 
//};

/*
 * @brief	
 * @param	
 * @return	
*/

#ifdef _cplusplus
extern "C"
{
#endif // _cplusplus

	/*
	 * @brief	split string according pointed splitter
	 * @param	string to be split
	 * @param	splitter
	 * @param	sub-string
	 * @return	void
	*/
	EXPORT_DLL void SplitString(const std::string& strSrc, const std::string& strSplitter, std::vector<std::string>& vecSplittedStr);
	
	/*
	 * @brief	convert a value to string
	 * @param	tInVal, input value
	 * @param	ullFloatPrecision, precision if double or float
	 * @return	a string
	*/
	template<typename T>
	EXPORT_DLL std::string inline ConvertValToString(T tInVal, size_t ullFloatPrecision = 0)
	{
		char czFloatToStr[DIGITAL_LEN] = { 0 };

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
	EXPORT_DLL unsigned int inline ConvertHexStrToUint(std::string strHexVal)
	{
		unsigned int unRetVal = 0;

		// a hexadecimal string should start with "0x" or "0X"
		if ('0' == strHexVal[0] && ('x' == strHexVal[1] || 'X' == strHexVal[1]))
		{
			unsigned char ucCharVal = 0;
			for (size_t lluStrIdx = 2; lluStrIdx < strHexVal.size(); lluStrIdx++)
			{
				if (strHexVal[lluStrIdx] >= '0' && strHexVal[lluStrIdx] <= '9')
				{
					ucCharVal = strHexVal[lluStrIdx] - '0';
				}
				else if (strHexVal[lluStrIdx] >= 'a' && strHexVal[lluStrIdx] <= 'f')
				{
					ucCharVal = strHexVal[lluStrIdx] - 'a' + 10;
				}
				else if (strHexVal[lluStrIdx] >= 'A' && strHexVal[lluStrIdx] <= 'F')
				{
					ucCharVal = strHexVal[lluStrIdx] - 'A' + 10;
				}
				else
				{
					// fetal error, an exceptional character encountered
					unRetVal = 0;
					break;
				}

				unRetVal = unRetVal * 16 + ucCharVal;
			}
		}

		return unRetVal;
	}

#ifdef _cplusplus
};
#endif // _cplusplus

#endif
