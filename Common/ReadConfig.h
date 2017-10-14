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

#ifndef __READ_CONFIGURATION_H__
#define __READ_CONFIGURATION_H__

#include <string>
#include <vector>
#include <map>

class _declspec(dllexport) CReadConfig
{
public:
	// singleton
	static CReadConfig* GetInstance();

	// get confiuraton item
	std::string GetCfgItem(const std::string strCfgItemName);

private:
	// default constructor
	CReadConfig();
	~CReadConfig();
	
	// split string get from .ini into desired items
	void SplitString(const std::string & strSrc, const std::string & strSpilter, std::vector<std::string> & vecSplitStr);


	// singleton pointer
	static CReadConfig* m_pIns;

	std::string::size_type m_nSubStrIdxStart;
	std::string::size_type m_nSubStrIdxStop;
	std::string::size_type m_nSubStrIdxTemp;

	std::map<std::string, std::string> m_mapCfgItems;
};

#endif