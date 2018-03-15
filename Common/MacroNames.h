/**********************************************
 * @file		MacroNames.h
 * @section		Common
 * @class		N/A
 * @brief		define error id
 * @author		bqrmtao@gmail.com
 * @date		2017/05/02 13:30:53
 * @version		2016/11/04 20.48
 * @copyright	bqrmtao@gmail.com
 ***********************************************
 * Error id 123456
 *	1 represent error level
 *	2 and 3 represent project id
 *	4, 5 and 6 represent error id
 ***********************************************
 * Error level
 *	1: Warning
 *	2: Error
 *	3: Fatal  
***********************************************/

#ifndef __MACRO_NAMES_H__
#define __MACRO_NAMES_H__

#define OK	0

// [General]
#define INVALID_FILE_NAME		200001

#define OPEN_FILE_ERR			200002
#define READ_FILE_ERR			200003
#define EMPTY_SAMPLE_SET		200004
#define NUM_SAMPLE_UNMATCH		200005
#define COLLUMN_OF_LABEL		200006

// [Common]
#define BUFF_ALLOCATED_SHORT	201001

// [LogisticRegression]

// [ExpectationMaximization]

#endif
