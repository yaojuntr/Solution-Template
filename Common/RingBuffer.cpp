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

#include <mutex>

#include "IntlMsgAliasID.h"
#include "RingBuffer.h"

#define  NUM_RING_BUFFER_SIZE	10

using namespace std;

CRingBufferInfo* CRingBufferInfo::m_pInstance = nullptr;

mutex m_oInfoRingLocker;

CRingBufferInfo::CRingBufferInfo()
{
	m_unFrontIdx = 0;
	m_unRearIdx = 0;
	m_unRingFullSize = NUM_RING_BUFFER_SIZE - 1;

	try
	{
		m_pVal = new int[NUM_RING_BUFFER_SIZE];

		m_bIsBufferInit = true;
	}
	catch (bad_alloc &eBadAlloc)
	{
		m_pVal = nullptr;

		m_bIsBufferInit = false;
	}
}

CRingBufferInfo* CRingBufferInfo::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oInfoRingLocker.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CRingBufferInfo();
		}
		m_oInfoRingLocker.unlock();
	}

	return m_pInstance;
}

void CRingBufferInfo::BufferSize(unsigned int& unBufferSize)
{
	unBufferSize = (m_unRearIdx + NUM_RING_BUFFER_SIZE - m_unFrontIdx) % NUM_RING_BUFFER_SIZE - 1;
}

unsigned int CRingBufferInfo::Read(int& nVal)
{
	if (!m_bIsBufferInit)
	{
		nVal = 0;

		return RING_BUFFER_UNINITIALIZED;
	}

	if (m_unFrontIdx == m_unRearIdx)
	{
		nVal = 0;

		return RING_BUFFER_EPTY;
	}

	nVal = m_pVal[m_unFrontIdx];

	if (++m_unFrontIdx >= NUM_RING_BUFFER_SIZE)
	{
		m_unFrontIdx = 0;
	}

	return STATUS_OK;
}

unsigned int CRingBufferInfo::Write(int nVal)
{
	if (!m_bIsBufferInit)
	{
		return RING_BUFFER_UNINITIALIZED;
	}

	if (m_unFrontIdx == (m_unRearIdx + 1) % NUM_RING_BUFFER_SIZE)
	{
		return RING_BUFFER_FULL;
	}

	//if (m_unFrontIdx == m_unRearIdx + 1 || 0 == m_unFrontIdx && m_unRearIdx == m_unRingFullSize)
	//{
	//	return RING_BUFFER_FULL;
	//}

	m_pVal[m_unRearIdx] = nVal;

	if (++m_unRearIdx >= NUM_RING_BUFFER_SIZE)
	{
		m_unRearIdx = 0;
	}

	return STATUS_OK;
}