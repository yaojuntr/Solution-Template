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
#ifndef __CV_FFT_2D_H__
#define __CV_FFT_2D_H__

#include "MacroDeclSpec.h"

/* enum */
//enum 
//{
//	,   ///< 
//};

/*
 * @class	
 * @brief	
*/
class _DLL_EXPORT_ CCvFFT2D
{
public:
	static CCvFFT2D* GetInstance();

	void FFT2D(float* pReal, float *pImag, unsigned int unNumRow, unsigned int unNumCol);
	void iFFT2D(float* pReal, float *pImag, unsigned int unNumRow, unsigned int unNumCol);

private:
	CCvFFT2D();
	~CCvFFT2D();

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

	// static member, the only instance of the class
	static CCvFFT2D* m_pInstance;

	// static member, to destruct singleton
	static CGarbo oGarbo;
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

#endif	// __CV_FFT_2D_H__