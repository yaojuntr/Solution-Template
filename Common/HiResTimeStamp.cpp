/***************************************************
 * @file		HiResTimeStamp.cpp
 * @section		Common
 * @class		CHiResTimeStamp
 * @brief		time stamp of std::chrono
 * @author		bqrmtao@gmail.com
 * @date		2018/09/18
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include <chrono>
#include <mutex>

#include "HiResTimeStamp.h"

using namespace std;

mutex m_oHiResTimeStampLock;

CHiResTimeStamp* CHiResTimeStamp::m_pInstance = nullptr;
CHiResTimeStamp::Garbo CHiResTimeStamp::oGarbo;

/*
 * @brief	constructor
*/
CHiResTimeStamp::CHiResTimeStamp()
{
	m_pInstance = nullptr;
}

/*
 * @brief	destructor
*/
CHiResTimeStamp::~CHiResTimeStamp()
{
}

/*
 * @brief	generate object pointer of singleton
 * @return	CHiResTimeStamp*
*/
CHiResTimeStamp* CHiResTimeStamp::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_oHiResTimeStampLock.lock();
		if (nullptr == m_pInstance)
		{
			m_pInstance = new CHiResTimeStamp();
		}
		m_oHiResTimeStampLock.unlock();
	}

	return m_pInstance;
}

/*
 * @brief	get current time stamp
 * @param	oTime, structure of time
 * @return	void
*/
void CHiResTimeStamp::GetCurrentTimeStamp(TimeReso &oTime)
{
	memset(&oTime, 0, sizeof(TimeReso));

	oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	oTime.usMillisecond = (chrono::duration_cast<chrono::milliseconds>(chrono::milliseconds(oTime.ullTimeStamp)) - chrono::duration_cast<chrono::seconds>(chrono::milliseconds(oTime.ullTimeStamp))).count();
	
	ConvertUnsignedLongToStruct(oTime);
}

/*
 * @brief	get time structure from an integer
 * @param	ullTimeVal, integer of input time
 * @param	nClockReso, clock resolution of input time
 * @param	oTime, structure of time
 * @return	void
*/
void CHiResTimeStamp::GetTimeFromValue(unsigned long long ullTimeVal, ClockResolution nClockReso, TimeReso &oTime)
{
	memset(&oTime, 0, sizeof(TimeReso));

	switch (nClockReso)
	{
	case NanoSecond:
		oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::nanoseconds(ullTimeVal)).count();
		break;
	case MicroSecond:
		oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::microseconds(ullTimeVal)).count();
		break;
	case MilliSecond:
		oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::milliseconds(ullTimeVal)).count();
		break;
	case Second:
		oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::seconds(ullTimeVal)).count();
		break;
	case Minute:
		oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::minutes(ullTimeVal)).count();
		break;
	case Hour:
		oTime.ullTimeStamp = chrono::duration_cast<chrono::milliseconds>(chrono::hours(ullTimeVal)).count();
		break;
	default:
		printf("error\n");
		return;
	}

	ConvertUnsignedLongToStruct(oTime);

	if (nClockReso > MilliSecond)
	{
		oTime.usMillisecond = 0;
	}
	else
	{
		switch (nClockReso)
		{
		case NanoSecond:
			oTime.usMillisecond = (chrono::duration_cast<chrono::milliseconds>(chrono::nanoseconds(oTime.ullTimeStamp)) - chrono::duration_cast<chrono::seconds>(chrono::nanoseconds(oTime.ullTimeStamp))).count();
			break;
		case MicroSecond:
			oTime.usMillisecond = (chrono::duration_cast<chrono::milliseconds>(chrono::microseconds(oTime.ullTimeStamp)) - chrono::duration_cast<chrono::seconds>(chrono::microseconds(oTime.ullTimeStamp))).count();
			break;
		case MilliSecond:
			oTime.usMillisecond = (chrono::duration_cast<chrono::milliseconds>(chrono::milliseconds(oTime.ullTimeStamp)) - chrono::duration_cast<chrono::seconds>(chrono::milliseconds(oTime.ullTimeStamp))).count();
			break;
		}
	}
}

/*
 * @brief	convert integer time to time structure
 * @param	oTime, structure of time
 * @return	void
*/
void CHiResTimeStamp::ConvertUnsignedLongToStruct(TimeReso &oTime)
{
	time_t cTime = chrono::system_clock::to_time_t(chrono::time_point<chrono::system_clock>(chrono::milliseconds(oTime.ullTimeStamp)));
	tm* pLocalTime = localtime(&cTime);

	oTime.usYear = pLocalTime->tm_year + 1900;
	oTime.ucMonth = pLocalTime->tm_mon + 1;
	oTime.ucMDay = pLocalTime->tm_mday;
	oTime.ucHour = pLocalTime->tm_hour;
	oTime.ucMinute = pLocalTime->tm_min;
	oTime.ucSecond = pLocalTime->tm_sec;
	oTime.ucWDay = pLocalTime->tm_wday;
	oTime.usYDay = pLocalTime->tm_yday;
}