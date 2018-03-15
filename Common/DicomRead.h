/***************************************************
 * @file		DicomRead.h
 * @section		Common
 * @class		N/A
 * @brief		read dicom image
 * @author		bqrmtao@gmail.com
 * @date		2017/10/14
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#ifndef _DICOM_READ_H_
#define _DICOM_READ_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "MacroDefination.h"

#define STR_BUF_LEN	128

/*
 * type of Dicom file
*/
enum DicomVersion
{
	NotDicom,
	Dicom3File,
	DicomOldType,
	DicomUnknow
};

/*
 * @brief	struct of Dicom file
*/
struct DicomInfo
{
	bool isBigEndian;
	bool isSignedData;
	char czModality[2];
	char czPhotoInterpretation[12];
	unsigned short usSamplesPerPixel;
	unsigned short usPixelRepresentation;
	unsigned short usPlanarConfiguration;
	unsigned short usImageHeight;
	unsigned short usImageWidth;
	unsigned short usPixelDepth;
	unsigned short usWinCenter;
	unsigned short usWinWidth;
	unsigned int unDataOffset;
	unsigned int unNumFrames;
	DicomVersion nDicomVersion;
	float fRescaleIntercept;
	float fRescaleSlope;
};

/*
 * @class	CDicomRead
 * @brief	read image data from dicom file
*/
class EXPORT_DLL CDicomRead
{
public:
	/*
	 * @brief	default constructor
	*/
	CDicomRead();
	
	/*
	 * @brief	default destructor
	*/
	~CDicomRead();
	
	/*
	 * @brief	get image information and data from a dicom file
	 * @param	strFileName
	 * @param	pDcmInfo
	 * @param	pDataBuf
	 * @param	unBuffLen
	 * @return	error code
	*/
	int GetInfoAndData(std::string strFileName, DicomInfo *pDcmInfo, char *pDataBuf, size_t unBuffLen);

private:
	/*
	 * @brief	add a tag to dicom information
	 * @param	strTag
	*/
	void AddTag(std::string strTagInfo);
	
	/*
	 * @brief	convert string to number
	*/
	template<typename T>
	T ConvertStr2Num(size_t unStrLen);

	/*
	 * @brief	read next tag' length
	*/
	void GetElementLen();

	/*
	 * @brief	read header according to different tag
	 * @return	header
	*/
	std::string GetHeaderInfo(std::string strTag);

	/*
	 * @brief	read next tag
	*/
	void GetNextTag();
	
	/*
	 * @brief	read value from stream
	*/
	template<typename T>
	T GetStreamValue(size_t unStrLen);
	
	/*
	 * @brief	initialize data in-class
	*/
	void InitData();
	
	/*
	 * @brief	initialize Dicom 3.0 dictionary
	*/
	void InitDictionary();

	/*
	 * @brief	transform integer to string
	 * @param	nInVal: signed and unsigned integer supported
	 * @param	ucBase: the base to be transformed into
	 * @param	unStrValLen: length of transformed string, big endian, the minimum base is stored most right.
	*/
	template<typename T>
	std::string Int2Str(T tInVal, unsigned char ucBase = 10, size_t unStrValLen = 0);
	
	/*
	 * @brief	read bytes from file
	 * @param	unBytesRead: bytes to read
	*/
	void ReadBuf(unsigned int unBytesRead);

	/*
	 * @brief	read dicom file
	 * @return	process result
	*/
	int ReadDicom(size_t unBuffLen);
	
	/*
	 * @brief	read image data
	 * @param	pDataBuf: buffer
	 * @param	unBuffLen: size of buffer
	*/
	int ReadImageData(size_t unBuffLen);
	
	/*
	 * @brief	read dicom info
	 * @return	process result
	*/
	int ReadInfo();
	
	/*
	 * @brief	little endian to big endian
	 * @param	unBufLen: bytes to Reverse
	*/
	void ReverseCopy(unsigned int unBufLen);

	bool m_isBigEndianSyntax;
	bool m_isDcmTagFound;
	bool m_isInSequence;
	bool m_isOddIdx;
	bool m_isPixelDataTagFound;

	unsigned char m_ucPixelValLower;
	unsigned char m_ucPixelValHigher;
	
	int m_nProcResult;
	int m_nStreamIdx;
	int m_nVR;
	int m_nPixVal;

	unsigned int m_unElementLen;
	unsigned int m_unTagVal;
	unsigned int m_unStreamLocation;
	unsigned int m_unGroupWord;
	unsigned int m_unElementWord;
	
	DicomInfo m_oDcmInfo;

	char *m_pDataPtr;

	char m_czStreamBuff[STR_BUF_LEN];
	char m_czHeaderBuf[STR_BUF_LEN];
	char m_czTagBuff[STR_BUF_LEN];
	char m_czHexBuff[STR_BUF_LEN];

	std::string m_strTag;
	std::string m_strFileName;

	std::fstream m_oReadFile;

	std::map<std::string, std::string> m_mapDicomDictionary;

	std::vector<std::string> m_vecErrorReplacer;
};

//#ifdef _cplusplus
//extern "C"
//{
//#endif // _cplusplus
//
//
//#ifdef _cplusplus
//};
//#endif // _cplusplus

#endif
