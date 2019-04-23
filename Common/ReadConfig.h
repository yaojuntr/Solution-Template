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

#include "MacroDeclSpec.h"

class _DLL_EXPORT_ CReadConfig
{
public:
	// singleton
	static CReadConfig* GetInstance();
	
	// get configuration item
	std::string GetCfgItem(const std::string strCfgItemName);
	
private:
	// default constructor
	CReadConfig();

	// default destructor
	~CReadConfig();
	
	// singleton pointer
	static CReadConfig* m_pIns;
	
	std::map<std::string, std::string> m_mapCfgItems;
};

#endif	// __READ_CONFIGURATION_H__