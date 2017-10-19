/**********************************************
 * @file		Logger.h
 * @section		Common
 * @class		N/A
 * @brief		macro definition of log4cxx
 * @author		bqrmtao@gmail.com
 * @date		2017/05/16 22:02:09
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
 ***********************************************/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

static log4cxx::LoggerPtr g_rootLogger = log4cxx::Logger::getRootLogger();
static log4cxx::LoggerPtr g_errLogger = log4cxx::Logger::getLogger("error");

#define __LOG_FUNC_START__	LOG4CXX_INFO(g_rootLogger, std::string(string(">> [") + std::string(__FUNCTION__) + std::string("]")).c_str())
#define __LOG_FUNC_END__	LOG4CXX_INFO(g_rootLogger, std::string(string("<< [") + std::string(__FUNCTION__) + std::string("]")).c_str())

// Level.ALL < Level.DEBUG < Level.INFO < Level.WARN < Level.ERROR < Level.FATAL < Level.OFF
#define __LOG_DEBUG__(x)	LOG4CXX_DEBUG(g_rootLogger, std::string(string("   [") + std::string(__FUNCTION__) + std::string("]: ") + std::string(x)).c_str())
#define __LOG_INFO__(x)		LOG4CXX_INFO(g_rootLogger, std::string(string("   [") + std::string(__FUNCTION__) + std::string("]: ") + std::string(x)).c_str())
#define __LOG_WARN__(x)		LOG4CXX_WARN(g_rootLogger, std::string(string("   [") + std::string(__FUNCTION__) + std::string("]: ") + std::string(x)).c_str())
#define __LOG_ERROR__(x)	LOG4CXX_ERROR(g_errLogger, std::string(string("** [") + std::string(__FUNCTION__) + std::string("]: ") + std::string(x)).c_str())

#endif // __LOGG_H__
