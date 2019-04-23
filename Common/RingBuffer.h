/**********************************************
 * @file		RingBuffer.h
 * @section		Common
 * @class		CRingBufferInfo, a singleton
 * @brief		ring buffer
 * @author		bqrmtao@gmail.com
 * @date		2019/03/21
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***********************************************/

#ifndef __GAIN_RING_H__
#define __GAIN_RING_H__

/*
 * @class	
 * @brief	
*/
class _declspec(dllexport) CRingBufferInfo
{
public:
	static CRingBufferInfo* GetInstance();

	void BufferSize(unsigned int& unBufferSize);

	unsigned int Read(int& nVal);

	void Reset();

	unsigned int Write(int nVal);

private:
	CRingBufferInfo();
	~CRingBufferInfo() {}

	// to destruct singleton
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

	// static member, to destruct singleton
	static CGarbo oCGarbo;

	static CRingBufferInfo* m_pInstance;

	bool m_bIsBufferInit;

	unsigned int m_unFrontIdx;
	unsigned int m_unRearIdx;
	unsigned int m_unRingFullSize;

	int *m_pVal;
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