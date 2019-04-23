/***************************************************
 * @file		
 * @section		
 * @class		
 * @brief		
 * @author		bqrmtao@gmail.com
 * @date		
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

/* __DEF_MACRO_H__ */
#ifndef __CV_METHOD_H__
#define __CV_METHOD_H__

#include <string>

#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"

#include "MacroDeclSpec.h"

/* enum */
//enum 
//{
//	,   ///< 
//};

/*
 * @class	CCvMethod
 * @brief	commonly used OpenCV method
*/
class _DLL_EXPORT_ CCvMethod
{
public:
	/*
	 * @brief	instance of singleton
	 * @return	pointer
	*/
	static CCvMethod* GetInstance();
	
	/*
	 * @brief	get type of cv::Mat
	 * @param	oInputType: type of variant
	 * @param	ucNumChannel: channel of matrix
	 * @return	type of cv::Mat in int16_t
	*/
	int16_t GetCVType(std::type_info oInputType, uint8_t ucNumChannel);
	
	/*
	 * @brief	show input image
	 * @param	strWindowName : window's name
	 * @param	pImgPtr: image pointer, char*
	 * @param	nImgHeight
	 * @param	nImgWidth
	 * @param	unImgType: type of cv::Mat
	 * @param	bIsDestroyWindow: whether to destroy the pop-up
	 * @return	void
	*/
	void ShowImage(std::string strWindowName, char* pImgPtr, int nImgHeight, int nImgWidth, size_t unImgType, bool bIsDestroyWindow = true);

private:
	/*
	 * @brief	constructor
	*/
	CCvMethod();
	/*
	 * @brief	destructor
	*/
	~CCvMethod();

	void MouseTipCallback(int nEventID, int nRowIdx, int nColIdx, int flags, void *param);
	
	/*
	 * @class	CGarbo
	 * @brief	destructor of singleton
	*/
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

	bool m_bIsMouseDown;

	cv::Point m_cvMouseCoord;

	static CCvMethod* m_pInstance;
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

#endif	// 