/**********************************************
 * @file		ReadConfig.h
 * @section		Common
 * @class		CReadConfig
 * @brief		read config parameters stored in SysCfg.ini
 * @author		bqrmtao@gmail.com
 * @date		2017/05/02 13:27:16
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***********************************************/

#include <mutex>
#include <fstream>

#include "ReadConfig.h"
#include "Logger.h"

using namespace std;

CReadConfig* CReadConfig::m_pIns = nullptr;
mutex m_oReadConfigLocker;

/*
 * @brief	constructor
*/
CReadConfig::CReadConfig()
{
	__LOG_FUNC_START__;

	ifstream oReadCfg("SysCfg.ini", ios::in | ios::binary);
	if (true == oReadCfg.good())
	{
		vector<string> vecSplitStr;
		string strLine;
		while (!oReadCfg.eof())
		{
			getline(oReadCfg, strLine);
			if (strLine.empty() || ';' == strLine[0])
			{
				continue;
			}

			while (!strLine.empty() && (strLine[strLine.length() - 1] == '\r' || strLine[strLine.length() - 1] == '\n'))
			{
				strLine.erase(strLine.length() - 1);
			}

			SplitString(strLine, "=", vecSplitStr);
			if (2 == vecSplitStr.size())
			{
				m_mapCfgItems.insert(make_pair(vecSplitStr[0], vecSplitStr[1]));
			}
		}

		__LOG_DEBUG__(to_string(m_mapCfgItems.size()) + " configuration item(s) loaded");
	}
	else
	{
		oReadCfg.close();
		__LOG_ERROR__("fatal error: SysCfg.ini does not exist");
	}

	__LOG_FUNC_END__;
}

/*
 * @brief	destructor
*/
CReadConfig::~CReadConfig()
{
}

/*
 * @brief	get the instance of the singleton CReadConfig
 * @param	none
 * @return	pointer of the instance of CReadConfig
*/
CReadConfig* CReadConfig::GetInstance()
{
	if (nullptr == m_pIns)
	{
		m_oReadConfigLocker.lock();
		if (nullptr == m_pIns)
		{
			m_pIns = new CReadConfig();
		}
		m_oReadConfigLocker.unlock();
	}

	return m_pIns;
}

/*
 * @brief	get the configuration value according to the item's name
 * @param	item's name
 * @return	configuration value
*/
string CReadConfig::GetCfgItem(const std::string strCfgItemName)
{
	if (m_mapCfgItems.find(strCfgItemName) != m_mapCfgItems.end())
	{
		return m_mapCfgItems.at(strCfgItemName);
	}
	else
	{
		__LOG_ERROR__("fail to locate key value " + strCfgItemName);
		return string("");
	}
}

/*
 * @brief	split string according pointed splitter
 * @param	string to be split
 * @param	splitter
 * @param	sub-string
 * @return	none
*/
void CReadConfig::SplitString(const std::string & strSrc, const std::string & strSpilter, std::vector<std::string> & vecSplitStr)
{
	m_nSubStrIdxStart = 0;
	vecSplitStr.clear();

	do
	{
		m_nSubStrIdxStop = strSrc.find_first_of(strSpilter, m_nSubStrIdxStart);
		if (m_nSubStrIdxStop != string::npos)
		{
			m_nSubStrIdxTemp = m_nSubStrIdxStop;
			while (' ' == strSrc[m_nSubStrIdxTemp - 1])
			{
				m_nSubStrIdxTemp--;
			}
			vecSplitStr.push_back(strSrc.substr(m_nSubStrIdxStart, m_nSubStrIdxTemp - m_nSubStrIdxStart));

			do 
			{
				m_nSubStrIdxStart = strSrc.find_first_not_of(strSpilter, ++m_nSubStrIdxStop);
			} while (string::npos != m_nSubStrIdxStart && ' ' == strSrc[m_nSubStrIdxStart]);

			if (string::npos == m_nSubStrIdxStart)
			{
				return;
			}
		}
		else
		{
			vecSplitStr.push_back(strSrc.substr(m_nSubStrIdxStart));
		}
	} while (m_nSubStrIdxStop != string::npos);
}
