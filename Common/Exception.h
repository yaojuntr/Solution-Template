/***************************************************
 * @file		Exception
 * @section		Common
 * @class		CException
 * @brief		wrap exceptions thrown by C++
 * @author		bqrmtao@gmail.com
 * @date		2017/10/14
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/
 
#ifndef __BASIC_EXCEPTION_H__
#define __BASIC_EXCEPTION_H__

#include <exception>
#include <string>

#include "Logger.h"
#include "MacroDeclSpec.h"
 
/*
 * @class	
 * @brief	
*/
class _DLL_EXPORT_ CException
{
public:
	CException(int nErrID, std::string strErrMsg);
	~CException();

	int GetErrID() { return m_nErrID; }
	std::string What() { return m_strIntlMsg; }

private:
	int m_nErrID;
	std::string m_strIntlMsg;
};
 
/*
 * @brief		
 * @param		
 * @return		
*/
CException::CException(int nErrID, std::string strErrMsg)
{
	m_nErrID = nErrID;
	m_strIntlMsg = strErrMsg;
}
 
/*
 * @brief		
 * @return		
*/
CException::~CException()
{
}
 
#endif	// __BASIC_EXCEPTION_H__