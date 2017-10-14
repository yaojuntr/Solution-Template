/**********************************************
 * @file		ErrorMsg.h
 * @section		Common
 * @class		CErrorMsg
 * @brief		compose error information with template stored in property file
 * @author		bqrmtao@gmail.com
 * @date		2017/05/02 13:30:53
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
 ***********************************************/

#include <fstream>
#include <mutex>

#include "ErrorMsg.h"
#include "Logger.h"
#include "ReadConfig.h"

#define ERROR_KEY_BITS 6

using namespace std;

CErrorMsg* CErrorMsg::m_pInst = nullptr;
mutex m_oErrorMsgLocker;

CErrorMsg::CErrorMsg(string strIntlMsgFileDir) : m_strPropertyFileDir(strIntlMsgFileDir)
{
	__LOG_FUNC_START__;
	InitErrorTemplate();
	__LOG_FUNC_END__;
}

CErrorMsg::~CErrorMsg()
{
}

CErrorMsg* CErrorMsg::GetInstance()
{
	if (nullptr == m_pInst)
	{
		m_oErrorMsgLocker.lock();
		if (nullptr == m_pInst)
		{
			m_pInst = new CErrorMsg(CReadConfig::GetInstance()->GetCfgItem("ErrorTemplateName"));
		}
		m_oErrorMsgLocker.unlock();
	}
	return m_pInst;
}

void CErrorMsg::InitErrorTemplate()
{
	__LOG_FUNC_START__;

	ifstream oReadErrorTemplate(m_strPropertyFileDir, ios::in);
	char cKeyStr[ERROR_KEY_BITS];

	if (oReadErrorTemplate.good())
	{
		while (!oReadErrorTemplate.eof())
		{
			string strInfo;
			getline(oReadErrorTemplate, strInfo);
	
			memcpy(cKeyStr, strInfo.c_str(), ERROR_KEY_BITS);
	
			m_nKeyValue = atoi(cKeyStr);
			if (m_nKeyValue > 0)
			{
				m_mErrorTemplate.insert(make_pair(m_nKeyValue, string(strInfo, ERROR_KEY_BITS + 1, strInfo.size())));
			}
		}

		__LOG_DEBUG__(to_string(m_mErrorTemplate.size()) + " error template(s) loaded");
	}
	else
	{
		__LOG_ERROR__("fail to open Error Message file: " + m_strPropertyFileDir);
	}

	oReadErrorTemplate.close();

	__LOG_FUNC_END__;
}

string CErrorMsg::GetMsgString()
{
	if (0 == m_nKeyValue)
	{
		return "OK";
	}

	if (m_mErrorTemplate.find(m_nKeyValue) == m_mErrorTemplate.end())
	{
		return "";
	}

	string strErrorMsg;

	m_nPosCurrentIdx = 0;
	m_nPosLeftBrace = 0;
	m_nPosRightBrace = 0;
	m_nSpecifierIdx = 0;
	m_nLenTemplate = m_mErrorTemplate[m_nKeyValue].size();

	while(m_vecErrorReplacer.size() > m_nSpecifierIdx && (m_nPosLeftBrace = m_mErrorTemplate[m_nKeyValue].find("{", m_nPosCurrentIdx)) != string::npos && (m_nPosRightBrace = m_mErrorTemplate[m_nKeyValue].find("}", m_nPosCurrentIdx)) != string::npos) 
	{
		strErrorMsg = m_mErrorTemplate[m_nKeyValue].substr(m_nPosCurrentIdx, m_nPosLeftBrace) + m_vecErrorReplacer[m_nSpecifierIdx++];

		m_nPosCurrentIdx = m_nPosRightBrace + 1;
	}

	strErrorMsg += m_mErrorTemplate[m_nKeyValue].substr(m_nPosCurrentIdx, m_nLenTemplate);

	return strErrorMsg;
}

string CErrorMsg::GetMsgString(int nKeyName, std::vector<std::string> vecSpecifierReplacers)
{
	if (0 == nKeyName)
	{
		return "OK";
	}

	if (m_mErrorTemplate.find(m_nKeyValue) == m_mErrorTemplate.end())
	{
		return "";
	}

	string strErrorMsg;

	m_nPosCurrentIdx = 0;
	m_nPosLeftBrace = 0;
	m_nPosRightBrace = 0;
	m_nSpecifierIdx = 0;
	m_nLenTemplate = m_mErrorTemplate[m_nKeyValue].size();

	while(vecSpecifierReplacers.size() > m_nSpecifierIdx && (m_nPosLeftBrace = m_mErrorTemplate[m_nKeyValue].find("{", m_nPosCurrentIdx)) != string::npos && (m_nPosRightBrace = m_mErrorTemplate[m_nKeyValue].find("}", m_nPosCurrentIdx)) != string::npos) 
	{
		strErrorMsg = m_mErrorTemplate[m_nKeyValue].substr(m_nPosCurrentIdx, m_nPosLeftBrace) + vecSpecifierReplacers[m_nSpecifierIdx++];

		m_nPosCurrentIdx = m_nPosRightBrace + 1;
	}

	strErrorMsg += m_mErrorTemplate[m_nKeyValue].substr(m_nPosCurrentIdx, m_nLenTemplate);

	return strErrorMsg;
}

void CErrorMsg::PushBackMsg(int nKeyName, vector<string> vecSpecifierReplacers)
{
	m_nKeyValue = nKeyName;

	if (0 == m_nKeyValue)
	{
		return;
	}

	m_vecErrorReplacer.clear();
	for (auto strReplacer : vecSpecifierReplacers)
	{
		m_vecErrorReplacer.push_back(strReplacer);
	}
}
