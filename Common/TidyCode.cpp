/***************************************************
 * @file		TidyCode.cpp
 * @section		Common
 * @class		CTidyCodes
 * @brief		tidy codes if not pretty
 * @author		bqrmtao@gmail.com
 * @date		2019/02/05
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include <fstream>
#include <iostream>
#include <mutex>
#include <stdio.h>

#include "TidyCode.h"

using namespace std;

#define STR_BUF_LEN	2048

CTidyCodes* CTidyCodes::m_pInstance = nullptr;

mutex m_oTidyCodesLock;

CTidyCodes::CTidyCodes()
{
}

CTidyCodes::~CTidyCodes()
{
}

CTidyCodes* CTidyCodes::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oTidyCodesLock.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CTidyCodes();
		}
		m_oTidyCodesLock.unlock();
	}

	return m_pInstance;
}

int CTidyCodes::TidyCpp(string strFilenameInput, string strFilenameOutput)
{
	bool bCodeParagraph = false;
	bool bHeadSkipped = false;
	bool bLeftComment = false;

	unsigned short usNumIndent = 0;
	unsigned short usNumParenthesis = 0;
	unsigned short usNumBracket = 0;

	//int nProcessResult = 0;
	int nCommentPosition = 0;
	int nLetterPosition = 0;
	int nBracketPosition = 0;

	int nIdx = 0;

	unsigned int nLineIdx = 0;

	CommentType nCommentType;
	BracketType nBracketType;

	string strLine;
	
	ifstream oReadCode(strFilenameInput.c_str(), ios::in | ios::binary);
	if (!oReadCode.good())
	{
		oReadCode.close();
		
		printf("fail to open %s\n", strFilenameInput.c_str());
		__LOG_ERROR__("fail to open " + strFilenameInput);
		
		return -1;
	}

	ofstream oWriteCode(strFilenameOutput.c_str(), ios::out | ios::binary);
	if (!oWriteCode.good())
	{
		oReadCode.close();
		
		printf("fail to open %s\n", strFilenameOutput.c_str());
		__LOG_ERROR__("fail to open " + strFilenameOutput);

		if (oReadCode.good())
		{
			oReadCode.close();
		}

		return -1;
	}

	while (!oReadCode.eof())
	{
		getline(oReadCode, strLine);

		nLineIdx++;
		
		printf("%s\n", strLine.c_str());

		for (nIdx = 0; nIdx < usNumIndent; nIdx++)
		{
			oWriteCode.write("\t", 1);
			printf("\t");
		}

		if (strLine.empty())
		{
			oWriteCode.write("\r\n", 2);
			printf("\r\n");

			continue;
		}

		// find file header
		//if (!bHeadSkipped)
		//{
		FindComment(strLine, nCommentType, nCommentPosition);

		FindLetter(strLine, usNumIndent, nLetterPosition);

		// no comment and real code starts
		//if (!bCodeStart && 0 == nCommentType && nLetterPosition >= 0)
		//{
		//	bCodeStart = true;
		//}

		// single line comment
		if (CommentSingleLine == nCommentType && (nCommentPosition < nLetterPosition || -1 == nLetterPosition))
		{
			__LOG_DEBUG__("single line");

			oWriteCode.write(strLine.c_str(), strLine.size());
			printf("%s\n", strLine.c_str());

			continue;
		}

		// multi lines comment in single line
		if (CommentMultiLinesPair == nCommentType && (nCommentPosition < nLetterPosition || -1 == nLetterPosition))
		{
			__LOG_DEBUG__("single line");

			oWriteCode.write(strLine.c_str(), strLine.size());
			printf("%s\n", strLine.c_str());

			continue;
		}

		// multi lines comment, left part
		if (CommentMultiLinesLeft == nCommentType && (nCommentPosition < nLetterPosition || -1 == nLetterPosition))
		{
			bLeftComment = true;

			__LOG_DEBUG__("multi lines comment, left part");

			oWriteCode.write(strLine.c_str(), strLine.size());
			printf("%s\n", strLine.c_str());

			continue;
		}

		if (bLeftComment)
		{
			oWriteCode.write(strLine.c_str(), strLine.size());
			printf("%s\n", strLine.c_str());

			if (CommentMultiLinesRight == nCommentType)
			{
				__LOG_DEBUG__("multi lines comment, right part");

				bLeftComment = false;
			}

			continue;
		}

		// find code paragraph
		FindBracket(strLine, nBracketType, nBracketPosition);
		switch (nBracketType)
		{
		case BracketLeft:
			oWriteCode.write("{\r\n", 3);
			printf("{\n");

			usNumIndent++;

			bCodeParagraph = true;

			// extra codes
			if (nLetterPosition > nBracketPosition)
			{
				string strSubCode = strLine.substr(nLetterPosition, strLine.size());
				oWriteCode.write(strSubCode.c_str(), strSubCode.size());
				printf("%s\n", strLine.c_str());
			}

			continue;
		case BracketRight:
			oWriteCode.seekp(-1, ios::end);
			oWriteCode.write("}\r\n}", 3);
			printf("}\n");
			usNumIndent--;
			bCodeParagraph = false;

			continue;
		case BracketPair:
			oWriteCode.write("{\r\n", 3);
			printf("{\n");

			for (nIdx = 0; nIdx < usNumIndent + 1; nIdx++)
			{
				oWriteCode.write("\t", 1);
				printf("\t");
			}

			// extra codes
			if (nLetterPosition > nBracketPosition)
			{
				string strSubCode = strLine.substr(nLetterPosition, strLine.size());
				oWriteCode.write(strSubCode.c_str(), strSubCode.size());
				printf("%s\n", strLine.c_str());
			}

			for (nIdx = 0; nIdx < usNumIndent; nIdx++)
			{
				oWriteCode.write("\t", 1);
				printf("\t");
			}

			oWriteCode.write("}\r\n}", 3);
			printf("}\n");

			continue;
		default:
			break;
		}

		if (BracketLeft == nBracketType)
		{
			oWriteCode.write("{\r\n", 3);
			printf("{\n");

			usNumIndent++;

			bCodeParagraph = true;

			// extra codes
			if (nLetterPosition > nBracketPosition)
			{
				string strSubCode = strLine.substr(nLetterPosition, strLine.size());
				oWriteCode.write(strSubCode.c_str(), strSubCode.size());
				printf("%s\n", strLine.c_str());
			}

			continue;
		}
		else if (BracketRight == nBracketType)
		{
			oWriteCode.seekp(-1, ios::end);
			oWriteCode.write("}\r\n}", 3);
			printf("}\n");
			usNumIndent--;
			bCodeParagraph = false;

			continue;
		}

		// empty line
		if (nLetterPosition < 0 && nCommentPosition < 0)
		{
			oWriteCode.write(strLine.c_str(), strLine.size());
			printf("%s\n", strLine.c_str());
			continue;
		}

		// real codes starts here
		if ('#' == strLine[nLetterPosition])
		{
			oWriteCode.seekp(-usNumIndent, ios::end);
		}

		if (nLetterPosition >= 0)
		{
			// add {} for if, else, while and for, if missing




			string strSubCode = strLine.substr(nLetterPosition, strLine.size());
			oWriteCode.write(strSubCode.c_str(), strSubCode.size());
			printf("%s\n", strSubCode.c_str());
		}

		// find keyword

		// find parenthesis pair

		// find bracket pair
	}
}

void CTidyCodes::FindBracket(string strCodeLine, BracketType& nBracketType, int& nPosition)
{
	int nTmp = 0;

	nPosition = -1;
	nBracketType = NoBracket;

	do 
	{
		nTmp = strCodeLine.find("}");
		if (nTmp >= 0)
		{
			nPosition = nTmp;
			nBracketType = BracketRight;
		}

		nTmp = strCodeLine.find("{");
		if (nTmp >= 0)
		{
			if (nPosition > 0)
			{
				nBracketType = BracketPair;
			}
			else
			{
				nBracketType = BracketLeft;
			}

			nPosition = nTmp;
		}
	} while (0);
}

void CTidyCodes::FindComment(string strCodeLine, CommentType& nCommentType, int& nPosition)
{
	int nTmp = 0;

	nPosition = -1;
	nCommentType = NoComment;

	do 
	{
		nTmp = strCodeLine.find("//");
		if (nTmp >= 0)
		{
			nPosition = nTmp;
			nCommentType = CommentSingleLine;
			break;
		}

		nTmp = strCodeLine.find("*/");
		if (nTmp >= 0)
		{
			nPosition = nTmp;
			nCommentType = CommentMultiLinesRight;
		}
	
		nTmp = strCodeLine.find("/*");
		if (nTmp >= 0)
		{
			if (CommentMultiLinesRight == nCommentType)
			{
				nCommentType = CommentMultiLinesPair;
			}
			else
			{
				nCommentType = CommentMultiLinesLeft;
			}

			nPosition = nTmp;

			break;
		}
	} while (0);
}

void CTidyCodes::FindLetter(string strCodeLine, bool bCodeParagraph, int& nPosition)
{
	nPosition = -1;

	for (int nIdx = 0; nIdx < strCodeLine.size(); nIdx++)
	{
		if ('#' == strCodeLine[nIdx]
			|| strCodeLine[nIdx] >= 'a' && strCodeLine[nIdx] <= 'z'
			|| strCodeLine[nIdx] >= 'A' && strCodeLine[nIdx] <= 'Z'
			|| strCodeLine[nIdx] >= '0' && strCodeLine[nIdx] <= '9'
			|| strCodeLine[nIdx] == '('
			|| strCodeLine[nIdx] == '"'
			|| bCodeParagraph && '*' == strCodeLine[nIdx])
		{
			nPosition = nIdx;
			break;
		}
	}
}

void CTidyCodes::FindPound(std::string strCodeLine, int& nPosition)
{
	nPosition = strCodeLine.find("#");
}

void CTidyCodes::WriteIndent(ofstream oStreamHandle, unsigned int unNumIndent)
{
	for (int nIdx = 0; nIdx < unNumIndent; nIdx++)
	{
		oStreamHandle.write("\t", 1);
	}
}