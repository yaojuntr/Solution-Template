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

#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
#include <filesystem>
#include <windows.h>

namespace sf = std::tr2::sys;
#else
#include <experimental/filesystem>
#include <unistd.h>

namespace sf = std::experimental::filesystem;
#endif

#include <fstream>

#include "CommonMethod.h"
#include "Logger.h"

using namespace std;

/*
 * @brief	convert a hexadecimal string into unsigned int
 * @param	strHexVal
 * @return	unsigned int
*/
unsigned int ConvertHexStrToUint(std::string strHexVal)
{
	unsigned int unRetVal = 0;

	// a hexadecimal string should start with "0x" or "0X"
	if ('0' == strHexVal[0] && ('x' == strHexVal[1] || 'X' == strHexVal[1]))
	{
		unsigned char ucCharVal = 0;
		for (size_t ullStrIdx = 2; ullStrIdx < strHexVal.size(); ullStrIdx++)
		{
			if (strHexVal[ullStrIdx] >= '0' && strHexVal[ullStrIdx] <= '9')
			{
				ucCharVal = strHexVal[ullStrIdx] - '0';
			}
			else if (strHexVal[ullStrIdx] >= 'a' && strHexVal[ullStrIdx] <= 'f')
			{
				ucCharVal = strHexVal[ullStrIdx] - 'a' + 10;
			}
			else if (strHexVal[ullStrIdx] >= 'A' && strHexVal[ullStrIdx] <= 'F')
			{
				ucCharVal = strHexVal[ullStrIdx] - 'A' + 10;
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

/*
 * @brief	get the full path of the running program
 * @return	full path of the running program
*/
string GetInitialPath()
{
#if (defined WIN32 || defined _WIN32 || defined _W64 || defined WINCE)
	return sf::initial_path<sf::path>().string();
#else
	char szTempPath[PATH_MAX] = { 0 };
	ssize_t unPathLen = readlink("/proc/self/cwd", szTempPath, PATH_MAX);
	if (-1 == unPathLen)
	{
		return "";
	}

	return string(szTempPath, unPathLen);
#endif
}

/*
 * @brief	read data from disk
 * @param	strFileName
 * @param	pDataBuff: char*&
 * @param	unByteToWrite
 * @param	unByteToSkip
 * @return	result of reading data from disk
*/
bool ReadFromDisk(const string strFileName, char*& pDataBuff, size_t unByteToRead, const size_t unByteToSkip)
{
	sf::path oFullFilePath = sf::complete(sf::path(strFileName));
	if (!sf::exists(oFullFilePath))
	{
		__LOG_ERROR__("failed to locate " + oFullFilePath.string());
		return false;
	}

	ifstream oReadData(oFullFilePath.string().c_str(), ios::in | ios::binary);
	if (!oReadData.good())
	{
		__LOG_ERROR__("failed to open file " + oFullFilePath.string());
		oReadData.close();
		return false;
	}

	// if unByteToRead is set to 0, read all data in file
	if (0 == unByteToRead)
	{
		unByteToRead = oReadData.tellg();

		if (unByteToRead <= unByteToSkip)
		{
			__LOG_ERROR__("total byte(s) of file " + oFullFilePath.string() + " is " + ConvertValToString<size_t>(unByteToRead) + ", byte(s) to skip is " + ConvertValToString<size_t>(unByteToSkip));
			oReadData.close();
			return false;
		}
	}

	if (unByteToSkip > 0)
	{
		try
		{
			oReadData.seekg(unByteToSkip);
		}
		catch (...)
		{
			__LOG_ERROR__("failed to skip " + ConvertValToString<size_t>(unByteToSkip) + " byte(s)");
			oReadData.close();
			return false;
		}
	}

	try
	{
		oReadData.read(pDataBuff, unByteToRead);
	}
	catch (...)
	{
		__LOG_ERROR__("failed to read " + ConvertValToString<size_t>(unByteToRead) + " byte(s)");
		oReadData.close();
		return false;
	}

	oReadData.close();
	return true;
}

/*
 * @brief	split string according pointed splitter
 * @param	string to be split
 * @param	splitter
 * @param	sub-string
 * @return	void
*/
void SplitString(const string& strSrc, const string& strSplitter, vector<string>& vecSplittedStr)
{
	// index of character in string
	string::size_type nSubStrIdxStart = 0;
	string::size_type nSubStrIdxStop = 0;
	string::size_type nSubStrIdxTemp = 0;

	vecSplittedStr.clear();

	do
	{
		// find first matched splitter from 
		nSubStrIdxStop = strSrc.find_first_of(strSplitter, nSubStrIdxStart);
		if (nSubStrIdxStop != string::npos)
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
			} while (string::npos != nSubStrIdxStart && ' ' == strSrc[nSubStrIdxStart]);

			if (string::npos == nSubStrIdxStart)
			{
				return;
			}
		}
		else
		{
			vecSplittedStr.push_back(strSrc.substr(nSubStrIdxStart));
		}
	} while (nSubStrIdxStop != string::npos);
}

/*
 * @brief	write data to disk
 * @param	strFolderPath
 * @param	strFileName
 * @param	pDataBuff: char*&
 * @param	unByteToWrite
 * @param	unByteToSkip
 * @return	result of writing data to disk
*/
bool WriteToDisk(const std::string strFolderPath, const std::string strFileName, char*& pDataBuff, const size_t unByteToWrite, const size_t unByteToSkip)
{
	sf::path oFullFolderPath = sf::complete(sf::path(strFolderPath));
	if (!sf::exists(oFullFolderPath.root_path()))
	{
		__LOG_ERROR__("root path of " + oFullFolderPath.string() + " does not exist");
		return false;
	}

	if (!sf::exists(oFullFolderPath) && (!sf::create_directories(oFullFolderPath)))
	{
		__LOG_ERROR__("failed to create folder " + oFullFolderPath.string());
		return false;
	}

	sf::path oFullFilePath = sf::complete(sf::path(strFileName), oFullFolderPath);
	ofstream oWriteData(oFullFilePath.string().c_str(), ios::out | ios::binary);
	if (!oWriteData.good())
	{
		__LOG_ERROR__("failed to open file " + oFullFilePath.string());
		oWriteData.close();
		return false;
	}

	if (unByteToSkip > 0)
	{
		try
		{
			oWriteData.seekp(unByteToSkip);
		}
		catch (...)
		{
			__LOG_ERROR__("failed to skip " + ConvertValToString<size_t>(unByteToSkip) + " byte(s)");
			oWriteData.close();
			return false;
		}
	}

	try
	{
		oWriteData.write(pDataBuff, unByteToWrite);
	}
	catch (...)
	{
		__LOG_ERROR__("failed to write " + ConvertValToString<size_t>(unByteToWrite) + " byte(s)");
		oWriteData.close();
		return false;
	}

	oWriteData.close();
	return true;
}