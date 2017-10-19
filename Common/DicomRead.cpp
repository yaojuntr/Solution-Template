/***************************************************
 * @file		DicomRead.cpp
 * @section		Common
 * @class		N/A
 * @brief		read dicom image
 * @author		bqrmtao@gmail.com
 * @date		2017/10/14
 * @version		1.0
 * @copyright	bqrmtao@gmail.com
***************************************************/

#include <filesystem>

#include "DicomRead.h"
#include "ErrorMsg.h"
#include "Exception.h"
#include "Logger.h"

#define ID_OFFSET	128
#define IMPLICIT_VR	0x2D2D

using namespace std;
namespace sf = std::tr2::sys;

const int AE = 0x4145;
const int AS = 0x4153;
const int AT = 0x4154;
const int CS = 0x4353;
const int DA = 0x4441;
const int DS = 0x4453;
const int DT = 0x4454;
const int FD = 0x4644;
const int FL = 0x464C;
const int IS = 0x4953;
const int LO = 0x4C4F;
const int LT = 0x4C54;
const int PN = 0x504E;
const int SH = 0x5348;
const int SL = 0x534C;
const int SS = 0x5353;
const int ST = 0x5354;
const int TM = 0x544D;
const int UI = 0x5549;
const int UL = 0x554C;
const int US = 0x5553;
const int UT = 0x5554;
const int OB = 0x4F42;
const int OW = 0x4F57;
const int SQ = 0x5351;
const int UN = 0x554E;
const int QQ = 0x3F3F;
const int RT = 0x5254;

const unsigned int PIXEL_REPRESENTATION       = 0x00280103;	   
const unsigned int TRANSFER_SYNTAX_UID        = 0x00020010;	   
const unsigned int MODALITY                   = 0x00080060;	   
const unsigned int SLICE_THICKNESS            = 0x00180050;	   
const unsigned int SLICE_SPACING              = 0x00180088;	   
const unsigned int SAMPLES_PER_PIXEL          = 0x00280002;	   
const unsigned int PHOTOMETRIC_INTERPRETATION = 0x00280004;	   
const unsigned int PLANAR_CONFIGURATION       = 0x00280006;	   
const unsigned int NUMBER_OF_FRAMES           = 0x00280008;	   
const unsigned int ROWS                       = 0x00280010;	   
const unsigned int COLUMNS                    = 0x00280011;	   
const unsigned int PIXEL_SPACING              = 0x00280030;	   
const unsigned int BITS_ALLOCATED             = 0x00280100;	   
const unsigned int WINDOW_CENTER              = 0x00281050;	   
const unsigned int WINDOW_WIDTH               = 0x00281051;	   
const unsigned int RESCALE_INTERCEPT          = 0x00281052;	   
const unsigned int RESCALE_SLOPE              = 0x00281053;	   
const unsigned int RED_PALETTE                = 0x00281201;	   
const unsigned int GREEN_PALETTE              = 0x00281202;	   
const unsigned int BLUE_PALETTE               = 0x00281203;	   
const unsigned int ICON_IMAGE_SEQUENCE        = 0x00880200;	   
const unsigned int PIXEL_DATA                 = 0x7FE00010;

const string ITEM                     = "FFFEE000";
const string ITEM_DELIMITATION        = "FFFEE00D";
const string SEQUENCE_DELIMITATION    = "FFFEE0DD";

/*
 * @brief	default constructor
*/
CDicomRead::CDicomRead()
{
	InitDictionary();
}

/*
 * @brief	default destructor
*/
CDicomRead::~CDicomRead()
{
}

/*
 * @brief	get image information and data from a dicom file
 * @param	strFileName
 * @param	pDcmInfo
 * @param	pDataBuf
 * @param	unBuffLen
 * @return	error code
*/
int CDicomRead::GetInfoAndData(std::string strFileName, DicomInfo *pDcmInfo, char *pDataBuf, size_t unBuffLen)
{
	__LOG_FUNC_START__;

	m_strFileName = strFileName;
	m_pDataPtr = pDataBuf;

	InitData();

	m_nProcResult = ReadDicom(unBuffLen);
	if (OK != m_nProcResult)
	{
		::memset(pDcmInfo, 0, sizeof(DicomInfo));
		return READ_FILE_ERR;
	}
	else
	{
		::memcpy(pDcmInfo, &m_oDcmInfo, sizeof(DicomInfo));
	}

	__LOG_FUNC_END__;

	return m_nProcResult;
}

/*
 * @brief	add a tag to dicom information
 * @param	strTag
*/
void CDicomRead::AddTag(std::string strTagInfo)
{
	string strHeaderInfo = GetHeaderInfo(strTagInfo);

	string strTagVal = Int2Str(m_unTagVal, 16, 8);

	if (m_isInSequence && strHeaderInfo != "" && m_nVR != SQ)
	{
		strHeaderInfo = ">" + strHeaderInfo;
	}
	if ("" != strHeaderInfo && strTagVal != ITEM)
	{

	}
}

/*
 * @brief	convert string to number
*/
template<typename T>
T CDicomRead::ConvertStr2Num(size_t unStrLen)
{
	ReverseCopy(unStrLen);

	T tVal = 0;
	for (int nIdx = 0; nIdx < unStrLen; nIdx++)
	{
		tVal |= m_czStreamBuff[nIdx];
		tVal << 8;
	}

	return tVal;
}

/*
 * @brief	read next tag' length
*/
void CDicomRead::GetElementLen()
{
	ReadBuf(4);

	m_nVR = m_czStreamBuff[0] << 8 | m_czStreamBuff[1];

	// Cannot know whether the VR is implicit or explicit without the complete Dicom Data Dictionary
	switch (m_nVR)
	{
	case OB:
	case OW:
	case SQ:
	case UN:
	case UT:
		// Explicit VR with 32-bit length if other two bytes are zero
		if (0 == (unsigned char)m_czStreamBuff[2] || 0 == (unsigned char)m_czStreamBuff[3])
		{
			ReadBuf(4);
			ReverseCopy(4);
			m_unElementLen = (unsigned int)m_czStreamBuff;
			return;
		}
		m_nVR = IMPLICIT_VR;
		ReverseCopy(4);
		m_unElementLen = (unsigned int)m_czStreamBuff;
		return;
	case AE:
	case AS:
	case AT:
	case CS:
	case DA:
	case DS:
	case DT:
	case FD:
	case FL:
	case IS:
	case LO:
	case LT:
	case PN:
	case SH:
	case SL:
	case SS:
	case ST:
	case TM:
	case UI:
	case UL:
	case US:
	case QQ:
	case RT:
		// Explicit vr with 16-bit length
		::memcpy(m_czStreamBuff, m_czStreamBuff + 2, 2);
		ReverseCopy(2);
		m_unElementLen = (unsigned short)m_czStreamBuff;
		return;
	default:
		m_nVR = IMPLICIT_VR;
		m_unElementLen = ConvertStr2Num<unsigned int>(4);
		return;
	}
}

/*
 * @brief	read header according to different tag
 * @return	byte(s) in header's string buffer
*/
std::string CDicomRead::GetHeaderInfo(std::string strTagInfo)
{
	string strHex = Int2Str<unsigned int>(m_unTagVal, 16, 8);
	if (strHex == ITEM_DELIMITATION || strHex == SEQUENCE_DELIMITATION)
	{
		m_isInSequence = false;
		return "";
	}

	string strID = "";
	if (m_mapDicomDictionary.find(strHex) != m_mapDicomDictionary.end())
	{
		strID = m_mapDicomDictionary[strHex];
		if (m_nVR == IMPLICIT_VR)
		{
			m_nVR = strID[0] << 8 | strID[1];
		}
		strID = strID.substr(2);
	}

	if (ITEM == strHex)
	{
		return strID == "" ? "" : strID;
	}

	if ("" != strTagInfo)
	{
		return strID + ": " + strTagInfo;
	}

	switch (m_nVR)
	{
	case FD:
	case FL:
		ReadBuf(m_unElementLen);
		break;
	case AE:
	case AS:
	case AT:
	case CS:
	case DA:
	case DS:
	case DT:
	case IS:
	case LO:
	case LT:
	case PN:
	case SH:
	case ST:
	case TM:
	case UI:
		ReadBuf(m_unElementLen);
		strTagInfo = std::string(m_czStreamBuff, m_unElementLen);
		break;
	case US:
		if (2 == m_unElementLen)
		{
			ReadBuf(2);
			ReverseCopy(2);
			strTagInfo = Int2Str((unsigned short)m_czStreamBuff, 10, 2);
		}
		else
		{
			for (int nIdx = 0; nIdx < m_unElementLen / 2; nIdx++)
			{
				strTagInfo = "";
				ReadBuf(2);
				ReverseCopy(2);
				strTagInfo += Int2Str((unsigned short)m_czStreamBuff, 10, 2);
			}
		}
		break;
	case IMPLICIT_VR:
		ReadBuf(m_unElementLen);
		if (m_unElementLen > 44)
		{
			strTagInfo = "";
		}
		else
		{
			strTagInfo = std::string(m_czStreamBuff, m_unElementLen);
		}
		break;
	case SQ:
		strTagInfo = "";
		if (m_unTagVal == ICON_IMAGE_SEQUENCE || ((m_unTagVal >> 16) & 1) != 0)
		{
			m_unStreamLocation += m_unElementLen;
		}
		break;
	default:
		strTagInfo = "";
		m_unStreamLocation += m_unElementLen;
		break;
	}

	if ("" != strTagInfo && "" == strID)
	{
		return "Private Tag: " + strTagInfo;
	}
	else if ("" == strID)
	{
		return "";
	}
	else
	{
		return strID + ": " + strTagInfo;
	}
}

/*
 * @brief	read next tag
*/
void CDicomRead::GetNextTag()
{
	// read first 2 bytes, GroupWord
	m_unGroupWord = GetStreamValue<unsigned short>(2);
	if (0x0800 == m_unGroupWord && m_isBigEndianSyntax)
	{
		m_oDcmInfo.isBigEndian = true;
		m_unGroupWord = 0x0008;
	}

	// read second 2 bytes, ElementWord
	m_unElementWord = GetStreamValue<unsigned short>(2);

	// combine GroupWord with ElementWord as a Tag
	m_unTagVal = m_unGroupWord << 16 | m_unElementWord;

	// read length of bytes represent current tag
	GetElementLen();

	// GE files
	if (m_unElementLen == 13 && !m_isOddIdx)
	{
		m_unElementLen = 10;
	}
	
	// "Undefined" element length.
	// This is a sort of bracket that encloses a sequence of elements.
	if (m_unElementLen == -1)
	{
		m_unElementLen = 0;
		m_isInSequence = true;
	}
}

/*
 * @brief	read bytes from stream and convert to a number
*/
template<typename T>
T CDicomRead::GetStreamValue(size_t unStrLen)
{
	ReadBuf(unStrLen);
	ReverseCopy(unStrLen);

	T tVal = 0;
	for (int nIdx = 0; nIdx < unStrLen - 1; nIdx++)
	{
		tVal += (unsigned char)m_czStreamBuff[nIdx];
		tVal <<= 8;
	}

	tVal += (unsigned char)m_czStreamBuff[unStrLen - 1];

	return tVal;
}

/*
 * @brief	initialize data in-class
*/
void CDicomRead::InitData()
{
	__LOG_FUNC_START__;

	m_unStreamLocation = 0;
	m_isDcmTagFound = false;
	m_isBigEndianSyntax = false;
	m_isOddIdx = false;
	m_isInSequence = false;

	memset(&m_oDcmInfo, 0, sizeof(DicomInfo));
	
	memset(m_czStreamBuff, 0, STR_BUF_LEN);
	memset(m_czHeaderBuf, 0, STR_BUF_LEN);
	memset(m_czTagBuff, 0, STR_BUF_LEN);
	memset(m_czHexBuff, 0, STR_BUF_LEN);

	m_strTag.resize(STR_BUF_LEN);

	// default using little endian
	m_oDcmInfo.isBigEndian = false;
	m_oDcmInfo.usSamplesPerPixel = 1;
	m_oDcmInfo.fRescaleIntercept = 0;
	m_oDcmInfo.fRescaleSlope = 1.0;

	__LOG_FUNC_END__;
}

/*
 * @brief	initialize Dicom 3.0 dictionary
*/
void CDicomRead::InitDictionary()
{
	m_mapDicomDictionary.insert(make_pair("00020002", "UIMedia Storage SOP Class UID"));
	m_mapDicomDictionary.insert(make_pair("00020003", "UIMediaStorageSOPInstanceUID"));
	m_mapDicomDictionary.insert(make_pair("00020010", "UITransferSyntaxUID"));
	m_mapDicomDictionary.insert(make_pair("00020012", "UIImplementationClassUID"));
	m_mapDicomDictionary.insert(make_pair("00020013", "SHImplementationVersionName"));
	m_mapDicomDictionary.insert(make_pair("00020016", "AESourceApplicationEntityTitle"));
	m_mapDicomDictionary.insert(make_pair("00080005", "CSSpecificCharacterSet"));
	m_mapDicomDictionary.insert(make_pair("00080008", "CSImageType"));
	m_mapDicomDictionary.insert(make_pair("00080010", "CSRecognitionCode"));
	m_mapDicomDictionary.insert(make_pair("00080012", "DAInstanceCreationDate"));
	m_mapDicomDictionary.insert(make_pair("00080013", "TMInstanceCreationTime"));
	m_mapDicomDictionary.insert(make_pair("00080014", "UIInstanceCreatorUID"));
	m_mapDicomDictionary.insert(make_pair("00080016", "UISOPClassUID"));
	m_mapDicomDictionary.insert(make_pair("00080018", "UISOPInstanceUID"));
	m_mapDicomDictionary.insert(make_pair("00080020", "DAStudyDate"));
	m_mapDicomDictionary.insert(make_pair("00080021", "DASeriesDate"));
	m_mapDicomDictionary.insert(make_pair("00080022", "DAAcquisitionDate"));
	m_mapDicomDictionary.insert(make_pair("00080023", "DAContentDate"));
	m_mapDicomDictionary.insert(make_pair("00080024", "DAOverlayDate"));
	m_mapDicomDictionary.insert(make_pair("00080025", "DACurveDate"));
	m_mapDicomDictionary.insert(make_pair("00080030", "TMStudyTime"));
	m_mapDicomDictionary.insert(make_pair("00080031", "TMSeriesTime"));
	m_mapDicomDictionary.insert(make_pair("00080032", "TMAcquisitionTime"));
	m_mapDicomDictionary.insert(make_pair("00080033", "TMContentTime"));
	m_mapDicomDictionary.insert(make_pair("00080034", "TMOverlayTime"));
	m_mapDicomDictionary.insert(make_pair("00080035", "TMCurveTime"));
	m_mapDicomDictionary.insert(make_pair("00080040", "USDataSetType"));
	m_mapDicomDictionary.insert(make_pair("00080041", "LODataSetSubtype"));
	m_mapDicomDictionary.insert(make_pair("00080042", "CSNuclearMedicineSeriesType"));
	m_mapDicomDictionary.insert(make_pair("00080050", "SHAccessionNumber"));
	m_mapDicomDictionary.insert(make_pair("00080052", "CSQuery/RetrieveLevel"));
	m_mapDicomDictionary.insert(make_pair("00080054", "AERetrieveAETitle"));
	m_mapDicomDictionary.insert(make_pair("00080058", "AEFailedSOPInstanceUIDList"));
	m_mapDicomDictionary.insert(make_pair("00080060", "CSModality"));
	m_mapDicomDictionary.insert(make_pair("00080064", "CSConversionType"));
	m_mapDicomDictionary.insert(make_pair("00080068", "CSPresentationIntentType"));
	m_mapDicomDictionary.insert(make_pair("00080070", "LOManufacturer"));
	m_mapDicomDictionary.insert(make_pair("00080080", "LOInstitutionName"));
	m_mapDicomDictionary.insert(make_pair("00080081", "STInstitutionAddress"));
	m_mapDicomDictionary.insert(make_pair("00080082", "SQInstitutionCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00080090", "PNReferringPhysician'sName"));
	m_mapDicomDictionary.insert(make_pair("00080092", "STReferringPhysician'sAddress"));
	m_mapDicomDictionary.insert(make_pair("00080094", "SHReferringPhysician'sTelephoneNumbers"));
	m_mapDicomDictionary.insert(make_pair("00080096", "SQReferringPhysicianIdentificationSequence"));
	m_mapDicomDictionary.insert(make_pair("00080100", "SHCodeValue"));
	m_mapDicomDictionary.insert(make_pair("00080102", "SHCodingSchemeDesignator"));
	m_mapDicomDictionary.insert(make_pair("00080103", "SHCodingSchemeVersion"));
	m_mapDicomDictionary.insert(make_pair("00080104", "LOCodeMeaning"));
	m_mapDicomDictionary.insert(make_pair("00080201", "SHTimezoneOffsetFromUTC"));
	m_mapDicomDictionary.insert(make_pair("00081010", "SHStationName"));
	m_mapDicomDictionary.insert(make_pair("00081030", "LOStudyDescription"));
	m_mapDicomDictionary.insert(make_pair("00081032", "SQProcedureCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("0008103E", "LOSeriesDescription"));
	m_mapDicomDictionary.insert(make_pair("00081040", "LOInstitutionalDepartmentName"));
	m_mapDicomDictionary.insert(make_pair("00081048", "PNPhysician(s)ofRecord"));
	m_mapDicomDictionary.insert(make_pair("00081050", "PNPerformingPhysician'sName"));
	m_mapDicomDictionary.insert(make_pair("00081060", "PNNameofPhysician(s)ReadingStudy"));
	m_mapDicomDictionary.insert(make_pair("00081070", "PNOperator'sName"));
	m_mapDicomDictionary.insert(make_pair("00081080", "LOAdmittingDiagnosesDescription"));
	m_mapDicomDictionary.insert(make_pair("00081084", "SQAdmittingDiagnosesCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00081090", "LOManufacturer'sModelName"));
	m_mapDicomDictionary.insert(make_pair("00081100", "SQReferencedResultsSequence"));
	m_mapDicomDictionary.insert(make_pair("00081110", "SQReferencedStudySequence"));
	m_mapDicomDictionary.insert(make_pair("00081111", "SQReferencedPerformedProcedureStepSequence"));
	m_mapDicomDictionary.insert(make_pair("00081115", "SQReferencedSeriesSequence"));
	m_mapDicomDictionary.insert(make_pair("00081120", "SQReferencedPatientSequence"));
	m_mapDicomDictionary.insert(make_pair("00081125", "SQReferencedVisitSequence"));
	m_mapDicomDictionary.insert(make_pair("00081130", "SQReferencedOverlaySequence"));
	m_mapDicomDictionary.insert(make_pair("00081140", "SQReferencedImageSequence"));
	m_mapDicomDictionary.insert(make_pair("00081145", "SQReferencedCurveSequence"));
	m_mapDicomDictionary.insert(make_pair("00081150", "UIReferencedSOPClassUID"));
	m_mapDicomDictionary.insert(make_pair("00081155", "UIReferencedSOPInstanceUID"));
	m_mapDicomDictionary.insert(make_pair("00082111", "STDerivationDescription"));
	m_mapDicomDictionary.insert(make_pair("00082112", "SQSourceImageSequence"));
	m_mapDicomDictionary.insert(make_pair("00082120", "SHStageName"));
	m_mapDicomDictionary.insert(make_pair("00082122", "ISStageNumber"));
	m_mapDicomDictionary.insert(make_pair("00082124", "ISNumberofStages"));
	m_mapDicomDictionary.insert(make_pair("00082129", "ISNumberofEventTimers"));
	m_mapDicomDictionary.insert(make_pair("00082128", "ISViewNumber"));
	m_mapDicomDictionary.insert(make_pair("0008212A", "ISNumberofViewsinStage"));
	m_mapDicomDictionary.insert(make_pair("00082130", "DSEventElapsedTime(s)"));
	m_mapDicomDictionary.insert(make_pair("00082132", "LOEventTimerName(s)"));
	m_mapDicomDictionary.insert(make_pair("00082142", "ISStartTrim"));
	m_mapDicomDictionary.insert(make_pair("00082143", "ISStopTrim"));
	m_mapDicomDictionary.insert(make_pair("00082144", "ISRecommendedDisplayFrameRate"));
	m_mapDicomDictionary.insert(make_pair("00082200", "CSTransducerPosition"));
	m_mapDicomDictionary.insert(make_pair("00082204", "CSTransducerOrientation"));
	m_mapDicomDictionary.insert(make_pair("00082208", "CSAnatomicStructure"));
	m_mapDicomDictionary.insert(make_pair("00100010", "PNPatient'sName"));
	m_mapDicomDictionary.insert(make_pair("00100020", "LOPatientID"));
	m_mapDicomDictionary.insert(make_pair("00100021", "LOIssuerofPatientID"));
	m_mapDicomDictionary.insert(make_pair("00100022", "CSTypeofPatientID"));
	m_mapDicomDictionary.insert(make_pair("00100030", "DAPatient'sBirthDate"));
	m_mapDicomDictionary.insert(make_pair("00100032", "TMPatient'sBirthTime"));
	m_mapDicomDictionary.insert(make_pair("00100040", "CSPatient'sSex"));
	m_mapDicomDictionary.insert(make_pair("00100050", "SQPatient'sInsurancePlanCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00100101", "SQPatient'sPrimaryLanguageCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00100102", "SQPatient'sPrimaryLanguageModifierCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00101000", "LOOtherPatientIDs"));
	m_mapDicomDictionary.insert(make_pair("00101001", "PNOtherPatientNames"));
	m_mapDicomDictionary.insert(make_pair("00101005", "PNPatient'sBirthName"));
	m_mapDicomDictionary.insert(make_pair("00101010", "ASPatient'sAge"));
	m_mapDicomDictionary.insert(make_pair("00101020", "DSPatient'sSize"));
	m_mapDicomDictionary.insert(make_pair("00101030", "DSPatient'sWeight"));
	m_mapDicomDictionary.insert(make_pair("00101040", "LOPatient'sAddress"));
	m_mapDicomDictionary.insert(make_pair("00101050", "LOInsurancePlanIdentification"));
	m_mapDicomDictionary.insert(make_pair("00102000", "LOMedicalAlerts"));
	m_mapDicomDictionary.insert(make_pair("00102110", "LOAllergies"));
	m_mapDicomDictionary.insert(make_pair("00102150", "LOCountryofResidence"));
	m_mapDicomDictionary.insert(make_pair("00102152", "LORegionofResidence"));
	m_mapDicomDictionary.insert(make_pair("00102154", "SHPatient'sTelephoneNumbers"));
	m_mapDicomDictionary.insert(make_pair("00102160", "SHEthnicGroup"));
	m_mapDicomDictionary.insert(make_pair("00102180", "SHOccupation"));
	m_mapDicomDictionary.insert(make_pair("001021A0", "CSSmokingStatus"));
	m_mapDicomDictionary.insert(make_pair("001021B0", "LTAdditionalPatientHistory"));
	m_mapDicomDictionary.insert(make_pair("00102201", "LOPatientSpeciesDescription"));
	m_mapDicomDictionary.insert(make_pair("00102203", "CSPatientSexNeutered"));
	m_mapDicomDictionary.insert(make_pair("00102292", "LOPatientBreedDescription"));
	m_mapDicomDictionary.insert(make_pair("00102297", "PNResponsiblePerson"));
	m_mapDicomDictionary.insert(make_pair("00102298", "CSResponsiblePersonRole"));
	m_mapDicomDictionary.insert(make_pair("00102299", "CSResponsibleOrganization"));
	m_mapDicomDictionary.insert(make_pair("00104000", "LTPatientComments"));
	m_mapDicomDictionary.insert(make_pair("00180010", "LOContrast/BolusAgent"));
	m_mapDicomDictionary.insert(make_pair("00180015", "CSBodyPartExamined"));
	m_mapDicomDictionary.insert(make_pair("00180020", "CSScanningSequence"));
	m_mapDicomDictionary.insert(make_pair("00180021", "CSSequenceVariant"));
	m_mapDicomDictionary.insert(make_pair("00180022", "CSScanOptions"));
	m_mapDicomDictionary.insert(make_pair("00180023", "CSMRAcquisitionType"));
	m_mapDicomDictionary.insert(make_pair("00180024", "SHSequenceName"));
	m_mapDicomDictionary.insert(make_pair("00180025", "CSAngioFlag"));
	m_mapDicomDictionary.insert(make_pair("00180030", "LORadionuclide"));
	m_mapDicomDictionary.insert(make_pair("00180031", "LORadiopharmaceutical"));
	m_mapDicomDictionary.insert(make_pair("00180032", "DSEnergyWindowCenterline"));
	m_mapDicomDictionary.insert(make_pair("00180033", "DSEnergyWindowTotalWidth"));
	m_mapDicomDictionary.insert(make_pair("00180034", "LOInterventionDrugName"));
	m_mapDicomDictionary.insert(make_pair("00180035", "TMInterventionDrugStartTime"));
	m_mapDicomDictionary.insert(make_pair("00180040", "ISCineRate"));
	m_mapDicomDictionary.insert(make_pair("00180050", "DSSliceThickness"));
	m_mapDicomDictionary.insert(make_pair("00180060", "DSKVP"));
	m_mapDicomDictionary.insert(make_pair("00180070", "ISCountsAccumulated"));
	m_mapDicomDictionary.insert(make_pair("00180071", "CSAcquisitionTerminationCondition"));
	m_mapDicomDictionary.insert(make_pair("00180072", "DSEffectiveDuration"));
	m_mapDicomDictionary.insert(make_pair("00180073", "CSAcquisitionStartCondition"));
	m_mapDicomDictionary.insert(make_pair("00180074", "ISAcquisitionStartConditionData"));
	m_mapDicomDictionary.insert(make_pair("00180075", "ISAcquisitionTerminationConditionData"));
	m_mapDicomDictionary.insert(make_pair("00180080", "DSRepetitionTime"));
	m_mapDicomDictionary.insert(make_pair("00180081", "DSEchoTime"));
	m_mapDicomDictionary.insert(make_pair("00180082", "DSInversionTime"));
	m_mapDicomDictionary.insert(make_pair("00180083", "DSNumberofAverages"));
	m_mapDicomDictionary.insert(make_pair("00180084", "DSImagingFrequency"));
	m_mapDicomDictionary.insert(make_pair("00180085", "SHImagedNucleus"));
	m_mapDicomDictionary.insert(make_pair("00180086", "ISEchoNumbers(s)"));
	m_mapDicomDictionary.insert(make_pair("00180087", "DSMagneticFieldStrength"));
	m_mapDicomDictionary.insert(make_pair("00180088", "DSSpacingBetweenSlices"));
	m_mapDicomDictionary.insert(make_pair("00180089", "ISNumberofPhaseEncodingSteps"));
	m_mapDicomDictionary.insert(make_pair("00180090", "DSDataCollectionDiameter"));
	m_mapDicomDictionary.insert(make_pair("00180091", "ISEchoTrainLength"));
	m_mapDicomDictionary.insert(make_pair("00180093", "DSPercentSampling"));
	m_mapDicomDictionary.insert(make_pair("00180094", "DSPercentPhaseFieldofView"));
	m_mapDicomDictionary.insert(make_pair("00180095", "DSPixelBandwidth"));
	m_mapDicomDictionary.insert(make_pair("00181000", "LODeviceSerialNumber"));
	m_mapDicomDictionary.insert(make_pair("00181004", "LOPlateID"));
	m_mapDicomDictionary.insert(make_pair("00181010", "LOSecondaryCaptureDeviceID"));
	m_mapDicomDictionary.insert(make_pair("00181012", "DADateofSecondaryCapture"));
	m_mapDicomDictionary.insert(make_pair("00181014", "TMTimeofSecondaryCapture"));
	m_mapDicomDictionary.insert(make_pair("00181016", "LOSecondaryCaptureDeviceManufacturer"));
	m_mapDicomDictionary.insert(make_pair("00181018", "LOSecondaryCaptureDeviceManufacturer'sModelName"));
	m_mapDicomDictionary.insert(make_pair("00181019", "LOSecondaryCaptureDeviceSoftwareVersions"));
	m_mapDicomDictionary.insert(make_pair("00181020", "LOSoftwareVersions(s)"));
	m_mapDicomDictionary.insert(make_pair("00181022", "SHVideoImageFormatAcquired"));
	m_mapDicomDictionary.insert(make_pair("00181023", "LODigitalImageFormatAcquired"));
	m_mapDicomDictionary.insert(make_pair("00181030", "LOProtocolName"));
	m_mapDicomDictionary.insert(make_pair("00181040", "LOContrast/BolusRoute"));
	m_mapDicomDictionary.insert(make_pair("00181041", "DSContrast/BolusVolume"));
	m_mapDicomDictionary.insert(make_pair("00181042", "TMContrast/BolusStartTime"));
	m_mapDicomDictionary.insert(make_pair("00181043", "TMContrast/BolusStopTime"));
	m_mapDicomDictionary.insert(make_pair("00181044", "DSContrast/BolusTotalDose"));
	m_mapDicomDictionary.insert(make_pair("00181045", "ISSyringeCounts"));
	m_mapDicomDictionary.insert(make_pair("00181050", "DSSpatialResolution"));
	m_mapDicomDictionary.insert(make_pair("00181060", "DSTriggerTime"));
	m_mapDicomDictionary.insert(make_pair("00181061", "LOTriggerSourceorType"));
	m_mapDicomDictionary.insert(make_pair("00181062", "ISNominalInterval"));
	m_mapDicomDictionary.insert(make_pair("00181063", "DSFrameTime"));
	m_mapDicomDictionary.insert(make_pair("00181064", "LOCardiacFramingType"));
	m_mapDicomDictionary.insert(make_pair("00181065", "DSFrameTimeVector"));
	m_mapDicomDictionary.insert(make_pair("00181066", "DSFrameDelay"));
	m_mapDicomDictionary.insert(make_pair("00181070", "LORadiopharmaceuticalRoute"));
	m_mapDicomDictionary.insert(make_pair("00181071", "DSRadiopharmaceuticalVolume"));
	m_mapDicomDictionary.insert(make_pair("00181072", "TMRadiopharmaceuticalStartTime"));
	m_mapDicomDictionary.insert(make_pair("00181073", "TMRadiopharmaceuticalStopTime"));
	m_mapDicomDictionary.insert(make_pair("00181074", "DSRadionuclideTotalDose"));
	m_mapDicomDictionary.insert(make_pair("00181075", "DSRadionuclideHalfLife"));
	m_mapDicomDictionary.insert(make_pair("00181076", "DSRadionuclidePositronFraction"));
	m_mapDicomDictionary.insert(make_pair("00181080", "CSBeatRejectionFlag"));
	m_mapDicomDictionary.insert(make_pair("00181081", "ISLowR-RValue"));
	m_mapDicomDictionary.insert(make_pair("00181082", "ISHighR-RValue"));
	m_mapDicomDictionary.insert(make_pair("00181083", "ISIntervalsAcquired"));
	m_mapDicomDictionary.insert(make_pair("00181084", "ISIntervalsRejected"));
	m_mapDicomDictionary.insert(make_pair("00181085", "LOPVCRejection"));
	m_mapDicomDictionary.insert(make_pair("00181086", "ISSkipBeats"));
	m_mapDicomDictionary.insert(make_pair("00181088", "ISHeartRate"));
	m_mapDicomDictionary.insert(make_pair("00181090", "ISCardiacNumberofImages"));
	m_mapDicomDictionary.insert(make_pair("00181094", "ISTriggerWindow"));
	m_mapDicomDictionary.insert(make_pair("00181100", "DSReconstructionDiameter"));
	m_mapDicomDictionary.insert(make_pair("00181110", "DSDistanceSourcetoDetector"));
	m_mapDicomDictionary.insert(make_pair("00181111", "DSDistanceSourcetoPatient"));
	m_mapDicomDictionary.insert(make_pair("00181120", "DSGantry/DetectorTilt"));
	m_mapDicomDictionary.insert(make_pair("00181130", "DSTableHeight"));
	m_mapDicomDictionary.insert(make_pair("00181131", "DSTableTraverse"));
	m_mapDicomDictionary.insert(make_pair("00181140", "CSRotationDirection"));
	m_mapDicomDictionary.insert(make_pair("00181141", "DSAngularPosition"));
	m_mapDicomDictionary.insert(make_pair("00181142", "DSRadialPosition"));
	m_mapDicomDictionary.insert(make_pair("00181143", "DSScanArc"));
	m_mapDicomDictionary.insert(make_pair("00181144", "DSAngularStep"));
	m_mapDicomDictionary.insert(make_pair("00181145", "DSCenterofRotationOffset"));
	m_mapDicomDictionary.insert(make_pair("00181146", "DSRotationOffset"));
	m_mapDicomDictionary.insert(make_pair("00181147", "CSFieldofViewShape"));
	m_mapDicomDictionary.insert(make_pair("00181149", "ISFieldofViewDimensions(s)"));
	m_mapDicomDictionary.insert(make_pair("00181150", "ISExposureTime"));
	m_mapDicomDictionary.insert(make_pair("00181151", "ISX-rayTubeCurrent"));
	m_mapDicomDictionary.insert(make_pair("00181152", "ISExposure"));
	m_mapDicomDictionary.insert(make_pair("00181153", "ISExposureinuAs"));
	m_mapDicomDictionary.insert(make_pair("00181154", "DSAveragePulseWidth"));
	m_mapDicomDictionary.insert(make_pair("00181155", "CSRadiationSetting"));
	m_mapDicomDictionary.insert(make_pair("00181156", "CSRectificationType"));
	m_mapDicomDictionary.insert(make_pair("0018115A", "CSRadiationMode"));
	m_mapDicomDictionary.insert(make_pair("0018115E", "DSImageandFluoroscopyAreaDoseProduct"));
	m_mapDicomDictionary.insert(make_pair("00181160", "SHFilterType"));
	m_mapDicomDictionary.insert(make_pair("00181161", "LOTypeofFilters"));
	m_mapDicomDictionary.insert(make_pair("00181162", "DSIntensifierSize"));
	m_mapDicomDictionary.insert(make_pair("00181164", "DSImagerPixelSpacing"));
	m_mapDicomDictionary.insert(make_pair("00181166", "CSGrid"));
	m_mapDicomDictionary.insert(make_pair("00181170", "ISGeneratorPower"));
	m_mapDicomDictionary.insert(make_pair("00181180", "SHCollimator/gridName"));
	m_mapDicomDictionary.insert(make_pair("00181181", "CSCollimatorType"));
	m_mapDicomDictionary.insert(make_pair("00181182", "ISFocalDistance"));
	m_mapDicomDictionary.insert(make_pair("00181183", "DSXFocusCenter"));
	m_mapDicomDictionary.insert(make_pair("00181184", "DSYFocusCenter"));
	m_mapDicomDictionary.insert(make_pair("00181190", "DSFocalSpot(s)"));
	m_mapDicomDictionary.insert(make_pair("00181191", "CSAnodeTargetMaterial"));
	m_mapDicomDictionary.insert(make_pair("001811A0", "DSBodyPartThickness"));
	m_mapDicomDictionary.insert(make_pair("001811A2", "DSCompressionForce"));
	m_mapDicomDictionary.insert(make_pair("00181200", "DADateofLastCalibration"));
	m_mapDicomDictionary.insert(make_pair("00181201", "TMTimeofLastCalibration"));
	m_mapDicomDictionary.insert(make_pair("00181210", "SHConvolutionKernel"));
	m_mapDicomDictionary.insert(make_pair("00181242", "ISActualFrameDuration"));
	m_mapDicomDictionary.insert(make_pair("00181243", "ISCountRate"));
	m_mapDicomDictionary.insert(make_pair("00181250", "SHReceiveCoilName"));
	m_mapDicomDictionary.insert(make_pair("00181251", "SHTransmitCoilName"));
	m_mapDicomDictionary.insert(make_pair("00181260", "SHPlateType"));
	m_mapDicomDictionary.insert(make_pair("00181261", "LOPhosphorType"));
	m_mapDicomDictionary.insert(make_pair("00181300", "ISScanVelocity"));
	m_mapDicomDictionary.insert(make_pair("00181301", "CSWholeBodyTechnique"));
	m_mapDicomDictionary.insert(make_pair("00181302", "ISScanLength"));
	m_mapDicomDictionary.insert(make_pair("00181310", "USAcquisitionMatrix"));
	m_mapDicomDictionary.insert(make_pair("00181312", "CSIn-planePhaseEncodingDirection"));
	m_mapDicomDictionary.insert(make_pair("00181314", "DSFlipAngle"));
	m_mapDicomDictionary.insert(make_pair("00181315", "CSVariableFlipAngleFlag"));
	m_mapDicomDictionary.insert(make_pair("00181316", "DSSAR"));
	m_mapDicomDictionary.insert(make_pair("00181318", "DSdB/dt"));
	m_mapDicomDictionary.insert(make_pair("00181400", "LOAcquisitionDeviceProcessingDescription"));
	m_mapDicomDictionary.insert(make_pair("00181401", "LOAcquisitionDeviceProcessingCode"));
	m_mapDicomDictionary.insert(make_pair("00181402", "CSCassetteOrientation"));
	m_mapDicomDictionary.insert(make_pair("00181403", "CSCassetteSize"));
	m_mapDicomDictionary.insert(make_pair("00181404", "USExposuresonPlate"));
	m_mapDicomDictionary.insert(make_pair("00181405", "ISRelativeX-RayExposure"));
	m_mapDicomDictionary.insert(make_pair("00181450", "CSColumnAngulation"));
	m_mapDicomDictionary.insert(make_pair("00181500", "CSPositionerMotion"));
	m_mapDicomDictionary.insert(make_pair("00181508", "CSPositionerType"));
	m_mapDicomDictionary.insert(make_pair("00181510", "DSPositionerPrimaryAngle"));
	m_mapDicomDictionary.insert(make_pair("00181511", "DSPositionerSecondaryAngle"));
	m_mapDicomDictionary.insert(make_pair("00181520", "DSPositionerPrimaryAngleIncrement"));
	m_mapDicomDictionary.insert(make_pair("00181521", "DSPositionerSecondaryAngleIncrement"));
	m_mapDicomDictionary.insert(make_pair("00181530", "DSDetectorPrimaryAngle"));
	m_mapDicomDictionary.insert(make_pair("00181531", "DSDetectorSecondaryAngle"));
	m_mapDicomDictionary.insert(make_pair("00181600", "CSShutterShape"));
	m_mapDicomDictionary.insert(make_pair("00181602", "ISShutterLeftVerticalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181604", "ISShutterRightVerticalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181606", "ISShutterUpperHorizontalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181608", "ISShutterLowerHorizontalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181610", "ISCenterofCircularShutter"));
	m_mapDicomDictionary.insert(make_pair("00181612", "ISRadiusofCircularShutter"));
	m_mapDicomDictionary.insert(make_pair("00181620", "ISVerticesofthePolygonalShutter"));
	m_mapDicomDictionary.insert(make_pair("00181700", "ISCollimatorShape"));
	m_mapDicomDictionary.insert(make_pair("00181702", "ISCollimatorLeftVerticalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181704", "ISCollimatorRightVerticalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181706", "ISCollimatorUpperHorizontalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181708", "ISCollimatorLowerHorizontalEdge"));
	m_mapDicomDictionary.insert(make_pair("00181710", "ISCenterofCircularCollimator"));
	m_mapDicomDictionary.insert(make_pair("00181712", "ISRadiusofCircularCollimator"));
	m_mapDicomDictionary.insert(make_pair("00181720", "ISVerticesofthePolygonalCollimator"));
	m_mapDicomDictionary.insert(make_pair("00185000", "SHOutputPower"));
	m_mapDicomDictionary.insert(make_pair("00185010", "LOTransducerData"));
	m_mapDicomDictionary.insert(make_pair("00185012", "DSFocusDepth"));
	m_mapDicomDictionary.insert(make_pair("00185020", "LOProcessingFunction"));
	m_mapDicomDictionary.insert(make_pair("00185021", "LOPostprocessingFunction"));
	m_mapDicomDictionary.insert(make_pair("00185022", "DSMechanicalIndex"));
	m_mapDicomDictionary.insert(make_pair("00185024", "DSBoneThermalIndex"));
	m_mapDicomDictionary.insert(make_pair("00185026", "DSCranialThermalIndex"));
	m_mapDicomDictionary.insert(make_pair("00185027", "DSSoftTissueThermalIndex"));
	m_mapDicomDictionary.insert(make_pair("00185028", "DSSoftTissue-focusThermalIndex"));
	m_mapDicomDictionary.insert(make_pair("00185029", "DSSoftTissue-surfaceThermalIndex"));
	m_mapDicomDictionary.insert(make_pair("00185050", "ISDepthofScanField"));
	m_mapDicomDictionary.insert(make_pair("00185100", "CSPatientPosition"));
	m_mapDicomDictionary.insert(make_pair("00185101", "CSViewPosition"));
	m_mapDicomDictionary.insert(make_pair("00185104", "SQProjectionEponymousNameCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00185210", "DSImageTransformationMatrix"));
	m_mapDicomDictionary.insert(make_pair("00185212", "DSImageTranslationVector"));
	m_mapDicomDictionary.insert(make_pair("00186000", "DSSensitivity"));
	m_mapDicomDictionary.insert(make_pair("00186011", "SQSequenceofUltrasoundRegions"));
	m_mapDicomDictionary.insert(make_pair("00186012", "USRegionSpatialFormat"));
	m_mapDicomDictionary.insert(make_pair("00186014", "USRegionDataType"));
	m_mapDicomDictionary.insert(make_pair("00186016", "ULRegionFlags"));
	m_mapDicomDictionary.insert(make_pair("00186018", "ULRegionLocationMinX0"));
	m_mapDicomDictionary.insert(make_pair("0018601A", "ULRegionLocationMinY0"));
	m_mapDicomDictionary.insert(make_pair("0018601C", "ULRegionLocationMaxX1"));
	m_mapDicomDictionary.insert(make_pair("0018601E", "ULRegionLocationMaxY1"));
	m_mapDicomDictionary.insert(make_pair("00186020", "SLReferencePixelX0"));
	m_mapDicomDictionary.insert(make_pair("00186022", "SLReferencePixelY0"));
	m_mapDicomDictionary.insert(make_pair("00186024", "USPhysicalUnitsXDirection"));
	m_mapDicomDictionary.insert(make_pair("00186026", "USPhysicalUnitsYDirection"));
	m_mapDicomDictionary.insert(make_pair("00181628", "FDReferencePixelPhysicalValueX"));
	m_mapDicomDictionary.insert(make_pair("0018602A", "FDReferencePixelPhysicalValueY"));
	m_mapDicomDictionary.insert(make_pair("0018602C", "FDPhysicalDeltaX"));
	m_mapDicomDictionary.insert(make_pair("0018602E", "FDPhysicalDeltaY"));
	m_mapDicomDictionary.insert(make_pair("00186030", "ULTransducerFrequency"));
	m_mapDicomDictionary.insert(make_pair("00186031", "CSTransducerType"));
	m_mapDicomDictionary.insert(make_pair("00186032", "ULPulseRepetitionFrequency"));
	m_mapDicomDictionary.insert(make_pair("00186034", "FDDopplerCorrectionAngle"));
	m_mapDicomDictionary.insert(make_pair("00186036", "FDSteeringAngle"));
	m_mapDicomDictionary.insert(make_pair("00186038", "ULDopplerSampleVolumeXPosition(Retired)"));
	m_mapDicomDictionary.insert(make_pair("00186039", "SLDopplerSampleVolumeXPosition"));
	m_mapDicomDictionary.insert(make_pair("0018603A", "ULDopplerSampleVolumeYPosition(Retired)"));
	m_mapDicomDictionary.insert(make_pair("0018603B", "SLDopplerSampleVolumeYPosition"));
	m_mapDicomDictionary.insert(make_pair("0018603C", "ULTM-LinePositionX0(Retired)"));
	m_mapDicomDictionary.insert(make_pair("0018603D", "SLTM-LinePositionX0"));
	m_mapDicomDictionary.insert(make_pair("0018603E", "ULTM-LinePositionY0(Retired)"));
	m_mapDicomDictionary.insert(make_pair("0018603F", "SLTM-LinePositionY0"));
	m_mapDicomDictionary.insert(make_pair("00186040", "ULTM-LinePositionX1(Retired)"));
	m_mapDicomDictionary.insert(make_pair("00186041", "SLTM-LinePositionX1"));
	m_mapDicomDictionary.insert(make_pair("00186042", "ULTM-LinePositionY1(Retired)"));
	m_mapDicomDictionary.insert(make_pair("00186043", "SLTM-LinePositionY1"));
	m_mapDicomDictionary.insert(make_pair("00186044", "USPixelComponentOrganization"));
	m_mapDicomDictionary.insert(make_pair("00186046", "ULPixelComponentMask"));
	m_mapDicomDictionary.insert(make_pair("00186048", "ULPixelComponentRangeStart"));
	m_mapDicomDictionary.insert(make_pair("0018604A", "ULPixelComponentRangeStop"));
	m_mapDicomDictionary.insert(make_pair("0018604C", "USPixelComponentPhysicalUnits"));
	m_mapDicomDictionary.insert(make_pair("0018604E", "USPixelComponentDataType"));
	m_mapDicomDictionary.insert(make_pair("00186050", "ULNumberofTableBreakPoints"));
	m_mapDicomDictionary.insert(make_pair("00186052", "ULTableofXBreakPoints"));
	m_mapDicomDictionary.insert(make_pair("00186054", "FDTableofYBreakPoints"));
	m_mapDicomDictionary.insert(make_pair("00186056", "ULNumberofTableEntries"));
	m_mapDicomDictionary.insert(make_pair("00186058", "ULTableofPixelValues"));
	m_mapDicomDictionary.insert(make_pair("0018605A", "ULTableofParameterValues"));
	m_mapDicomDictionary.insert(make_pair("00187000", "CSDetectorConditionsNominalFlag"));
	m_mapDicomDictionary.insert(make_pair("00187001", "DSDetectorTemperature"));
	m_mapDicomDictionary.insert(make_pair("00187004", "CSDetectorType"));
	m_mapDicomDictionary.insert(make_pair("00187005", "CSDetectorConfiguration"));
	m_mapDicomDictionary.insert(make_pair("00187006", "LTDetectorDescription"));
	m_mapDicomDictionary.insert(make_pair("00187008", "LTDetectorMode"));
	m_mapDicomDictionary.insert(make_pair("0018700A", "SHDetectorID"));
	m_mapDicomDictionary.insert(make_pair("0018700C", "DADateofLastDetectorCalibration"));
	m_mapDicomDictionary.insert(make_pair("0018700E", "TMTimeofLastDetectorCalibration"));
	m_mapDicomDictionary.insert(make_pair("00187010", "ISExposuresonDetectorSinceLastCalibration"));
	m_mapDicomDictionary.insert(make_pair("00187011", "ISExposuresonDetectorSinceManufactured"));
	m_mapDicomDictionary.insert(make_pair("00187012", "DSDetectorTimeSinceLastExposure"));
	m_mapDicomDictionary.insert(make_pair("00187014", "DSDetectorActiveTime"));
	m_mapDicomDictionary.insert(make_pair("00187016", "DSDetectorActivationOffsetFromExposure"));
	m_mapDicomDictionary.insert(make_pair("0018701A", "DSDetectorBinning"));
	m_mapDicomDictionary.insert(make_pair("00187020", "DSDetectorElementPhysicalSize"));
	m_mapDicomDictionary.insert(make_pair("00187022", "DSDetectorElementSpacing"));
	m_mapDicomDictionary.insert(make_pair("00187024", "CSDetectorActiveShape"));
	m_mapDicomDictionary.insert(make_pair("00187026", "DSDetectorActiveDimension(s)"));
	m_mapDicomDictionary.insert(make_pair("00187028", "DSDetectorActiveOrigin"));
	m_mapDicomDictionary.insert(make_pair("00187030", "DSFieldofViewOrigin"));
	m_mapDicomDictionary.insert(make_pair("00187032", "DSFieldofViewRotation"));
	m_mapDicomDictionary.insert(make_pair("00187034", "CSFieldofViewHorizontalFlip"));
	m_mapDicomDictionary.insert(make_pair("00187040", "LTGridAbsorbingMaterial"));
	m_mapDicomDictionary.insert(make_pair("00187041", "LTGridSpacingMaterial"));
	m_mapDicomDictionary.insert(make_pair("00187042", "DSGridThickness"));
	m_mapDicomDictionary.insert(make_pair("00187044", "DSGridPitch"));
	m_mapDicomDictionary.insert(make_pair("00187046", "ISGridAspectRatio"));
	m_mapDicomDictionary.insert(make_pair("00187048", "DSGridPeriod"));
	m_mapDicomDictionary.insert(make_pair("0018704C", "DSGridFocalDistance"));
	m_mapDicomDictionary.insert(make_pair("00187050", "LTFilterMaterial"));
	m_mapDicomDictionary.insert(make_pair("00187052", "DSFilterThicknessMinimum"));
	m_mapDicomDictionary.insert(make_pair("00187054", "DSFilterThicknessMaximum"));
	m_mapDicomDictionary.insert(make_pair("00187060", "CSExposureControlMode"));
	m_mapDicomDictionary.insert(make_pair("00187062", "LTExposureControlModeDescription"));
	m_mapDicomDictionary.insert(make_pair("00187064", "CSExposureStatus"));
	m_mapDicomDictionary.insert(make_pair("00187065", "DSPhototimerSetting"));
	m_mapDicomDictionary.insert(make_pair("0020000D", "UIStudyInstanceUID"));
	m_mapDicomDictionary.insert(make_pair("0020000E", "UISeriesInstanceUID"));
	m_mapDicomDictionary.insert(make_pair("00200010", "SHStudyID"));
	m_mapDicomDictionary.insert(make_pair("00200011", "ISSeriesNumber"));
	m_mapDicomDictionary.insert(make_pair("00200012", "ISAcquisitionNumber"));
	m_mapDicomDictionary.insert(make_pair("00200013", "ISInstanceNumber"));
	m_mapDicomDictionary.insert(make_pair("00200014", "ISIsotopeNumber"));
	m_mapDicomDictionary.insert(make_pair("00200015", "ISPhaseNumber"));
	m_mapDicomDictionary.insert(make_pair("00200016", "ISIntervalNumber"));
	m_mapDicomDictionary.insert(make_pair("00200017", "ISTimeSlotNumber"));
	m_mapDicomDictionary.insert(make_pair("00200018", "ISAngleNumber"));
	m_mapDicomDictionary.insert(make_pair("00200020", "CSPatientOrientation"));
	m_mapDicomDictionary.insert(make_pair("00200022", "USOverlayNumber"));
	m_mapDicomDictionary.insert(make_pair("00200024", "USCurveNumber"));
	m_mapDicomDictionary.insert(make_pair("00200030", "DSImagePosition"));
	m_mapDicomDictionary.insert(make_pair("00200032", "DSImagePosition(Patient)"));
	m_mapDicomDictionary.insert(make_pair("00200037", "DSImageOrientation(Patient)"));
	m_mapDicomDictionary.insert(make_pair("00200050", "DSLocation"));
	m_mapDicomDictionary.insert(make_pair("00200052", "UIFrameofReferenceUID"));
	m_mapDicomDictionary.insert(make_pair("00200060", "CSLaterality"));
	m_mapDicomDictionary.insert(make_pair("00200070", "LOImageGeometryType"));
	m_mapDicomDictionary.insert(make_pair("00200080", "UIMaskingImage"));
	m_mapDicomDictionary.insert(make_pair("00200100", "ISTemporalPositionIdentifier"));
	m_mapDicomDictionary.insert(make_pair("00200105", "ISNumberofTemporalPositions"));
	m_mapDicomDictionary.insert(make_pair("00200110", "DSTemporalResolution"));
	m_mapDicomDictionary.insert(make_pair("00201000", "ISSeriesinStudy"));
	m_mapDicomDictionary.insert(make_pair("00201002", "ISImagesinAcquisition"));
	m_mapDicomDictionary.insert(make_pair("00201004", "ISAcquisitionsinStudy"));
	m_mapDicomDictionary.insert(make_pair("00201040", "LOPositionReferenceIndicator"));
	m_mapDicomDictionary.insert(make_pair("00201041", "DSSliceLocation"));
	m_mapDicomDictionary.insert(make_pair("00201070", "ISOtherStudyNumbers"));
	m_mapDicomDictionary.insert(make_pair("00201200", "ISNumberofPatientRelatedStudies"));
	m_mapDicomDictionary.insert(make_pair("00201202", "ISNumberofPatientRelatedSeries"));
	m_mapDicomDictionary.insert(make_pair("00201204", "ISNumberofPatientRelatedInstances"));
	m_mapDicomDictionary.insert(make_pair("00201206", "ISNumberofStudyRelatedSeries"));
	m_mapDicomDictionary.insert(make_pair("00201208", "ISNumberofStudyRelatedInstances"));
	m_mapDicomDictionary.insert(make_pair("00204000", "LTImageComments"));
	m_mapDicomDictionary.insert(make_pair("00280002", "USSamplesperPixel"));
	m_mapDicomDictionary.insert(make_pair("00280004", "CSPhotometricInterpretation"));
	m_mapDicomDictionary.insert(make_pair("00280006", "USPlanarConfiguration"));
	m_mapDicomDictionary.insert(make_pair("00280008", "ISNumberofFrames"));
	m_mapDicomDictionary.insert(make_pair("00280009", "ATFrameIncrementPointer"));
	m_mapDicomDictionary.insert(make_pair("00280010", "USRows"));
	m_mapDicomDictionary.insert(make_pair("00280011", "USColumns"));
	m_mapDicomDictionary.insert(make_pair("00280030", "DSPixelSpacing"));
	m_mapDicomDictionary.insert(make_pair("00280031", "DSZoomFactor"));
	m_mapDicomDictionary.insert(make_pair("00280032", "DSZoomCenter"));
	m_mapDicomDictionary.insert(make_pair("00280034", "ISPixelAspectRatio"));
	m_mapDicomDictionary.insert(make_pair("00280051", "CSCorrectedImage"));
	m_mapDicomDictionary.insert(make_pair("00280100", "USBitsAllocated"));
	m_mapDicomDictionary.insert(make_pair("00280101", "USBitsStored"));
	m_mapDicomDictionary.insert(make_pair("00280102", "USHighBit"));
	m_mapDicomDictionary.insert(make_pair("00280103", "USPixelRepresentation"));
	m_mapDicomDictionary.insert(make_pair("00280106", "USSmallestImagePixelValue"));
	m_mapDicomDictionary.insert(make_pair("00280107", "USLargestImagePixelValue"));
	m_mapDicomDictionary.insert(make_pair("00280108", "USSmallestPixelValueinSeries"));
	m_mapDicomDictionary.insert(make_pair("00280109", "USLargestPixelValueinSeries"));
	m_mapDicomDictionary.insert(make_pair("00280120", "USPixelPaddingValue"));
	m_mapDicomDictionary.insert(make_pair("00280300", "CSQualityControlImage"));
	m_mapDicomDictionary.insert(make_pair("00280301", "CSBurnedInAnnotation"));
	m_mapDicomDictionary.insert(make_pair("00281040", "CSPixelIntensityRelationship"));
	m_mapDicomDictionary.insert(make_pair("00281041", "SSPixelIntensityRelationshipSign"));
	m_mapDicomDictionary.insert(make_pair("00281050", "DSWindowCenter"));
	m_mapDicomDictionary.insert(make_pair("00281051", "DSWindowWidth"));
	m_mapDicomDictionary.insert(make_pair("00281052", "DSRescaleIntercept"));
	m_mapDicomDictionary.insert(make_pair("00281053", "DSRescaleSlope"));
	m_mapDicomDictionary.insert(make_pair("00281054", "LORescaleType"));
	m_mapDicomDictionary.insert(make_pair("00281055", "LOWindowCenter&WidthExplanation"));
	m_mapDicomDictionary.insert(make_pair("00281101", "USRedPaletteColorLookupTableDescriptor"));
	m_mapDicomDictionary.insert(make_pair("00281102", "USGreenPaletteColorLookupTableDescriptor"));
	m_mapDicomDictionary.insert(make_pair("00281103", "USBluePaletteColorLookupTableDescriptor"));
	m_mapDicomDictionary.insert(make_pair("00281104", "USAlphaPaletteColorLookupTableDescriptor"));
	m_mapDicomDictionary.insert(make_pair("00281201", "OWRedPaletteColorLookupTableData"));
	m_mapDicomDictionary.insert(make_pair("00281202", "OWGreenPaletteColorLookupTableData"));
	m_mapDicomDictionary.insert(make_pair("00281203", "OWBluePaletteColorLookupTableData"));
	m_mapDicomDictionary.insert(make_pair("00281204", "OWAlphaPaletteColorLookupTableData"));
	m_mapDicomDictionary.insert(make_pair("00282110", "CSLossyImageCompression"));
	m_mapDicomDictionary.insert(make_pair("00283000", "SQModalityLUTSequence"));
	m_mapDicomDictionary.insert(make_pair("00283002", "USLUTDescriptor"));
	m_mapDicomDictionary.insert(make_pair("00283003", "LOLUTExplanation"));
	m_mapDicomDictionary.insert(make_pair("00283004", "LOModalityLUTType"));
	m_mapDicomDictionary.insert(make_pair("00283006", "USLUTData"));
	m_mapDicomDictionary.insert(make_pair("00283010", "SQVOILUTSequence"));
	m_mapDicomDictionary.insert(make_pair("0032000A", "CSStudyStatusID"));
	m_mapDicomDictionary.insert(make_pair("0032000C", "CSStudyPriorityID"));
	m_mapDicomDictionary.insert(make_pair("00320012", "LOStudyIDIssuer"));
	m_mapDicomDictionary.insert(make_pair("00320032", "DAStudyVerifiedDate"));
	m_mapDicomDictionary.insert(make_pair("00320033", "TMStudyVerifiedTime"));
	m_mapDicomDictionary.insert(make_pair("00320034", "DAStudyReadDate"));
	m_mapDicomDictionary.insert(make_pair("00320035", "TMStudyReadTime"));
	m_mapDicomDictionary.insert(make_pair("00321000", "DAScheduledStudyStartDate"));
	m_mapDicomDictionary.insert(make_pair("00321001", "TMScheduledStudyStartTime"));
	m_mapDicomDictionary.insert(make_pair("00321010", "DAScheduledStudyStopDate"));
	m_mapDicomDictionary.insert(make_pair("00321011", "TMScheduledStudyStopTime"));
	m_mapDicomDictionary.insert(make_pair("00321020", "LOScheduledStudyLocation"));
	m_mapDicomDictionary.insert(make_pair("00321021", "AEScheduledStudyLocationAETitle"));
	m_mapDicomDictionary.insert(make_pair("00321030", "LOReasonforStudy"));
	m_mapDicomDictionary.insert(make_pair("00321032", "PNRequestingPhysician"));
	m_mapDicomDictionary.insert(make_pair("00321033", "LORequestingService"));
	m_mapDicomDictionary.insert(make_pair("00321040", "DAStudyArrivalDate"));
	m_mapDicomDictionary.insert(make_pair("00321041", "TMStudyArrivalTime"));
	m_mapDicomDictionary.insert(make_pair("00321050", "DAStudyCompletionDate"));
	m_mapDicomDictionary.insert(make_pair("00321051", "TMStudyCompletionTime"));
	m_mapDicomDictionary.insert(make_pair("00321055", "CSStudyComponentStatusID"));
	m_mapDicomDictionary.insert(make_pair("00321060", "LORequestedProcedureDescription"));
	m_mapDicomDictionary.insert(make_pair("00321064", "SQRequestedProcedureCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00321070", "LORequestedContrastAgent"));
	m_mapDicomDictionary.insert(make_pair("00324000", "LTStudyComments"));
	m_mapDicomDictionary.insert(make_pair("00400001", "AEScheduledStationAETitle"));
	m_mapDicomDictionary.insert(make_pair("00400002", "DAScheduledProcedureStepStartDate"));
	m_mapDicomDictionary.insert(make_pair("00400003", "TMScheduledProcedureStepStartTime"));
	m_mapDicomDictionary.insert(make_pair("00400004", "DAScheduledProcedureStepEndDate"));
	m_mapDicomDictionary.insert(make_pair("00400005", "TMScheduledProcedureStepEndTime"));
	m_mapDicomDictionary.insert(make_pair("00400006", "PNScheduledPerformingPhysician'sName"));
	m_mapDicomDictionary.insert(make_pair("00400007", "LOScheduledProcedureStepDescription"));
	m_mapDicomDictionary.insert(make_pair("00400008", "SQScheduledProtocolCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00400009", "SHScheduledProcedureStepID"));
	m_mapDicomDictionary.insert(make_pair("00400010", "SHScheduledStationName"));
	m_mapDicomDictionary.insert(make_pair("00400011", "SHScheduledProcedureStepLocation"));
	m_mapDicomDictionary.insert(make_pair("00400012", "LOPre-Medication"));
	m_mapDicomDictionary.insert(make_pair("00400020", "CSScheduledProcedureStepStatus"));
	m_mapDicomDictionary.insert(make_pair("00400100", "SQScheduledProcedureStepSequence"));
	m_mapDicomDictionary.insert(make_pair("00400220", "SQReferencedNon-ImageCompositeSOPInstanceSequence"));
	m_mapDicomDictionary.insert(make_pair("00400241", "AEPerformedStationAETitle"));
	m_mapDicomDictionary.insert(make_pair("00400242", "SHPerformedStationName"));
	m_mapDicomDictionary.insert(make_pair("00400243", "SHPerformedLocation"));
	m_mapDicomDictionary.insert(make_pair("00400244", "DAPerformedProcedureStepStartDate"));
	m_mapDicomDictionary.insert(make_pair("00400245", "TMPerformedProcedureStepStartTime"));
	m_mapDicomDictionary.insert(make_pair("00400250", "DAPerformedProcedureStepEndDate"));
	m_mapDicomDictionary.insert(make_pair("00400251", "TMPerformedProcedureStepEndTime"));
	m_mapDicomDictionary.insert(make_pair("00400252", "CSPerformedProcedureStepStatus"));
	m_mapDicomDictionary.insert(make_pair("00400253", "SHPerformedProcedureStepID"));
	m_mapDicomDictionary.insert(make_pair("00400254", "LOPerformedProcedureStepDescription"));
	m_mapDicomDictionary.insert(make_pair("00400255", "LOPerformedProcedureTypeDescription"));
	m_mapDicomDictionary.insert(make_pair("00400260", "SQPerformedProtocolCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00400270", "SQScheduledStepAttributesSequence"));
	m_mapDicomDictionary.insert(make_pair("00400275", "SQRequestAttributesSequence"));
	m_mapDicomDictionary.insert(make_pair("00400280", "STCommentsonthePerformedProcedureStep"));
	m_mapDicomDictionary.insert(make_pair("00400293", "SQQuantitySequence"));
	m_mapDicomDictionary.insert(make_pair("00400294", "DSQuantity"));
	m_mapDicomDictionary.insert(make_pair("00400295", "SQMeasuringUnitsSequence"));
	m_mapDicomDictionary.insert(make_pair("00400296", "SQBillingItemSequence"));
	m_mapDicomDictionary.insert(make_pair("00400300", "USTotalTimeofFluoroscopy"));
	m_mapDicomDictionary.insert(make_pair("00400301", "USTotalNumberofExposures"));
	m_mapDicomDictionary.insert(make_pair("00400302", "USEntranceDose"));
	m_mapDicomDictionary.insert(make_pair("00400303", "USExposedArea"));
	m_mapDicomDictionary.insert(make_pair("00400306", "DSDistanceSourcetoEntrance"));
	m_mapDicomDictionary.insert(make_pair("00400307", "DSDistanceSourcetoSupport"));
	m_mapDicomDictionary.insert(make_pair("00400310", "STCommentsonRadiationDose"));
	m_mapDicomDictionary.insert(make_pair("00400312", "DSX-RayOutput"));
	m_mapDicomDictionary.insert(make_pair("00400314", "DSHalfValueLayer"));
	m_mapDicomDictionary.insert(make_pair("00400316", "DSOrganDose"));
	m_mapDicomDictionary.insert(make_pair("00400318", "CSOrganExposed"));
	m_mapDicomDictionary.insert(make_pair("00400320", "SQBillingProcedureStepSequence"));
	m_mapDicomDictionary.insert(make_pair("00400321", "SQFilmConsumptionSequence"));
	m_mapDicomDictionary.insert(make_pair("00400324", "SQBillingSuppliesandDevicesSequence"));
	m_mapDicomDictionary.insert(make_pair("00400330", "SQReferencedProcedureStepSequence"));
	m_mapDicomDictionary.insert(make_pair("00400340", "SQPerformedSeriesSequence"));
	m_mapDicomDictionary.insert(make_pair("00400400", "LTCommentsontheScheduledProcedureStep"));
	m_mapDicomDictionary.insert(make_pair("0040050A", "LOSpecimenAccessionNumber"));
	m_mapDicomDictionary.insert(make_pair("00400550", "SQSpecimenSequence"));
	m_mapDicomDictionary.insert(make_pair("00400551", "LOSpecimenIdentifier"));
	m_mapDicomDictionary.insert(make_pair("00400555", "SQAcquisitionContextSequence"));
	m_mapDicomDictionary.insert(make_pair("00400556", "STAcquisitionContextDescription"));
	m_mapDicomDictionary.insert(make_pair("0040059A", "SQSpecimenTypeCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("004006FA", "LOSlideIdentifier"));
	m_mapDicomDictionary.insert(make_pair("0040071A", "SQImageCenterPointCoordinatesSequence"));
	m_mapDicomDictionary.insert(make_pair("0040072A", "DSXOffsetinSlideCoordinateSystem"));
	m_mapDicomDictionary.insert(make_pair("0040073A", "DSYOffsetinSlideCoordinateSystem"));
	m_mapDicomDictionary.insert(make_pair("0040074A", "DSZOffsetinSlideCoordinateSystem"));
	m_mapDicomDictionary.insert(make_pair("004008D8", "SQPixelSpacingSequence"));
	m_mapDicomDictionary.insert(make_pair("004008DA", "SQCoordinateSystemAxisCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("004008EA", "SQMeasurementUnitsCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00401001", "SHRequestedProcedureID"));
	m_mapDicomDictionary.insert(make_pair("00401002", "LOReasonfortheRequestedProcedure"));
	m_mapDicomDictionary.insert(make_pair("00401003", "SHRequestedProcedurePriority"));
	m_mapDicomDictionary.insert(make_pair("00401004", "LOPatientTransportArrangements"));
	m_mapDicomDictionary.insert(make_pair("00401005", "LORequestedProcedureLocation"));
	m_mapDicomDictionary.insert(make_pair("00401006", "SHPlacerOrderNumber/Procedure"));
	m_mapDicomDictionary.insert(make_pair("00401007", "SHFillerOrderNumber/Procedure"));
	m_mapDicomDictionary.insert(make_pair("00401008", "LOConfidentialityCode"));
	m_mapDicomDictionary.insert(make_pair("00401009", "SHReportingPriority"));
	m_mapDicomDictionary.insert(make_pair("00401010", "PNNamesofIntendedRecipientsofResults"));
	m_mapDicomDictionary.insert(make_pair("00401400", "LTRequestedProcedureComments"));
	m_mapDicomDictionary.insert(make_pair("00402001", "LOReasonfortheImagingServiceRequest"));
	m_mapDicomDictionary.insert(make_pair("00402004", "DAIssueDateofImagingServiceRequest"));
	m_mapDicomDictionary.insert(make_pair("00402005", "TMIssueTimeofImagingServiceRequest"));
	m_mapDicomDictionary.insert(make_pair("00402006", "SHPlacerOrderNumber/ImagingServiceRequest(Retired)"));
	m_mapDicomDictionary.insert(make_pair("00402007", "SHFillerOrderNumber/ImagingServiceRequest(Retired)"));
	m_mapDicomDictionary.insert(make_pair("00402008", "PNOrderEnteredBy"));
	m_mapDicomDictionary.insert(make_pair("00402009", "SHOrderEnterer'sLocation"));
	m_mapDicomDictionary.insert(make_pair("00402010", "SHOrderCallbackPhoneNumber"));
	m_mapDicomDictionary.insert(make_pair("00402016", "LOPlacerOrderNumber/ImagingServiceRequest"));
	m_mapDicomDictionary.insert(make_pair("00402017", "LOFillerOrderNumber/ImagingServiceRequest"));
	m_mapDicomDictionary.insert(make_pair("00402400", "LTImagingServiceRequestComments"));
	m_mapDicomDictionary.insert(make_pair("00403001", "LOConfidentialityConstraintonPatientDataDescription"));
	m_mapDicomDictionary.insert(make_pair("00408302", "DSEntranceDoseinmGy"));
	m_mapDicomDictionary.insert(make_pair("0040A010", "CSRelationshipType"));
	m_mapDicomDictionary.insert(make_pair("0040A027", "LOVerifyingOrganization"));
	m_mapDicomDictionary.insert(make_pair("0040A030", "DTVerificationDateTime"));
	m_mapDicomDictionary.insert(make_pair("0040A032", "DTObservationDateTime"));
	m_mapDicomDictionary.insert(make_pair("0040A040", "CSValueType"));
	m_mapDicomDictionary.insert(make_pair("0040A043", "SQConceptNameCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A050", "CSContinuityOfContent"));
	m_mapDicomDictionary.insert(make_pair("0040A073", "SQVerifyingObserverSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A075", "PNVerifyingObserverName"));
	m_mapDicomDictionary.insert(make_pair("0040A088", "SQVerifyingObserverIdentificationCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A0B0", "USReferencedWaveformChannels"));
	m_mapDicomDictionary.insert(make_pair("0040A120", "DTDateTime"));
	m_mapDicomDictionary.insert(make_pair("0040A121", "DADate"));
	m_mapDicomDictionary.insert(make_pair("0040A122", "TMTime"));
	m_mapDicomDictionary.insert(make_pair("0040A123", "PNPersonName"));
	m_mapDicomDictionary.insert(make_pair("0040A124", "UIUID"));
	m_mapDicomDictionary.insert(make_pair("0040A130", "CSTemporalRangeType"));
	m_mapDicomDictionary.insert(make_pair("0040A132", "ULReferencedSamplePositions"));
	m_mapDicomDictionary.insert(make_pair("0040A136", "USReferencedFrameNumbers"));
	m_mapDicomDictionary.insert(make_pair("0040A138", "DSReferencedTimeOffsets"));
	m_mapDicomDictionary.insert(make_pair("0040A13A", "DTReferencedDateTime"));
	m_mapDicomDictionary.insert(make_pair("0040A160", "UTTextValue"));
	m_mapDicomDictionary.insert(make_pair("0040A168", "SQConceptCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A180", "USAnnotationGroupNumber"));
	m_mapDicomDictionary.insert(make_pair("0040A195", "SQModifierCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A300", "SQMeasuredValueSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A30A", "DSNumericValue"));
	m_mapDicomDictionary.insert(make_pair("0040A360", "SQPredecessorDocumentsSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A370", "SQReferencedRequestSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A372", "SQPerformedProcedureCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A375", "SQCurrentRequestedProcedureEvidenceSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A385", "SQPertinentOtherEvidenceSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A491", "CSCompletionFlag"));
	m_mapDicomDictionary.insert(make_pair("0040A492", "LOCompletionFlagDescription"));
	m_mapDicomDictionary.insert(make_pair("0040A493", "CSVerificationFlag"));
	m_mapDicomDictionary.insert(make_pair("0040A504", "SQContentTemplateSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A525", "SQIdenticalDocumentsSequence"));
	m_mapDicomDictionary.insert(make_pair("0040A730", "SQContentSequence"));
	m_mapDicomDictionary.insert(make_pair("0040B020", "SQWaveformAnnotationSequence"));
	m_mapDicomDictionary.insert(make_pair("0040DB00", "CSTemplateIdentifier"));
	m_mapDicomDictionary.insert(make_pair("0040DB06", "DTTemplateVersion"));
	m_mapDicomDictionary.insert(make_pair("0040DB07", "DTTemplateLocalVersion"));
	m_mapDicomDictionary.insert(make_pair("0040DB0B", "CSTemplateExtensionFlag"));
	m_mapDicomDictionary.insert(make_pair("0040DB0C", "UITemplateExtensionOrganizationUID"));
	m_mapDicomDictionary.insert(make_pair("0040DB0D", "UITemplateExtensionCreatorUID"));
	m_mapDicomDictionary.insert(make_pair("0040DB73", "ULReferencedContentItemIdentifier"));
	m_mapDicomDictionary.insert(make_pair("00540011", "USNumberofEnergyWindows"));
	m_mapDicomDictionary.insert(make_pair("00540012", "SQEnergyWindowInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540013", "SQEnergyWindowRangeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540014", "DSEnergyWindowLowerLimit"));
	m_mapDicomDictionary.insert(make_pair("00540015", "DSEnergyWindowUpperLimit"));
	m_mapDicomDictionary.insert(make_pair("00540016", "SQRadiopharmaceuticalInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540017", "ISResidualSyringeCounts"));
	m_mapDicomDictionary.insert(make_pair("00540018", "SHEnergyWindowName"));
	m_mapDicomDictionary.insert(make_pair("00540020", "USDetectorVector"));
	m_mapDicomDictionary.insert(make_pair("00540021", "USNumberofDetectors"));
	m_mapDicomDictionary.insert(make_pair("00540022", "SQDetectorInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540030", "USPhaseVector"));
	m_mapDicomDictionary.insert(make_pair("00540031", "USNumberofPhases"));
	m_mapDicomDictionary.insert(make_pair("00540032", "SQPhaseInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540033", "USNumberofFramesinPhase"));
	m_mapDicomDictionary.insert(make_pair("00540036", "ISPhaseDelay"));
	m_mapDicomDictionary.insert(make_pair("00540038", "ISPauseBetweenFrames"));
	m_mapDicomDictionary.insert(make_pair("00540039", "CSPhaseDescription"));
	m_mapDicomDictionary.insert(make_pair("00540050", "USRotationVector"));
	m_mapDicomDictionary.insert(make_pair("00540051", "USNumberofRotations"));
	m_mapDicomDictionary.insert(make_pair("00540052", "SQRotationInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540053", "USNumberofFramesinRotation"));
	m_mapDicomDictionary.insert(make_pair("00540060", "USR-RIntervalVector"));
	m_mapDicomDictionary.insert(make_pair("00540061", "USNumberofR-RIntervals"));
	m_mapDicomDictionary.insert(make_pair("00540062", "SQGatedInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540063", "SQDataInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540070", "USTimeSlotVector"));
	m_mapDicomDictionary.insert(make_pair("00540071", "USNumberofTimeSlots"));
	m_mapDicomDictionary.insert(make_pair("00540072", "SQTimeSlotInformationSequence"));
	m_mapDicomDictionary.insert(make_pair("00540073", "DSTimeSlotTime"));
	m_mapDicomDictionary.insert(make_pair("00540080", "USSliceVector"));
	m_mapDicomDictionary.insert(make_pair("00540081", "USNumberofSlices"));
	m_mapDicomDictionary.insert(make_pair("00540090", "USAngularViewVector"));
	m_mapDicomDictionary.insert(make_pair("00540100", "USTimeSliceVector"));
	m_mapDicomDictionary.insert(make_pair("00540101", "USNumberofTimeSlices"));
	m_mapDicomDictionary.insert(make_pair("00540200", "DSStartAngle"));
	m_mapDicomDictionary.insert(make_pair("00540202", "CSTypeofDetectorMotion"));
	m_mapDicomDictionary.insert(make_pair("00540210", "ISTriggerVector"));
	m_mapDicomDictionary.insert(make_pair("00540211", "USNumberofTriggersinPhase"));
	m_mapDicomDictionary.insert(make_pair("00540220", "SQViewCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540222", "SQViewModifierCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540300", "SQRadionuclideCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540302", "SQAdministrationRouteCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540304", "SQRadiopharmaceuticalCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540306", "SQCalibrationDataSequence"));
	m_mapDicomDictionary.insert(make_pair("00540308", "USEnergyWindowNumber"));
	m_mapDicomDictionary.insert(make_pair("00540400", "SHImageID"));
	m_mapDicomDictionary.insert(make_pair("00540410", "SQPatientOrientationCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540412", "SQPatientOrientationModifierCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540414", "SQPatientGantryRelationshipCodeSequence"));
	m_mapDicomDictionary.insert(make_pair("00540500", "CSSliceProgressionDirection"));
	m_mapDicomDictionary.insert(make_pair("00541000", "CSSeriesType"));
	m_mapDicomDictionary.insert(make_pair("00541001", "CSUnits"));
	m_mapDicomDictionary.insert(make_pair("00541002", "CSCountsSource"));
	m_mapDicomDictionary.insert(make_pair("00541004", "CSReprojectionMethod"));
	m_mapDicomDictionary.insert(make_pair("00541100", "CSRandomsCorrectionMethod"));
	m_mapDicomDictionary.insert(make_pair("00541101", "LOAttenuationCorrectionMethod"));
	m_mapDicomDictionary.insert(make_pair("00541102", "CSDecayCorrection"));
	m_mapDicomDictionary.insert(make_pair("00541103", "LOReconstructionMethod"));
	m_mapDicomDictionary.insert(make_pair("00541104", "LODetectorLinesofResponseUsed"));
	m_mapDicomDictionary.insert(make_pair("00541105", "LOScatterCorrectionMethod"));
	m_mapDicomDictionary.insert(make_pair("00541200", "DSAxialAcceptance"));
	m_mapDicomDictionary.insert(make_pair("00541201", "ISAxialMash"));
	m_mapDicomDictionary.insert(make_pair("00541202", "ISTransverseMash"));
	m_mapDicomDictionary.insert(make_pair("00541203", "DSDetectorElementSize"));
	m_mapDicomDictionary.insert(make_pair("00541210", "DSCoincidenceWindowWidth"));
	m_mapDicomDictionary.insert(make_pair("00541220", "CSSecondaryCountsType"));
	m_mapDicomDictionary.insert(make_pair("00541300", "DSFrameReferenceTime"));
	m_mapDicomDictionary.insert(make_pair("00541310", "ISPrimary(Prompts)CountsAccumulated"));
	m_mapDicomDictionary.insert(make_pair("00541311", "ISSecondaryCountsAccumulated"));
	m_mapDicomDictionary.insert(make_pair("00541320", "DSSliceSensitivityFactor"));
	m_mapDicomDictionary.insert(make_pair("00541321", "DSDecayFactor"));
	m_mapDicomDictionary.insert(make_pair("00541322", "DSDoseCalibrationFactor"));
	m_mapDicomDictionary.insert(make_pair("00541323", "DSScatterFractionFactor"));
	m_mapDicomDictionary.insert(make_pair("00541324", "DSDeadTimeFactor"));
	m_mapDicomDictionary.insert(make_pair("00541330", "USImageIndex"));
	m_mapDicomDictionary.insert(make_pair("00541400", "CSCountsIncluded"));
	m_mapDicomDictionary.insert(make_pair("00541401", "CSDeadTimeCorrectionFlag"));
	m_mapDicomDictionary.insert(make_pair("20300010", "USAnnotationPosition"));
	m_mapDicomDictionary.insert(make_pair("20300020", "LOTextString"));
	m_mapDicomDictionary.insert(make_pair("20500010", "SQPresentationLUTSequence"));
	m_mapDicomDictionary.insert(make_pair("20500020", "CSPresentationLUTShape"));
	m_mapDicomDictionary.insert(make_pair("20500500", "SQReferencedPresentationLUTSequence"));
	m_mapDicomDictionary.insert(make_pair("30020002", "SHRTImageLabel"));
	m_mapDicomDictionary.insert(make_pair("30020003", "LORTImageName"));
	m_mapDicomDictionary.insert(make_pair("30020004", "STRTImageDescription"));
	m_mapDicomDictionary.insert(make_pair("3002000A", "CSReportedValuesOrigin"));
	m_mapDicomDictionary.insert(make_pair("3002000C", "CSRTImagePlane"));
	m_mapDicomDictionary.insert(make_pair("3002000D", "DSX-RayImageReceptorTranslation"));
	m_mapDicomDictionary.insert(make_pair("3002000E", "DSX-RayImageReceptorAngle"));
	m_mapDicomDictionary.insert(make_pair("30020010", "DSRTImageOrientation"));
	m_mapDicomDictionary.insert(make_pair("30020011", "DSImagePlanePixelSpacing"));
	m_mapDicomDictionary.insert(make_pair("30020012", "DSRTImagePosition"));
	m_mapDicomDictionary.insert(make_pair("30020020", "SHRadiationMachineName"));
	m_mapDicomDictionary.insert(make_pair("30020022", "DSRadiationMachineSAD"));
	m_mapDicomDictionary.insert(make_pair("30020024", "DSRadiationMachineSSD"));
	m_mapDicomDictionary.insert(make_pair("30020026", "DSRTImageSID"));
	m_mapDicomDictionary.insert(make_pair("30020028", "DSSourcetoReferenceObjectDistance"));
	m_mapDicomDictionary.insert(make_pair("30020029", "ISFractionNumber"));
	m_mapDicomDictionary.insert(make_pair("30020030", "SQExposureSequence"));
	m_mapDicomDictionary.insert(make_pair("30020032", "DSMetersetExposure"));
	m_mapDicomDictionary.insert(make_pair("30020034", "DSDiaphragmPosition"));
	m_mapDicomDictionary.insert(make_pair("30020040", "SQFluenceMapSequence"));
	m_mapDicomDictionary.insert(make_pair("30020041", "CSFluenceDataSource"));
	m_mapDicomDictionary.insert(make_pair("30020042", "DSFluenceDataScale"));
	m_mapDicomDictionary.insert(make_pair("30040001", "CSDVHType"));
	m_mapDicomDictionary.insert(make_pair("30040002", "CSDoseUnits"));
	m_mapDicomDictionary.insert(make_pair("30040004", "CSDoseType"));
	m_mapDicomDictionary.insert(make_pair("30040006", "LODoseComment"));
	m_mapDicomDictionary.insert(make_pair("30040008", "DSNormalizationPoint"));
	m_mapDicomDictionary.insert(make_pair("3004000A", "CSDoseSummationType"));
	m_mapDicomDictionary.insert(make_pair("3004000C", "DSGridFrameOffsetVector"));
	m_mapDicomDictionary.insert(make_pair("3004000E", "DSDoseGridScaling"));
	m_mapDicomDictionary.insert(make_pair("30040010", "SQRTDoseROISequence"));
	m_mapDicomDictionary.insert(make_pair("30040012", "DSDoseValue"));
	m_mapDicomDictionary.insert(make_pair("30040014", "CSTissueHeterogeneityCorrection"));
	m_mapDicomDictionary.insert(make_pair("30040040", "DSDVHNormalizationPoint"));
	m_mapDicomDictionary.insert(make_pair("30040042", "DSDVHNormalizationDoseValue"));
	m_mapDicomDictionary.insert(make_pair("30040050", "SQDVHSequence"));
	m_mapDicomDictionary.insert(make_pair("30040052", "DSDVHDoseScaling"));
	m_mapDicomDictionary.insert(make_pair("30040054", "CSDVHVolumeUnits"));
	m_mapDicomDictionary.insert(make_pair("30040056", "ISDVHNumberofBins"));
	m_mapDicomDictionary.insert(make_pair("30040058", "DSDVHData"));
	m_mapDicomDictionary.insert(make_pair("30040060", "SQDVHReferencedROISequence"));
	m_mapDicomDictionary.insert(make_pair("30040062", "CSDVHROIContributionType"));
	m_mapDicomDictionary.insert(make_pair("30040070", "DSDVHMinimumDose"));
	m_mapDicomDictionary.insert(make_pair("30040072", "DSDVHMaximumDose"));
	m_mapDicomDictionary.insert(make_pair("30040074", "DSDVHMeanDose"));
	m_mapDicomDictionary.insert(make_pair("300A00B3", "CSPrimaryDosimeterUnit"));
	m_mapDicomDictionary.insert(make_pair("300A00F0", "ISNumberofBlocks"));
	m_mapDicomDictionary.insert(make_pair("300A011E", "DSGantryAngle"));
	m_mapDicomDictionary.insert(make_pair("300A0120", "DSBeamLimitingDeviceAngle"));
	m_mapDicomDictionary.insert(make_pair("300A0122", "DSPatientSupportAngle"));
	m_mapDicomDictionary.insert(make_pair("300A0128", "DSTableTopVerticalPosition"));
	m_mapDicomDictionary.insert(make_pair("300A0129", "DSTableTopLongitudinalPosition"));
	m_mapDicomDictionary.insert(make_pair("300A012A", "DSTableTopLateralPosition"));
	m_mapDicomDictionary.insert(make_pair("300C0006", "ISReferencedBeamNumber"));
	m_mapDicomDictionary.insert(make_pair("300C0008", "DSStartCumulativeMetersetWeight"));
	m_mapDicomDictionary.insert(make_pair("300C0022", "ISReferencedFractionGroupNumber"));
	m_mapDicomDictionary.insert(make_pair("7FE00010", "OXPixelData"));//RepresentsOBorOWtypeofVR
	m_mapDicomDictionary.insert(make_pair("FFFEE000", "DLItem"));
	m_mapDicomDictionary.insert(make_pair("FFFEE00D", "DLItemDelimitationItem"));
	m_mapDicomDictionary.insert(make_pair("FFFEE0DD", "DLSequenceDelimitationItem"));
}

/*
 * @brief	transform integer to string
 * @param	nInVal: signed and unsigned integer supported
 * @param	ucBase: the base to be transformed into
 * @param	unStrValLen: length of transformed string, big endian, the minimum base is stored most right.
*/
template<typename T>
std::string CDicomRead::Int2Str(T tInVal, unsigned char ucBase, size_t unStrValLen)
{
	memset(m_czHexBuff, '0', unStrValLen);

	if (0 == unStrValLen)
	{
		do 
		{
			m_czHexBuff[unStrValLen++] = tInVal % ucBase + 48;
			tInVal /= ucBase;
		} while (tInVal > 0);
	} 
	else
	{
		for (int nIdx = unStrValLen - 1; nIdx > 1, tInVal > 0; nIdx--)
		{
			m_czHexBuff[nIdx] = tInVal % ucBase + 48;
			tInVal /= ucBase;
		}
	}

	return std::string(m_czHexBuff, unStrValLen);
}

/*
 * @brief	read bytes from file
 * @param	unBytesRead: bytes to read
*/
void CDicomRead::ReadBuf(unsigned int unBytesRead)
{
	m_oReadFile.read(m_czStreamBuff, unBytesRead);
	m_unStreamLocation += unBytesRead;
}

/*
 * @brief	read dicom file
 * @return	process result
*/
int CDicomRead::ReadDicom(size_t unBuffLen)
{
	__LOG_FUNC_START__;

	sf::path oFileName = sf::system_complete(sf::path(m_strFileName));

	if (!sf::exists(oFileName))
	{
		m_vecErrorReplacer.clear();
		m_vecErrorReplacer.push_back(oFileName.string());
		__LOG_ERROR__(CErrorMsg::GetInstance()->GetMsgString(INVALID_FILE_NAME, m_vecErrorReplacer));
		return INVALID_FILE_NAME;
	}

	m_oReadFile.open(oFileName.string(), ios::in | ios::binary);
	if (!m_oReadFile.good())
	{
		m_oReadFile.close();

		m_vecErrorReplacer.clear();
		m_vecErrorReplacer.push_back(oFileName.string());
		__LOG_ERROR__(CErrorMsg::GetInstance()->GetMsgString(OPEN_FILE_ERR, m_vecErrorReplacer));

		return OPEN_FILE_ERR;
	}

	m_nProcResult = ReadInfo();
	if (OK != m_nProcResult)
	{
		m_oReadFile.close();

		m_vecErrorReplacer.clear();
		m_vecErrorReplacer.push_back(oFileName.string());
		__LOG_ERROR__(CErrorMsg::GetInstance()->GetMsgString(READ_FILE_ERR, m_vecErrorReplacer));

		return READ_FILE_ERR;
	}

	__LOG_DEBUG__("image height =\t" + to_string(m_oDcmInfo.usImageHeight));
	__LOG_DEBUG__("image width =\t\t" + to_string(m_oDcmInfo.usImageWidth));
	__LOG_DEBUG__("pixel depth =\t\t" + to_string(m_oDcmInfo.usPixelDepth));
	if (m_isPixelDataTagFound && m_oDcmInfo.usImageHeight > 0 && m_oDcmInfo.usImageWidth > 0 && m_oDcmInfo.usPixelDepth > 0)
	{
		ReadImageData(unBuffLen);

		if (m_isDcmTagFound)
		{
			__LOG_DEBUG__("a DICOM 3.0 file");
			m_oDcmInfo.nDicomVersion = Dicom3File;
		}
		else
		{
			__LOG_DEBUG__("an old version of DICOM");
			m_oDcmInfo.nDicomVersion = DicomOldType;
		}
	}
	else
	{
		m_oReadFile.close();
		__LOG_ERROR__(oFileName.string() + " parameters wrong.");
		return READ_FILE_ERR;
	}

	m_oReadFile.close();
	
	__LOG_FUNC_END__;
	
	return OK;
}

/*
 * @brief	read image data
 * @param	pDataBuf: buffer
 * @param	unBuffLen: size of buffer
*/
int CDicomRead::ReadImageData(size_t unBuffLen)
{
	__LOG_FUNC_START__;

	unsigned unNumPixels = m_oDcmInfo.usImageHeight * m_oDcmInfo.usImageWidth;
	unsigned unBytesToRead = unNumPixels * m_oDcmInfo.usPixelDepth / 8 * m_oDcmInfo.usSamplesPerPixel;

	if (unBuffLen < unBytesToRead)
	{
		__LOG_ERROR__(to_string(unBytesToRead) + " bytes required, " + to_string(unBuffLen) + "provided");
		return BUFF_ALLOCATED_SHORT;
	}

	m_oReadFile.seekg(m_oDcmInfo.unDataOffset, ios::beg);
	m_oReadFile.read(m_pDataPtr, unBytesToRead);
	__LOG_DEBUG__(to_string(unBytesToRead) + " bytes loaded");

	if (1 == m_oDcmInfo.usSamplesPerPixel)
	{
		if (8 == m_oDcmInfo.usPixelDepth)
		{
			for (int nIdx = 0; nIdx < unNumPixels; nIdx++)
			{
				m_ucPixelValLower = (unsigned char)(m_pDataPtr);
				m_nPixVal = (int)(m_ucPixelValLower * m_oDcmInfo.fRescaleSlope + m_oDcmInfo.fRescaleIntercept + 0.5);

				if (0 == memcmp("MONOCHROME1", m_oDcmInfo.czPhotoInterpretation, strlen("MONOCHROME1")))
				{
					m_nPixVal = 255 - m_nPixVal;
				}

				*m_pDataPtr++ = m_nPixVal;
			}
		}

		if (16 == m_oDcmInfo.usPixelDepth)
		{
			for (int nIdx = 0; nIdx < unNumPixels; nIdx++)
			{
				if (m_oDcmInfo.usPixelRepresentation)
				{
					// Pixel representation is 1, indicating a 2s complement image
					ReverseCopy(2);
				}

				m_ucPixelValLower = *(unsigned char*)(m_pDataPtr);
				m_ucPixelValHigher = *(unsigned char*)(m_pDataPtr + 1);

				m_nPixVal = m_ucPixelValHigher << 8 | m_ucPixelValLower;

				m_nPixVal = (int)(m_nPixVal * m_oDcmInfo.fRescaleSlope + m_oDcmInfo.fRescaleIntercept + 0.5);

				if (0 == memcmp("MONOCHROME1", m_oDcmInfo.czPhotoInterpretation, strlen("MONOCHROME1")))
				{
					m_nPixVal = 65535 - m_nPixVal;
				}

				if (m_oDcmInfo.usPixelDepth)
				{
					*(unsigned short*)m_pDataPtr = m_nPixVal;
				}
				else
				{
					*(unsigned short*)m_pDataPtr = m_nPixVal+ 32768;
				}

				m_pDataPtr += 2;
			}
		}
	}

	__LOG_FUNC_END__;

	return OK;
}

/*
 * @brief	read dicom info
 * @return	process result
*/
int CDicomRead::ReadInfo()
{
	__LOG_FUNC_START__;

	m_isPixelDataTagFound = false;
	m_oDcmInfo.usPixelDepth = 16;

	m_oReadFile.seekg(ID_OFFSET, ios::beg);
	
	ReadBuf(4);

	// check if the file is before version 3.0
	if (0 == strncmp(m_czStreamBuff, "DICM", 4))
	{
		// version 3.0
		m_isDcmTagFound = true;
	}
	else
	{
		// not Dicom 3.0
		m_unStreamLocation = 0;
		m_oReadFile.seekg(0, ios::beg);

		m_isDcmTagFound = false;
	}

	bool isDecodingTag = true;
	while (isDecodingTag)
	{
		GetNextTag();

		if ((m_unStreamLocation & 1) != 0)
		{
			m_isOddIdx = true;
		}

		if (m_isInSequence)
		{
			continue;
		}

		m_strTag = "";
		switch (m_unTagVal)
		{
		case (int)(TRANSFER_SYNTAX_UID):
			ReadBuf(m_unElementLen);
			::memcpy(&m_strTag, m_czStreamBuff, m_unElementLen);
			AddTag(m_strTag);
			if (m_strTag.find("1.2.4") > -1 || m_strTag.find("1.2.5") > -1)
			{
				m_oDcmInfo.nDicomVersion = DicomUnknow;
				return READ_FILE_ERR;
			}
			if (m_strTag.find("1.2.840.10008.1.2.2") >= 0)
			{
				m_isBigEndianSyntax = true;
			}
			break;
		case (int)MODALITY:
			ReadBuf(m_unElementLen);
			::memcpy(m_oDcmInfo.czModality, m_czStreamBuff, 2);
			AddTag(string(m_oDcmInfo.czModality, 2));
			break;
		case (int)(NUMBER_OF_FRAMES):
			ReadBuf(m_unElementLen);
			break;
		case (int)(SAMPLES_PER_PIXEL):
			m_oDcmInfo.usSamplesPerPixel = GetStreamValue<unsigned short>(2);
			AddTag(string(m_czStreamBuff, 2));
			break;
		case (int)PHOTOMETRIC_INTERPRETATION:
			ReadBuf(m_unElementLen);
			::memcpy(m_oDcmInfo.czPhotoInterpretation, m_czStreamBuff, m_unElementLen);
			AddTag(string(m_czStreamBuff, m_unElementLen));
			break;
		case (int)(PLANAR_CONFIGURATION):
			m_oDcmInfo.usPlanarConfiguration = GetStreamValue<unsigned short>(2);
			AddTag(string(m_czStreamBuff, 2));
			break;
		case (int)ROWS:
			m_oDcmInfo.usImageHeight = GetStreamValue<unsigned short>(2);
			AddTag(string(m_czStreamBuff, 2));
			break;
		case (int)COLUMNS:
			m_oDcmInfo.usImageWidth = GetStreamValue<unsigned short>(2);
			AddTag(string(m_czStreamBuff, 2));
			break;
		case (int)PIXEL_SPACING:
			break;;
		case (int)SLICE_SPACING:
		case (int)SLICE_THICKNESS:
			break;
		case (int)BITS_ALLOCATED:
			m_oDcmInfo.usPixelDepth = GetStreamValue<unsigned short>(2);
			AddTag(string(m_czStreamBuff, 2));
			break;
		case (int)PIXEL_REPRESENTATION:
			m_oDcmInfo.usPixelRepresentation = GetStreamValue<unsigned short>(2);
			AddTag(string(m_czStreamBuff, 2));
			break;
		case (int)WINDOW_CENTER:
			ReadBuf(4);
			m_czStreamBuff[4] = 0;
			m_oDcmInfo.usWinCenter = atoi(m_czStreamBuff);
			AddTag(string(m_czStreamBuff, 4));
			break;
		case (int)WINDOW_WIDTH:
			ReadBuf(4);
			m_czStreamBuff[4] = 0;
			m_oDcmInfo.usWinWidth = atoi(m_czStreamBuff);
			AddTag(string(m_czStreamBuff, 4));
			break;
		case (int)(RESCALE_INTERCEPT):
			break;
		case (int)(RESCALE_SLOPE):
			break;
		case (int)(RED_PALETTE):
			break;
		case (int)(GREEN_PALETTE):
			break;
		case (int)(BLUE_PALETTE):
			break;
		case (int)PIXEL_DATA:
			if (0 != m_unElementLen)
			{
				m_oDcmInfo.unDataOffset = m_unStreamLocation;
				AddTag(to_string(m_unStreamLocation));
				m_isPixelDataTagFound = true;
				isDecodingTag = false;
			}
			break;
		default:
			AddTag("");
			break;
		}
	}

	__LOG_FUNC_END__;

	return OK;
}

/*
 * @brief	little endian to big endian
 * @param	unBufLen: bytes to Reverse
*/
void CDicomRead::ReverseCopy(unsigned int unBufLen)
{
	if (m_oDcmInfo.isBigEndian)
	{
		return;
	}

	// swap bytes front and rear, if unBufLen is odd, the middle one stays where it is
	for (m_nStreamIdx = 0; m_nStreamIdx < unBufLen / 2; m_nStreamIdx++)
	{
		swap(m_czStreamBuff[m_nStreamIdx], m_czStreamBuff[unBufLen - m_nStreamIdx - 1]);
	}
}
