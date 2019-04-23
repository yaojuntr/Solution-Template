/***************************************************
 * @file		TidyCode.h
 * @section		Common
 * @class		CTidyCodes
 * @brief		tidy codes if not pretty
 * @author		bqrmtao@gmail.com
 * @date		2019/02/05
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

/* __DEF_MACRO_H__ */
#ifndef __TIDY_CODE_H__
#define __TIDY_CODE_H__

#include <string>

#include "Logger.h"

/* enum */
enum CommentType
{
	NoComment = 0,   ///< 
	CommentSingleLine,
	CommentMultiLinesLeft,
	CommentMultiLinesRight,
	CommentMultiLinesPair
};

enum BracketType
{
	NoBracket = 0,	///
	BracketLeft,
	BracketRight,
	BracketPair
};

/*
 * @class	CTidyCodes
 * @brief	tidy codes if not pretty
*/
class _declspec(dllexport) CTidyCodes
{
public:
	static CTidyCodes* GetInstance();

	int TidyCpp(std::string strFilenameInput, std::string strFilenameOutput);

private:
	CTidyCodes();
	~CTidyCodes();

	void FindBracket(std::string strCodeLine, BracketType& nBracketType, int& nPosition);

	void FindComment(std::string strCodeLine, CommentType& nCommentType, int& nPosition);

	void FindLetter(std::string strCodeLine, bool bCodeParagraph, int& nPosition);

	void FindPound(std::string strCodeLine, int& nPosition);

	void WriteIndent(std::ofstream oStreamHandle, unsigned int unNumIndent);

	//int m_nTmpPosition;

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

	static CGarbo oGarbo;

	static CTidyCodes* m_pInstance;
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

#endif	// __TIDY_CODE_H__