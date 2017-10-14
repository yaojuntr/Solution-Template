/**********************************************
 * @file		HiResTimer.h
 * @section		Common
 * @class		CHiResTimer
 * @brief		high resolution timer Use chrono
 * @author		bqrmtao@gmail.com
 * @date		2016/11/04 22:09:16
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***********************************************/

#ifndef __HIGH_RESOLUTION_TIMER_H__
#define __HIGH_RESOLUTION_TIMER_H__

#include <chrono>

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

/*
 * @class	CHiResTimer
 * @brief	high resolution timer Use chrono
*/
class _declspec(dllexport) CHiResTimer
{
public:
	/*
	 * @brief	default constructor, use millisecond
	*/
	CHiResTimer(){ m_nClockResolution = MilliSecond; }

	/*
	 * @brief	constructor
	 * @param	users' specified resolution.
	*/
	CHiResTimer(ClockResolution nClockReso)
	{
		m_nClockResolution = nClockReso;
		if (m_nClockResolution < NanoSecond || m_nClockResolution > Hour)
		{
			m_nClockResolution = MilliSecond;
		}
	}

	/*
	 * @brief	destructor
	*/
	~CHiResTimer(){}
	
	/*
	 * @brief	start the timer
	 * @param	none
	 * @return	
	*/
	void Start()
	{
		m_nTotalEalpsed = 0;
		m_oClockStart = std::chrono::system_clock::now();
	}
	
	/*
	 * @brief	stop the timer, calculate the time span.
	 * @param	none
	 * @return	time span in uint64_t.
	*/
	uint64_t Stop()
	{
		m_oClockStop = std::chrono::system_clock::now();
		switch (m_nClockResolution)
		{
		case NanoSecond:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::nanoseconds>(m_oClockStop - m_oClockStart).count();
			break;
		case MicroSecond:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::microseconds>(m_oClockStop - m_oClockStart).count();
			break;
		case MilliSecond:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_oClockStop - m_oClockStart).count();
			break;
		case Second:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::seconds>(m_oClockStop - m_oClockStart).count();
			break;
		case Minute:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::minutes>(m_oClockStop - m_oClockStart).count();
			break;
		case Hour:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::hours>(m_oClockStop - m_oClockStart).count();
			break;
		default:
			m_nCurrentEalpsed = 0;
			break;
		}
		m_nTotalEalpsed += m_nCurrentEalpsed;
		return m_nCurrentEalpsed;
	}
	
	/*
	 * @brief	pause the timer, calculate the time span, add current time span to total calculator, and resume the timer again.
	 * @param	none
	 * @return	current time span in uint64_t.
	*/
	uint64_t StopStart()
	{
		m_oClockStop = std::chrono::system_clock::now();
		switch (m_nClockResolution)
		{
		case NanoSecond:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::nanoseconds>(m_oClockStop - m_oClockStart).count();
			break;
		case MicroSecond:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::microseconds>(m_oClockStop - m_oClockStart).count();
			break;
		case MilliSecond:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::milliseconds>(m_oClockStop - m_oClockStart).count();
			break;
		case Second:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::seconds>(m_oClockStop - m_oClockStart).count();
			break;
		case Minute:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::minutes>(m_oClockStop - m_oClockStart).count();
			break;
		case Hour:
			m_nCurrentEalpsed = std::chrono::duration_cast<std::chrono::hours>(m_oClockStop - m_oClockStart).count();
			break;
		default:
			m_nCurrentEalpsed = 0;
			break;
		}
		
		m_nTotalEalpsed += m_nCurrentEalpsed;
		m_oClockStart = std::chrono::system_clock::now();
		
		return m_nCurrentEalpsed;
	}
	
	/*
	 * @brief	return total time span.
	 * @param	none
	 * @return	total time span in uint64_t.
	*/
	uint64_t TotalElapsed() { return m_nTotalEalpsed; }

private:
	int m_nClockResolution;

	uint64_t m_nTotalEalpsed;
	uint64_t m_nCurrentEalpsed;

	std::chrono::system_clock::time_point m_oClockStart;
	std::chrono::system_clock::time_point m_oClockStop;
};

#endif