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

#include "DicomRead.h"
#include "ErrorMsg.h"
#include "Logger.h"
#include "ReadConfig.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

#define IMG_BUF_LEN	512 * 512 * 2

int main(int argc, char** argv)
{
	// load property of log4cxx
	log4cxx::PropertyConfigurator::configure("logcfg.properties");
	
	__LOG_FUNC_START__;

	// example of logging an error
	__LOG_DEBUG__(CErrorMsg::GetInstance()->GetMsgString(OK));

	DicomInfo oDcmInfo;
	char czImgData[IMG_BUF_LEN];

	CDicomRead oReadDicom;
	int nProcResult = oReadDicom.GetInfoAndData("../CR-MONO1-10-chest", &oDcmInfo, czImgData, IMG_BUF_LEN);

	if (OK == nProcResult)
	{
		cv::Mat oImgMat(oDcmInfo.usImageHeight, oDcmInfo.usImageWidth, CV_16U);
		memcpy(oImgMat.data, czImgData, oDcmInfo.usImageHeight * oDcmInfo.usImageWidth * oDcmInfo.usPixelDepth / 8);

		int nNumHistSize = 65536;
		int nChannels = 0;
		float czHistRange[2] = {0, 65535};
		const float* pRanges[1] = {czHistRange};

		cv::MatND oHistMat;

		// calculate histogram
		cv::calcHist(&oImgMat,1, &nChannels, cv::Mat(), oHistMat, 1, &nNumHistSize, pRanges);

		int nMinPixelVal = 0;
		int nMaxPixelVal = 65535;
		int nTotalPixels = oImgMat.rows * oImgMat.cols;

		// find minimum pixel value
		for (int nIdx = 0; nIdx < nNumHistSize; nIdx++)
		{
			if (oHistMat.at<float>(nIdx) > 0)
			{
				nMinPixelVal = nIdx;
				break;
			}
		}

		// find maximum pixel value
		for (int nIdx = nNumHistSize - 1; nIdx >= 0; nIdx--)
		{
			if (oHistMat.at<float>(nIdx) > 0)
			{
				nMaxPixelVal = nIdx;
				break;
			}
		}

		// map pixel values to 0-65535
		cv::Mat oShownMat;
		oShownMat = (oImgMat - nMinPixelVal) * (65535.0 / (nMaxPixelVal - nMinPixelVal));

		cv::namedWindow("eq image", CV_WINDOW_NORMAL);
		cv::imshow("eq image", oShownMat);
		cv::waitKey(0);

		cv::destroyAllWindows();

		oShownMat.release();
		oImgMat.release();
	}

	__LOG_FUNC_END__;
}
