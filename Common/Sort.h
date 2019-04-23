/***************************************************
 * @file		Sort.h
 * @section		Common
 * @class		CSort, a singleton
 * @brief		sort algorithm
 * @author		bqrmtao@gmail.com
 * @date		2018/09/07
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

/* __DEF_MACRO_H__ */
#ifndef __SORT_ALGORITHMS_H__
#define __SORT_ALGORITHMS_H__

//#include <map>

/*
 * @class	
 * @brief	
 */
class _declspec(dllexport) CSort
{
public:
	static CSort* GetInstance();

	template<typename T>
	void BubbleSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		bool bIsSwapped = false;
		size_t ullLastIdx = unNumItem, ullIdx2nd = 0, ullIdx1st = 0;
		T tmpVal;

		for (ullIdx1st = 0; ullIdx1st < ullLastIdx; ullIdx1st++)
		{
			bIsSwapped = false;

			for (ullIdx2nd = 0; ullIdx2nd < ullLastIdx - 1; ullIdx2nd++)
			{
				if (bIsAscending && pData[ullIdx2nd] > pData[ullIdx2nd + 1] || !bIsAscending && pData[ullIdx2nd] < pData[ullIdx2nd + 1])
				{
					tmpVal = pData[ullIdx2nd];
					pData[ullIdx2nd] = pData[ullIdx2nd + 1];
					pData[ullIdx2nd + 1] = tmpVal;

					bIsSwapped = true;
				}
			}

			if (!bIsSwapped)
			{
				break;
			}
		}
	}

	//template<typename T>
	//void CountingSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	//{
	//	std::map<T, size_t> mapData;
	//	size_t unIdx = 0;
	//
	//	for (unIdx = 0; unIdx < unNumItem; unIdx++)
	//	{
	//		if (mapData.find(pData[unIdx]) != mapData.end())
	//		{
	//			mapData[pData[unIdx]]++;
	//		}
	//		else
	//		{
	//			mapData.insert(std::make_pair(pData[unIdx], 1));
	//		}
	//	}
	//
	//	unIdx = 0;
	//	for (auto iterIdx = mapData.begin(); iterIdx != mapData.end(); iterIdx++)
	//	{
	//		for (size_t unSubIdx = 0; unSubIdx < iterIdx->second; unSubIdx++)
	//		{
	//			pData[unIdx++] = iterIdx->first;
	//		}
	//	}
	//}

	template<typename T>
	void MergeSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		if (1 == unNumItem)
		{
			return;
		}

		if (2 == unNumItem)
		{
			if (bIsAscending && pData[0] > pData[1] || !bIsAscending && pData[0] < pData[1])
			{
				T tmpVal = pData[0];
				pData[0] = pData[1];
				pData[1] = tmpVal;
			}

			return;
		}

		size_t unNumLeft = unNumItem / 2;
		size_t unNumRight = unNumItem - unNumLeft;
		MergeSort<T>(pData, unNumLeft, bIsAscending);
		MergeSort<T>(pData + unNumLeft, unNumRight, bIsAscending);

		// merge
		T *pTmp = new T[unNumItem];
		size_t unIdxLeft = 0;
		size_t unIdxRight = unNumLeft;
		size_t unIdxFull = 0;

		do 
		{
			if (bIsAscending && pData[unIdxLeft] <= pData[unIdxRight] || !bIsAscending && pData[unIdxLeft] >= pData[unIdxRight])
			{
				pTmp[unIdxFull++] = pData[unIdxLeft++];
			}
			else if (bIsAscending && pData[unIdxLeft] > pData[unIdxRight] || !bIsAscending && pData[unIdxLeft] < pData[unIdxRight])
			{
				pTmp[unIdxFull++] = pData[unIdxRight++];
			}
		} while (unIdxLeft < unNumLeft && unIdxRight < unNumItem);

		while (unIdxLeft < unNumLeft)
		{
			pTmp[unIdxFull++] = pData[unIdxLeft++];
		}

		while (unIdxRight < unNumItem)
		{
			pTmp[unIdxFull++] = pData[unIdxRight++];
		}

		memcpy(pData, pTmp, sizeof(T) * unNumItem);
		delete[] pTmp;
	}

	template<typename T>
	void HeapSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		// build a heap, whose value is larger than both branch's.
		size_t unIdx = unNumItem / 2;
		do 
		{
			TraverseHeap(pData, unNumItem, unIdx, bIsAscending);
		} while (unIdx-- > 0);

		// traverse all heap
		T tmpVal;
		for (unIdx = unNumItem - 1; unIdx > 0; unIdx--)
		{
			// switch "largest(smallest)" value with the "rear" of the heap
			tmpVal = pData[0];
			pData[0] = pData[unIdx];
			pData[unIdx] = tmpVal;

			TraverseHeap(pData, unIdx, 0, bIsAscending);
		}
	}

	template<typename T>
	void InsertionSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		size_t ullIdx2nd = 0, ullIdx1st = 0;
		T tmpVal;

		for (ullIdx1st = 1; ullIdx1st < unNumItem; ullIdx1st++)
		{
			tmpVal = pData[ullIdx1st];
			ullIdx2nd = ullIdx1st - 1;

			do 
			{
				if (bIsAscending && tmpVal < pData[ullIdx2nd] || !bIsAscending && tmpVal > pData[ullIdx2nd])
				{
					pData[ullIdx2nd + 1] = pData[ullIdx2nd];
				}
				else
				{
					break;
				}
			} while (ullIdx2nd-- > 0);

			pData[ullIdx2nd + 1] = tmpVal;
		}
	}

	template<typename T>
	void QuickSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		if (unNumItem <= 1)
		{
			return;
		}

		size_t ullIdx1st = 0, ullIdx2nd = unNumItem - 1;
		T tmpVal = pData[ullIdx1st];

		while (ullIdx1st < ullIdx2nd)
		{
			while (ullIdx1st < ullIdx2nd && (bIsAscending && pData[ullIdx2nd] >= tmpVal || !bIsAscending && pData[ullIdx2nd] <= tmpVal))
			{
				ullIdx2nd--;
			}

			pData[ullIdx1st] = pData[ullIdx2nd];

			while (ullIdx1st < ullIdx2nd && (bIsAscending && pData[ullIdx1st] <= tmpVal || !bIsAscending && pData[ullIdx1st] >= tmpVal))
			{
				ullIdx1st++;
			}

			pData[ullIdx2nd] = pData[ullIdx1st];
		}

		pData[ullIdx1st] = tmpVal;

		QuickSort(pData, ullIdx1st, bIsAscending);
		QuickSort(pData + ullIdx1st + 1, unNumItem - ullIdx1st - 1, bIsAscending);
	}

	template<typename T>
	void SelectionSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		size_t ullCurrentIdx = 0, ullIdx2nd = 0, ullIdx1st = 0;
		T tmpVal;

		for (ullIdx1st = 0; ullIdx1st < unNumItem - 1; ullIdx1st++)
		{
			ullCurrentIdx = ullIdx1st;
			tmpVal = pData[ullIdx1st];

			for (ullIdx2nd = ullIdx1st; ullIdx2nd < unNumItem; ullIdx2nd++)
			{
				if (bIsAscending && tmpVal > pData[ullIdx2nd] || !bIsAscending && tmpVal < pData[ullIdx2nd])
				{
					ullCurrentIdx = ullIdx2nd;
					tmpVal = pData[ullIdx2nd];
				}
			}

			if (ullCurrentIdx > ullIdx1st)
			{
				pData[ullCurrentIdx] = pData[ullIdx1st];
				pData[ullIdx1st] = tmpVal;
			}
		}
	}

	template<typename T>
	void ShellSort(T* pData, size_t unNumItem, bool bIsAscending = true)
	{
		size_t ullGapVal = 1, ullGapIdx = 0, ullIdx2nd = 0, ullIdx1st = 0;
		T tmpVal;

		std::vector<size_t> vecGap;
		do 
		{
			vecGap.push_back(ullGapVal);
			ullGapVal = 2 * ullGapVal + 1;
		} while (ullGapVal < unNumItem);

		ullGapIdx = vecGap.size() - 1;
		do
		{
			ullGapVal = vecGap[ullGapIdx];
			for (ullIdx1st = ullGapVal; ullIdx1st < unNumItem; ullIdx1st++)
			{
				tmpVal = pData[ullIdx1st];
				ullIdx2nd = ullIdx1st;

				do 
				{
					if (bIsAscending && tmpVal < pData[ullIdx2nd - ullGapVal] || !bIsAscending && tmpVal > pData[ullIdx2nd - ullGapVal])
					{
						pData[ullIdx2nd] = pData[ullIdx2nd - ullGapVal];
					}
					else
					{
						break;
					}
				} while ((ullIdx2nd -= ullGapVal) >= ullGapVal);

				pData[ullIdx2nd] = tmpVal;
			}
		} while (ullGapIdx-- > 0);
	}
	
private:
	CSort() {}
	~CSort() {}

	template<typename T>
	void TraverseHeap(T* pData, size_t unNumItem, size_t unIdxCurr, bool bIsAscending)
	{
		size_t unIdxLeft = unIdxCurr * 2;
		size_t unIdxRight = unIdxLeft + 1;

		if (unIdxLeft < unNumItem)
		{
			if (bIsAscending && pData[unIdxCurr] < pData[unIdxLeft] || !bIsAscending && pData[unIdxCurr] > pData[unIdxLeft])
			{
				T tmpVal = pData[unIdxCurr];
				pData[unIdxCurr] = pData[unIdxLeft];
				pData[unIdxLeft] = tmpVal;

				// make sure left node is the largest(smallest) of its sub-branch
				TraverseHeap(pData, unNumItem, unIdxLeft, bIsAscending);
			}
		}

		if (unIdxRight < unNumItem)
		{
			if (bIsAscending && pData[unIdxCurr] < pData[unIdxRight] || !bIsAscending && pData[unIdxCurr] > pData[unIdxRight])
			{
				T tmpVal = pData[unIdxCurr];
				pData[unIdxCurr] = pData[unIdxRight];
				pData[unIdxRight] = tmpVal;

				// make sure right node is the largest(smallest) of its sub-branch
				TraverseHeap(pData, unNumItem, unIdxRight, bIsAscending);
			}
		}
	}

	class Garbo
	{
	public:
		~Garbo()
		{
			if ( nullptr != m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = nullptr;
			}
		}
	};

	static CSort* m_pInstance;

	static Garbo oGarbo;
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
//
//#ifdef __cplusplus
//};
//#endif // __cplusplus

#endif	// __SORT_ALGORITHMS_H__