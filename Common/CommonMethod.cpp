/***************************************************
 * @file		CommonMethod.cpp
 * @section		Common
 * @class		N/A
 * @brief		N/A
 * @author		bqrmtao@gmail.com
 * @date		2017/11/13
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include "CommonMethod.h"

using namespace std;

/*
 * @brief	split string according pointed splitter
 * @param	string to be split
 * @param	splitter
 * @param	sub-string
 * @return	void
*/
void SplitString(const std::string& strSrc, const std::string& strSplitter, std::vector<std::string>& vecSplittedStr)
{
	// index of character in string
	std::string::size_type nSubStrIdxStart = 0;
	std::string::size_type nSubStrIdxStop = 0;
	std::string::size_type nSubStrIdxTemp = 0;

	vecSplittedStr.clear();

	do
	{
		// find first matched splitter from 
		nSubStrIdxStop = strSrc.find_first_of(strSplitter, nSubStrIdxStart);
		if (nSubStrIdxStop != std::string::npos)
		{
			nSubStrIdxTemp = nSubStrIdxStop;
			while (' ' == strSrc[nSubStrIdxTemp - 1])
			{
				nSubStrIdxTemp--;
			}
			vecSplittedStr.push_back(strSrc.substr(nSubStrIdxStart, nSubStrIdxTemp - nSubStrIdxStart));

			do
			{
				nSubStrIdxStart = strSrc.find_first_not_of(strSplitter, ++nSubStrIdxStop);
			} while (std::string::npos != nSubStrIdxStart && ' ' == strSrc[nSubStrIdxStart]);

			if (std::string::npos == nSubStrIdxStart)
			{
				return;
			}
		}
		else
		{
			vecSplittedStr.push_back(strSrc.substr(nSubStrIdxStart));
		}
	} while (nSubStrIdxStop != std::string::npos);
}