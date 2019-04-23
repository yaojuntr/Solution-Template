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

#include <mutex>

#include "CvMethod.h"

using namespace std;

CCvMethod* CCvMethod::m_pInstance = nullptr;
mutex m_oShowImageLock;

/*
 * @brief	constructor
*/
CCvMethod::CCvMethod()
{
}

/*
 * @brief	destructor
*/
CCvMethod::~CCvMethod()
{
}

/*
 * @brief	instance of singleton
 * @return	pointer
*/
CCvMethod* CCvMethod::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oShowImageLock.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CCvMethod();
		}
		m_oShowImageLock.unlock();
	}

	return m_pInstance;
}

/*
 * @brief	get type of cv::Mat
 * @param	oInputType: type of variant
 * @param	ucNumChannel: channel of matrix
 * @return	type of cv::Mat in int16_t
*/
int16_t CCvMethod::GetCVType(std::type_info oInputType, uint8_t ucNumChannel)
{
	int16_t sDepth = -1;

	if (oInputType == typeid(uint8_t))
	{
		sDepth = CV_8U;
	}
	else if (oInputType == typeid(int8_t))
	{
		sDepth = CV_8S;
	}
	else if (oInputType == typeid(uint16_t))
	{
		sDepth = CV_16U;
	}
	else if (oInputType == typeid(int16_t))
	{
		sDepth = CV_16S;
	}
	else if (oInputType == typeid(int32_t))
	{
		sDepth = CV_32S;
	}
	else if (oInputType == typeid(float))
	{
		sDepth = CV_32F;
	}
	else if (oInputType == typeid(double))
	{
		sDepth = CV_64F;
	}

	if (-1 != sDepth)
	{
		// variable type of CV_YCX, such as CV_16SC1
		// CV_DEPTH = CV_Y + (X - 1) * 8
		sDepth += (ucNumChannel - 1) << 3;
	}

	return sDepth;
}

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
void CCvMethod::ShowImage(string strWindowName, char* pImgPtr, int nImgHeight, int nImgWidth, size_t unImgType, bool bIsDestroyWindow)
{
	cv::Mat cvMatSrc = cv::Mat(nImgHeight, nImgWidth, unImgType, pImgPtr);
	cv::Mat cvMatTmp;
	cvMatSrc.convertTo(cvMatTmp, CV_32FC1);

	double dMinVal = 0;
	double dMaxVal = 0;
	cv::minMaxLoc(cvMatTmp, &dMinVal, &dMaxVal);
	cv::addWeighted(cvMatTmp, 255.0 / (dMaxVal - dMinVal), cvMatTmp, 0, -dMinVal * 255.0 / (dMaxVal - dMinVal), cvMatTmp);
	cv::Mat cvMatShow;
	cvMatTmp.convertTo(cvMatShow, CV_8UC1);

	cv::namedWindow(strWindowName.c_str(), 0);
	cv::imshow(strWindowName.c_str(), cvMatShow);
	cv::waitKey(0);
	
	if (bIsDestroyWindow)
	{
		cv::destroyWindow(strWindowName.c_str());
	}
	
	cvMatShow.release();
	cvMatTmp.release();
	cvMatSrc.release();
}

void CCvMethod::MouseTipCallback(int nEventID, int nRowIdx, int nColIdx, int flags, void *param)
{
}