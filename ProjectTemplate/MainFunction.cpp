/***************************************************
 * @file		MainFunction.cpp
 * @section		ProjectTemplate
 * @class		N/A
 * @brief		explanation of 
 * @author		bqrmtao@gmail.com
 * @date		2017/10/14
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include "ErrorMsg.h"
#include "Logger.h"
#include "ReadConfig.h"

using namespace std;

int main(int argc, char** argv)
{
	// load property of log4cxx
	log4cxx::PropertyConfigurator::configure("logcfg.properties");
	
	__LOG_FUNC_START__;

	// example of logging an error
	__LOG_DEBUG__(CErrorMsg::GetInstance()->GetMsgString(NO_ERROR));

	__LOG_FUNC_END__;
}
