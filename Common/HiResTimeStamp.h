/***************************************************
 * @file		HiResTimeStamp.h
 * @section		Common
 * @class		CHiResTimeStamp
 * @brief		time stamp of std::chrono
 * @author		bqrmtao@gmail.com
 * @date		2018/09/18
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

/* __DEF_MACRO_H__ */
#ifndef __HI_RES_TIME_STAMP_H__
#define __HI_RES_TIME_STAMP_H__

#include "MacroDeclSpec.h"

/* enum for resolution */
enum ClockResolution
{
	NanoSecond = 0,	///< nanosecond
	MicroSecond,	///< microsecond
	MilliSecond,	///< millisecond
	Second,			///< second
	Minute,			///< minute
	Hour			///< hour
};

struct TimeReso
{
	unsigned char ucHour;
	unsigned char ucMDay;
	unsigned char ucMinute;
	unsigned char ucMonth;
	unsigned char ucSecond;
	unsigned char ucWDay;
	unsigned short usYDay;
	unsigned short usYear;
	unsigned short usMillisecond;
	unsigned long long ullTimeStamp;
};

/*
 * @class	
 * @brief	
 */
class _DLL_EXPORT_ CHiResTimeStamp
{
public:
	/*
	 * @brief	generate object pointer of singleton
	 * @return	CHiResTimeStamp*
	*/
	static CHiResTimeStamp* GetInstance();
	
	/*
	 * @brief	get current time stamp
	 * @param	oTime, structure of time
	 * @return	void
	*/
	void GetCurrentTimeStamp(TimeReso &oTime);
	/*
	 * @brief	get time structure from an integer
	 * @param	ullTimeVal, integer of input time
	 * @param	nClockReso, clock resolution of input time
	 * @param	oTime, structure of time
	 * @return	void
	*/
	void GetTimeFromValue(unsigned long long ullTimeVal, ClockResolution nClockReso, TimeReso &oTime);

private:
	/*
	 * @brief	constructor
	*/
	CHiResTimeStamp();
	/*
	 * @brief	destructor
	*/
	~CHiResTimeStamp();
	
	/*
	 * @brief	convert integer time to time structure
	 * @param	oTime, structure of time
	 * @return	void
	*/
	void ConvertUnsignedLongToStruct(TimeReso &oTime);
	
	/*
	 * @class	Garbo
	 * @brief	recollect resources of singleton
	*/
	class Garbo
	{
	public:
		~Garbo()
		{
			if (nullptr != m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = nullptr;
			}
		}
	};

	static CHiResTimeStamp* m_pInstance;
	static Garbo oGarbo;
};

#endif	// __HI_RES_TIME_STAMP_H__