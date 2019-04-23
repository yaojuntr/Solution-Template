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

#include "opencv/cv.h"

#include "CvFFT2D.h"

using namespace std;

CCvFFT2D* CCvFFT2D::m_pInstance = nullptr;
mutex m_oFFTLock;

CCvFFT2D::CCvFFT2D()
{
}

CCvFFT2D::~CCvFFT2D()
{
}

CCvFFT2D* CCvFFT2D::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oFFTLock.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CCvFFT2D();
		}
		m_oFFTLock.unlock();
	}

	return m_pInstance;
}

void CCvFFT2D::FFT2D(float* pReal, float *pImag, unsigned int unNumRow, unsigned int unNumCol)
{
	cv::Mat cvPlanes[] = { cv::Mat(unNumRow, unNumCol, CV_32F, pReal), cv::Mat(unNumRow, unNumCol, CV_32F, pImag) };	
	cv::Mat cvComplexI;
	cv::merge(cvPlanes, 2, cvComplexI);	// Add to the expanded another plane with zeros
	cv::dft(cvComplexI, cvComplexI);	// this way the result may fit in the source matrix
	cv::split(cvComplexI, cvPlanes);
}

void CCvFFT2D::iFFT2D(float* pReal, float *pImag, unsigned int unNumRow, unsigned int unNumCol)
{
	cv::Mat cvPlanes[] = { cv::Mat(unNumRow, unNumCol, CV_32F, pReal), cv::Mat(unNumRow, unNumCol, CV_32F, pImag) };	
	cv::Mat cvComplexI;
	cv::merge(cvPlanes, 2, cvComplexI);	// Add to the expanded another plane with zeros
	cv::idft(cvComplexI, cvComplexI);	// this way the result may fit in the source matrix
	cvComplexI /= (unNumRow * unNumCol);
	cv::split(cvComplexI, cvPlanes);
}