/***************************************************
 * @file		Sort.cpp
 * @section		Common
 * @class		CSort, a singleton
 * @brief		sort algorithm
 * @author		bqrmtao@gmail.com
 * @date		2018/09/07
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include <mutex>

#include "Sort.h"

using namespace std;

mutex m_oSortInstanceLock;
CSort* CSort::m_pInstance = nullptr;
CSort::Garbo CSort::oGarbo;

CSort* CSort::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oSortInstanceLock.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CSort();
		}
		m_oSortInstanceLock.unlock();
	}

	return m_pInstance;
}