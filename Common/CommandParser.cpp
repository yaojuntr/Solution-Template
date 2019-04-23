/***************************************************
 * @file		CommandParser.cpp
 * @section		Common
 * @class		CCommandParser
 * @brief		parse command defined in xml
 * @author		bqrmtao@gmail.com
 * @date		2019/03/07
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include <mutex>

#include "tinyxml2.h"

#include "CommandParser.h"
#include "CommonMethod.h"
#include "IntlMsgAliasID.h"

using namespace std;

mutex m_oCommandParserLock;

CCommandParser *CCommandParser::m_pInstance = nullptr;
CCommandParser::CGarbo CCommandParser::m_oGarbo;

CCommandParser::CCommandParser()
{
	InitCommands("conf/command_names.xml");
}

CCommandParser::~CCommandParser()
{
	ClearCommands();
}

CCommandParser* CCommandParser::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oCommandParserLock.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CCommandParser();
		}
		m_oCommandParserLock.unlock();
	}

	return m_pInstance;
}

void CCommandParser::ClearCommands()
{
	if (!m_mapCommandNames.empty())
	{
		for (auto iterCommandNames = m_mapCommandNames.begin(); iterCommandNames != m_mapCommandNames.end(); iterCommandNames++)
		{
			iterCommandNames->second.clear();
			//printf("%d\n", iterCommandNames->size());
		}

		m_mapCommandNames.clear();
	}
}

unsigned int CCommandParser::ParseCommand(string strCommand, pair<string, map<string, tuple<int, int, float>>>& pairParsedCmd)
{
	//mapParsedCmd.clear();

	if (m_mapCommandNames.empty() || m_mapCommandProperties.empty())
	{
		return NO_CMD;
	}

	if (strCommand.empty())
	{
		return CMD_NOT_FOUND;
	}

	vector<string> vecCmdInfo;
	SplitString(strCommand, "-", vecCmdInfo);

	string strCmdName = "";

	bool bIsCmdFound = false;
	if (m_mapCommandNames.find(vecCmdInfo[0]) != m_mapCommandNames.end())
	{
		strCmdName = vecCmdInfo[0];
		bIsCmdFound = true;
	}
	else
	{
		for (auto iterCmdName = m_mapCommandNames.begin(); iterCmdName != m_mapCommandNames.end(); iterCmdName++)
		{
			for (auto iterAlias = iterCmdName->second.begin(); iterAlias != iterCmdName->second.end(); iterAlias++)
			{
				if (vecCmdInfo[0] == *iterAlias)
				{
					bIsCmdFound = true;
					strCmdName = iterCmdName->first;
					goto CMD_PARSED_RESULT;
				}
			}
		}
	}

CMD_PARSED_RESULT:
	if (!bIsCmdFound)
	{
		return CMD_NOT_FOUND;
	}

	bool bIsPropertyFound;
	map<string, tuple<set<string>, int, int, float>> mapTmpProperties = m_mapCommandProperties[strCmdName];
	map<string, tuple<int, int, float>> mapParsedProperties;

	if (!mapTmpProperties.empty())
	{
		string strPropertyName = "";
		for (int nPropertyIdx = 1; nPropertyIdx < vecCmdInfo.size(); nPropertyIdx++)
		{
			vector<string> vecPropertyInfo;
			SplitString(vecCmdInfo[nPropertyIdx], " ", vecPropertyInfo);

			if (2 != vecPropertyInfo.size())
			{
				continue;
			}

			bIsPropertyFound = false;
			if (mapTmpProperties.find(vecPropertyInfo[0]) != mapTmpProperties.end())
			{
				bIsPropertyFound = true;
			}
			else
			{
				for (auto iterProperty = mapTmpProperties.begin(); iterProperty != mapTmpProperties.end(); iterProperty++)
				{
					set<string> setAlias = get<0>(iterProperty->second);
					if (!setAlias.empty() && setAlias.find(vecPropertyInfo[0]) != setAlias.end())
					{
						bIsPropertyFound = true;
						strPropertyName = iterProperty->first;
						break;
					}
				}
			}

			if (!bIsPropertyFound)
			{
				continue;
			}

			if (0 == get<1>(mapTmpProperties[strPropertyName]))
			{
				int nPropertyValue = get<2>(mapTmpProperties[strPropertyName]);

				try
				{
					nPropertyValue = atoi(vecPropertyInfo[1].c_str());
				}
				catch (...)
				{
					nPropertyValue = get<2>(mapTmpProperties[strPropertyName]);
				}

				mapParsedProperties.insert(make_pair(strPropertyName, make_tuple(0, nPropertyValue, 0.0f)));
			}
			else if (1 == get<1>(mapTmpProperties[strPropertyName]))
			{
				float fPropertyValue = get<3>(mapTmpProperties[strPropertyName]);

				try
				{
					fPropertyValue = atof(vecPropertyInfo[1].c_str());
				}
				catch (...)
				{
					fPropertyValue = get<3>(mapTmpProperties[strPropertyName]);
				}

				mapParsedProperties.insert(make_pair(strPropertyName, make_tuple(1, 0, fPropertyValue)));
			}
		}
	}

	if (mapParsedProperties.size() != mapTmpProperties.size())
	{
		for (auto iterDefaultProperty = mapTmpProperties.begin(); iterDefaultProperty != mapTmpProperties.end(); iterDefaultProperty++)
		{
			if (mapParsedProperties.find(iterDefaultProperty->first) == mapParsedProperties.end())
			{
				mapParsedProperties.insert(make_pair(iterDefaultProperty->first, make_tuple(get<1>(mapTmpProperties[iterDefaultProperty->first]), get<2>(mapTmpProperties[iterDefaultProperty->first]), get<3>(mapTmpProperties[iterDefaultProperty->first]))));
			}
		}
	}

	pairParsedCmd = make_pair(strCmdName, mapParsedProperties);

	return STATUS_OK;
}

void CCommandParser::InitCommands(string strFilePath)
{
	ClearCommands();

	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
	if (pDoc->LoadFile(strFilePath.c_str()) != 0)
	{
		printf("fail to read mode supported.\n");
		return;
	}

	tinyxml2::XMLElement* pRootNode = pDoc->RootElement();  
	tinyxml2::XMLElement* pCommandName = pRootNode->FirstChildElement("command_name");  

	while (pCommandName)
	{
		set<string> setCommandNames;

		string strCommandName = pCommandName->FirstAttribute()->Value();
		setCommandNames.insert(strCommandName);

		if (NULL != pCommandName->FirstChildElement("alias"))
		{
			vector<string> vecCommandName;
			string strCommand = string(pCommandName->FirstChildElement("alias")->GetText());
			SplitString(pCommandName->FirstChildElement("alias")->GetText(), ",", vecCommandName);

			for (int nIdx = 0; nIdx < vecCommandName.size(); nIdx++)
			{
				setCommandNames.insert(vecCommandName[nIdx]);
			}
		}

		m_mapCommandNames.insert(make_pair(strCommandName, setCommandNames));

		// properties
		{
			map<string, tuple<set<string>, int, int, float>> mapCmdProperties;

			tinyxml2::XMLElement *pPropertiesAttr = pCommandName->FirstChildElement("properties");
			if (NULL != pPropertiesAttr)
			{

				tinyxml2::XMLElement *pFeatureName = pPropertiesAttr->FirstChildElement();
				while (pFeatureName)
				{
					//set<tuple<set<string>, type_info>> setFeature;

					set<string> setFeatureName;

					string strPropertiesName = pFeatureName->FirstAttribute()->Value();
					setFeatureName.insert(strPropertiesName);

					tinyxml2::XMLElement *pAlias = pFeatureName->FirstChildElement("alias");
					tinyxml2::XMLElement *pVariableType = pFeatureName->FirstChildElement("variable_type");
					tinyxml2::XMLElement *pDefaultValue = pFeatureName->FirstChildElement("default");
					if (NULL != pAlias && NULL != pVariableType && NULL != pDefaultValue)
					{
						vector<string> vecFeatureName;
						SplitString(pAlias->GetText(), ",", vecFeatureName);

						//tuple<set<string>, type_info, int, float> tupleFeature;

						for (int nIdx = 0; nIdx < vecFeatureName.size(); nIdx++)
						{
							setFeatureName.insert(vecFeatureName[nIdx]);
						}

						string strVariableType = pVariableType->GetText();
						if ("int" == strVariableType)
						{
							mapCmdProperties.insert(make_pair(strPropertiesName, make_tuple(setFeatureName, 0, pDefaultValue->IntText(), 0.0f)));
						}
						else if ("float" == strVariableType)
						{
							mapCmdProperties.insert(make_pair(strPropertiesName, make_tuple(setFeatureName, 1, 0, pDefaultValue->FloatText())));
						}
					}

					pFeatureName = pFeatureName->NextSiblingElement();
				}
			}

			m_mapCommandProperties.insert(make_pair(strCommandName, mapCmdProperties));
		}

		pCommandName =  pCommandName->NextSiblingElement();
	}
}