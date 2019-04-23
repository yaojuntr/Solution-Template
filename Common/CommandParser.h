/***************************************************
 * @file		CommandParser.h
 * @section		Common
 * @class		CCommandParser
 * @brief		parse command defined in xml
 * @author		bqrmtao@gmail.com
 * @date		2019/03/07
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef __COMMAND_PARSER_H__
#define __COMMAND_PARSER_H__

#include <map>
#include <set>
#include <string>
#include <tuple>

/*
 * @class	CCommandParser
 * @brief	parse command
*/
class _declspec(dllexport) CCommandParser
{
public:
	static CCommandParser *GetInstance();

	void ClearCommands();

	unsigned int ParseCommand(std::string strCommand, std::pair<std::string, std::map<std::string, std::tuple<int, int, float>>>& pairParsedCmd);

private:
	CCommandParser();
	~CCommandParser();

	void InitCommands(std::string strFilePath);

	std::map<std::string, std::set<std::string>> m_mapCommandNames;
	std::map<std::string, std::map<std::string, std::tuple<std::set<std::string>, int, int, float>>> m_mapCommandProperties;

	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (nullptr != m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = nullptr;
			}
		}
	};

	static CCommandParser *m_pInstance;
	static CGarbo m_oGarbo;
};

/*
 * @brief	
 * @param	
 * @return	
*/

//#ifdef __cplusplus
//extern "C"
//{
//#endif // __cplusplus
//
//
//#ifdef __cplusplus
//};
//#endif // __cplusplus

#endif	// __COMMAND_PARSER_H__