/**********************************************
 * @file		ErrorMsg.h
 * @section		Common
 * @class		CErrorMsg
 * @brief		compose error information with template stored in property file
 * @author		bqrmtao@gmail.com
 * @date		2017/05/02 13:30:53
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
 ***********************************************
 * Error id 123456
 *	1 and 2 represent project id
 *	3 represent error level:
 *	4, 5 and 6 represent error id
 ***********************************************
 * Error level
 *	1: Warning
 *	2: Error
 *	3: Fatal  
***********************************************/

#ifndef __ERROR_MSG_H__
#define __ERROR_MSG_H__

#include <string>
#include <map>
#include <vector>

#include "IntlMsgAliasID.h"
#include "MacroDeclSpec.h"

class _DLL_EXPORT_ CErrorMsg
{
public:
	// singleton
	static CErrorMsg* GetInstance();

	// get error message
	std::string GetMsgString();

	// get error message
	std::string GetMsgString(int nKeyName, std::vector<std::string> vecSpecifierReplacers = std::vector<std::string>());

	// set error id and replacer string
	void PushBackMsg(int nKeyName, std::vector<std::string> vecSpecifierReplacers = std::vector<std::string>());
	
private:
	// default constructor
	CErrorMsg(std::string strIntlMsgFileDir);
	~CErrorMsg();

	// read template from plain text file.
	void InitErrorTemplate();

	// singleton pointer
	static CErrorMsg* m_pInst;

	int m_nKeyValue;

	size_t m_nSpecifierIdx;
	size_t m_nPosCurrentIdx;
	size_t m_nPosLeftBrace;
	size_t m_nPosRightBrace;
	size_t m_nLenTemplate;

	std::string m_strPropertyFileDir;
	std::map<int, std::string> m_mErrorTemplate;
	std::map<int, std::string>::iterator m_iterIdx;
	std::vector<std::string> m_vecErrorReplacer;
};

#endif	// __ERROR_MSG_H__