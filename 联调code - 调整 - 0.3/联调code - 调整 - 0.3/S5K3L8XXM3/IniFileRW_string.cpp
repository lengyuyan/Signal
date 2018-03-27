/////////////////////////////////////////////////////////////
// All Rights Reserved by DOTHINKEY
// Last Update:2010 01 20
/////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "IniFileRW.h"
#include "imagekit.h"
#include "diskinfo.h"

#include <algorithm>

using namespace std;

string m_sIniFile="";
int m_nIniCurDev = 0;



//////////////////////////////////////////////////////////////////////////
//获得当前exe路径，返回值是完整路径
//
//
//////////////////////////////////////////////////////////////////////////
string sIniPathName()
{
	char sFilename[_MAX_PATH];
	char sDrive[_MAX_DRIVE];
	char sDir[_MAX_DIR];
	char sFname[_MAX_FNAME];
	char sExt[_MAX_EXT];
	GetModuleFileName(AfxGetInstanceHandle(), sFilename, _MAX_PATH);
	_tsplitpath_s(sFilename, sDrive, sDir, sFname, sExt);

	string rVal(string(sDrive) + string(sDir));
	
	if ( rVal.at(rVal.length()-1) != _T('\\') )
		rVal += _T("\\"); 

	return rVal;
}

//////////////////////////////////////////////////////////////////////////
//返回当前ini文件名，如果不存在，则是默认的hsset.ini文件
//////////////////////////////////////////////////////////////////////////
string sIniFileName()
{
	if(m_sIniFile.length()==0)
	{
		if(m_nIniCurDev <= 0)
		{
			m_sIniFile = sIniPathName();
			m_sIniFile.append("hsset.ini")
		}
		else
		{
			char temp[10] = {0};
			sprintf_s(temp,"%d",m_nIniCurDev + 1);
			m_sIniFile = sIniPathName();
			m_sIniFile.append("hsset_");

			m_sIniFile.append(".ini");

		}
		//AfxMessageBox(m_sIniFile);
	}
	
    return m_sIniFile;
}

//////////////////////////////////////////////////////////////////////////
//设置当前ini文件，不含路径，也就是ini文件一定是当前exe所在路径下面
//
//////////////////////////////////////////////////////////////////////////
void SetIniFileNameInExePath(string sFileName)
{
	m_sIniFile = sIniPathName() + sFileName;
}

//////////////////////////////////////////////////////////////////////////
//设置包含路径的ini文件名，sFileName必须含有路径名
//
//////////////////////////////////////////////////////////////////////////
void SetIniFileName(string sFileName)
{
	m_sIniFile = sFileName;
}

//////////////////////////////////////////////////////////////////////////
//当前文件名清空
//////////////////////////////////////////////////////////////////////////
//
void ClearIniFileName()
{
	m_sIniFile.Empty();
}

//////////////////////////////////////////////////////////////////////////
//写数据到ini文件
//////////////////////////////////////////////////////////////////////////
BOOL WriteIniData(string sSection, string sSectionKey, int nValue)
{
	string  sValue;
	char temp[20] = {0};
	sprintf_s(temp,"%d",nValue);
	sValue(temp);
	//sValue.Format("%d",nValue);
	return WritePrivateProfileString(sSection.c_str(),sSectionKey.c_str(),sValue.c_str(),sIniFileName().c_str());
}

//////////////////////////////////////////////////////////////////////////
//从ini文件读数据
//////////////////////////////////////////////////////////////////////////
int ReadIniData(string sSection,string sSectionKey,int nDefault)
{
    return GetPrivateProfileInt(sSection.c_str(),sSectionKey.c_str(),nDefault,sIniFileName().c_str());
}

//////////////////////////////////////////////////////////////////////////
//写字符串到ini文件
//////////////////////////////////////////////////////////////////////////
BOOL WriteIniString(string sSection, string sSectionKey, string sValue)
{
	return WritePrivateProfileString(sSection.c_str(),sSectionKey.c_str(),sValue.c_str(),sIniFileName().c_str());
}

//////////////////////////////////////////////////////////////////////////
//从ini文件读字符串
//////////////////////////////////////////////////////////////////////////
string ReadIniString(string sSection,string sSectionKey,string sDefault)
{
	string sTemp;
	GetPrivateProfileString(sSection.c_str(), sSectionKey.c_str(), sDefault.c_str(), sTemp.GetBuffer(MAX_PATH), MAX_PATH, sIniFileName().c_str());
	sTemp.TrimLeft();
	sTemp.TrimRight();
	sTemp.ReleaseBuffer();
	return sTemp;
}

//////////////////////////////////////////////////////////////////////////
//从ini文件读十六进制数据
//////////////////////////////////////////////////////////////////////////
long ReadIniDataHex(string sSection,string sSectionKey,long nDefault)
{
    
	char temp[MAX_PATH] = {0};
	GetPrivateProfileString(sSection.c_str(),sSectionKey.c_str(),"", temp, MAX_PATH, sIniFileName().c_str());
	long x = (!strlen(temp)) ? nDefault : strtoul( temp, NULL, 16); 
	return x;
}

//////////////////////////////////////////////////////////////////////////
//写十六进制数据到文件
//////////////////////////////////////////////////////////////////////////
BOOL WriteIniDataHex(string sSection, string sSectionKey, long nValue)
{
	string sTmp;
	char temp[20]
	sprintf_s(temp,"%d",nValue);
	sTmp(temp);
	//sTmp.Format("%x", nValue);
	return WritePrivateProfileString(sSection.c_str(), sSectionKey.c_str(), sTmp.c_str(), sIniFileName().c_str());
}

UINT GetParaFromFile(string sFile, string sSection, UINT *pBuf)
{
	CStdioFile file;
	if (!file.Open((sFile), CFile::modeRead | CFile::typeText))
	{
		return FALSE;
	}
	string szLine ;
	string sReg, sVal;
	UINT reg, value;
	int tmp = 0;
	BOOL bRet = 0;

	string dst;
	transform(sSection.begin(), sSection.end(), dst.begin(), ::tolower);
	sSection = dst;

	sSection.TrimLeft();
	sSection.TrimRight();	

	UINT* pData = pBuf;
	UINT ParaListSize = 0;
	BOOL bStart = 0;
	ULONGLONG fileLen = file.GetLength();
	while(1)
	{
		if(fileLen < file.GetPosition())
		{
			break;
		}

		bRet = file.ReadString(szLine);
		if(!bRet)
		{
			break;
		}

		//寻找注释符号或者']',如果有这样的，只取前面的，
		tmp = szLine.FindOneOf("//"); 
		if( tmp == 0)
		{
			continue;
		}
		else if(tmp > 0)
		{
			szLine = szLine.Left(tmp);
		}
		tmp = szLine.FindOneOf("]"); 
		if( tmp == 0)
		{
			continue;
		}
		else if(tmp > 0)
		{
			szLine = szLine.Left(tmp+1);
		}
		szLine.MakeLower();
		szLine.TrimLeft();
		szLine.TrimRight();		

		if(!bStart)
		{
			if(strcmp(szLine.c_str(), sSection.c_str())) 
			{
				continue;
			}
			bStart =  1;
			continue;
		}

		if(szLine.IsEmpty())
			continue;
		else if(szLine.Left(1) == ",")
			continue;
		else if(szLine.Left(1) == ";")
			continue;
		else if(szLine.Left(1) == "/")
			continue;

		else if(szLine.Left(1) == "[")
		{
			break;
		}
		//	AfxMessageBox(szLine);
		AfxExtractSubString(sReg, szLine, 0, ',');
		AfxExtractSubString(sVal, szLine, 1, ',');
		sReg.TrimLeft();   
		sReg.TrimRight();
		sVal.TrimLeft();
		sVal.TrimRight();  

		if(!sscanf_s(sReg.c_str(), "0x%x", &reg)) //读取键值对数据	
			sscanf_s(sReg.c_str(), "%d", &reg);

		if(!sscanf_s(sVal.c_str(), "0x%x", &value)) //读取键值对数据	
			sscanf_s(sVal.c_str(), "%d", &value);

		*pData = reg;
		pData++;
		*pData = value;
		pData++;
		ParaListSize += 2;	
		szLine.Empty();
	}
	file.Close();
	return ParaListSize;
}

void MipiRaw10ToP8Raw(BYTE *pIn, USHORT DesW, USHORT DesH)
{
	BYTE *pTemp = pIn;
	BYTE *Pout = pIn;
	USHORT SrcW = DesW*5/4;
	for(USHORT j = 0 ; j < DesH; j++)
	{
		for(USHORT i = 0 ; i <  SrcW; i+=5)
		{
			*Pout++ = pTemp[i];
			*Pout++ = pTemp[i+1];
			*Pout++ = pTemp[i+2];
			*Pout++ = pTemp[i+3];
		}
		pTemp += SrcW;
	}
}

void MipiRaw12ToP8Raw(BYTE *pIn, USHORT DesW, USHORT DesH)
{
	BYTE *Pout = pIn;
	BYTE *pTemp = pIn;
	USHORT SrcW = DesW*3/2;

	for(USHORT j = 0 ; j < DesH; j++)
	{
		for(USHORT i = 0 ; i <  SrcW; i+=3)
		{
			*Pout++ = pTemp[i];
			*Pout++ = pTemp[i+1];
		}
		pTemp += SrcW;
	}
}
void Raw10toRaw8(BYTE *pIn, long number)
{
	BYTE *pTemp = pIn;
	BYTE *pOut = pIn;

	for (long i = 0; i < number; i = i + 5)
	{
		*pOut++ = pTemp[i];
		*pOut++ = pTemp[i + 1];
		*pOut++ = pTemp[i + 2];
		*pOut++ = pTemp[i + 3];
	}
}

void Raw16toRaw8(BYTE *pIn, long number)
{
	BYTE *pTemp = pIn;
	BYTE *pOut = pIn;
	for (long i = 0; i < number; i = i +2)
	{
		*pOut++ = pTemp[i];
	}
}

//20141128 added by leedo....
void MipiRaw10ToWord(BYTE *pIn,WORD *pOut, long number)
{
	int index = 0; 

	//BYTE0 :P1[9:2]
	//BYTE1 :P2[9:2]
	//BYTE2 :P3[9:2]
	//BYTE3 :P4[9:2]
	//BYTE4 :P4[1:0]P3[1:0]P2[1:0]P1[1:0]

	//little endian
	for (long i = 0; i < number; i = i + 5)
	{
		pOut[index++]  = ((WORD)(pIn[i]<< 8)) | (pIn[i+4] & 0x03 );

		pIn[i+4] >>= 2;
		pOut[index++]  = ((WORD)(pIn[i+1]<< 8) ) | (pIn[i+4] & 0x03);

		pIn[i+4] >>= 2;
		pOut[index++]  = ((WORD)(pIn[i+2]<< 8) ) | (pIn[i+4] & 0x03);

		pIn[i+4] >>= 2;
		pOut[index++]  = ((WORD)(pIn[i+3]<< 8) ) | (pIn[i+4] & 0x03);
	} 
}

void MipiRaw10ToP10(BYTE *pIn,BYTE *pOut, long number)
{
	int index = 0; 

	//BYTE0 :P1[9:2]
	//BYTE1 :P2[9:2]
	//BYTE2 :P3[9:2]
	//BYTE3 :P4[9:2]
	//BYTE4 :P4[1:0]P3[1:0]P2[1:0]P1[1:0]

	//little endian
	for (long i = 0; i < number; i = i + 5)
	{
		pOut[index++]  = ((pIn[i  ]<< 2) & 0xfc) | (pIn[i+4] & 0x03 );
		pOut[index++]  = ( pIn[i  ]>> 6) & 0x03;

		pIn[i+4] >>= 2;
		pOut[index++]  = ((pIn[i+1]<< 2) & 0xfc) | (pIn[i+4] & 0x03);
		pOut[index++]  = ( pIn[i+1]>> 6) & 0x03;

		pIn[i+4] >>= 2;
		pOut[index++]  = ((pIn[i+2]<< 2) & 0xfc) | (pIn[i+4] & 0x03);
		pOut[index++]  = ( pIn[i+2]>> 6) & 0x03;

		pIn[i+4] >>= 2;
		pOut[index++]  = ((pIn[i+3]<< 2) & 0xfc) | (pIn[i+4] & 0x03);
		pOut[index++]  = ( pIn[i+3]>> 6) & 0x03;
	} 
}

void Raw12toRaw8(BYTE *pIn, long number)
{

	BYTE *pTemp = pIn;
	BYTE *pOut = pIn;

	for (long i = 0; i < number; i = i +3)
	{
		*pOut++ = pTemp[i];
		*pOut++ = pTemp[i + 1];
	}
}


//宽度为4的整数倍
USHORT GetTrueSizeOfPixel(USHORT SizeInPixel, BYTE SensorType)
{
	USHORT SizeInBytes = SensorType == D_MIPI_RAW10 ? (SizeInPixel *5 /4) /20 *20 :
		SensorType == D_RAW10 ? (SizeInPixel *5 /4) /20 *20 :
		SensorType == D_MIPI_RAW12 ? (SizeInPixel *3 /2) /12 *12 : 
		SensorType == D_MIPI_RAW8 ? SizeInPixel/4*4 : SizeInPixel/2*4;	
return SizeInBytes;
}

//宽度为2的整数倍
USHORT GetTrueSizeOfPixel2(USHORT SizeInPixel, BYTE SensorType)
{
	USHORT SizeInBytes = SensorType == D_MIPI_RAW10 ? (SizeInPixel *5 /4) /10 *10 :
		SensorType == D_RAW10 ? (SizeInPixel *5 /4) /10 *10 :
		SensorType == D_MIPI_RAW12 ? (SizeInPixel *3 /2) /6 *6 : 
		SensorType == D_MIPI_RAW8 ? SizeInPixel/2*2 : SizeInPixel/2*4;	
return SizeInBytes;
}

USHORT GetPixelSizeOfData(USHORT SizeInBytes, BYTE SensorType)
{
	USHORT SizeInPixels;
	switch(SensorType)
	{
	case D_MIPI_RAW10:
	case D_RAW10:	
		SizeInPixels = SizeInBytes * 4 /5;
		break;
	case D_MIPI_RAW12:
		SizeInPixels = SizeInBytes * 2 /3;
		break;
	case D_YUV:
	case D_YUV_MTK_S:
	case D_YUV_SPI:
	case D_RGB565:
		SizeInPixels = SizeInBytes / 2; //yuv
		break;
	case D_RAW8:
	default:
		SizeInPixels = SizeInBytes;  //8bit
		break;
	}
	return SizeInPixels;
}

void ResetExpGain(pExp_Gain_Tab pTab)
{
	pTab->ExpRegNum = 0;
	pTab->GainRegNum = 0;
	memset(pTab->ExpReg, EXP_GAIN_DATA_EMPTY, sizeof(pTab->ExpReg));
	memset(pTab->GainReg, EXP_GAIN_DATA_EMPTY, sizeof(pTab->GainReg));
	memset(pTab->Exp_Range, EXP_GAIN_DATA_EMPTY, sizeof(pTab->Exp_Range));
	memset(pTab->Gain_Range, EXP_GAIN_DATA_EMPTY, sizeof(pTab->Gain_Range));
	memset(pTab->Func_ExpGain, EXP_GAIN_DATA_EMPTY, sizeof(pTab->Func_ExpGain));
}


UINT WriteSensorCurrentExpGainToIni(string sSensorName, pExp_Gain_Tab pTab, int State)
{
	SetIniFileNameInExePath("dt_exp_gain.ini");
	State = min(State, FUNC_GROUP_MAX - 1);
	string sSectKey;
	string sValue;
	sSectKey.Format("func%d", State);
	sValue.Format("0x%x, 0x%x,  %d, %d, %d", pTab->Func_ExpGain[FUNC_ELEMENT*State],     //exptime
											pTab->Func_ExpGain[FUNC_ELEMENT*State + 1],//analog gain 
											pTab->Func_ExpGain[FUNC_ELEMENT*State + 2],//wb digital Red gain 
											pTab->Func_ExpGain[FUNC_ELEMENT*State + 3],//wb digital Green gain
											pTab->Func_ExpGain[FUNC_ELEMENT*State + 4]);//wb digital Blue Gain
	WriteIniString(sSensorName, sSectKey, sValue);

	ClearIniFileName();
	return 0;
}

UINT LoadSensorCurrentExpGainFromIni(string sSensorName, pExp_Gain_Tab pTab, int State)
{
	SetIniFileNameInExePath("dt_exp_gain.ini");
	State = min(State, FUNC_GROUP_MAX-1);
	string sSectKey;
	sSectKey.Format("func%d", State);
	ReadSensorExpGainIniSection(sSensorName, sSectKey, pTab->Func_ExpGain + FUNC_ELEMENT*State, FUNC_ELEMENT);
	ClearIniFileName();
	return 0;
}

UINT LoadSensorExpGainFromIni(string sSensorName, pExp_Gain_Tab pTab)
{
	SetIniFileNameInExePath("dt_exp_gain.ini");

	int i;
	string stmp;
	
	pTab->ExpRegNum = ReadSensorExpGainIniSection(sSensorName, "exp_reg", pTab->ExpReg, sizeof(pTab->ExpReg)/sizeof(int));
	pTab->GainRegNum = ReadSensorExpGainIniSection(sSensorName, "gain_reg", pTab->GainReg, sizeof(pTab->GainReg)/sizeof(int));

	ReadSensorExpGainIniSection(sSensorName, "exp_range", pTab->Exp_Range, sizeof(pTab->Exp_Range)/sizeof(int));
	ReadSensorExpGainIniSection(sSensorName, "gain_range", pTab->Gain_Range, sizeof(pTab->Gain_Range)/sizeof(int));

	for(i = 0; i < FUNC_GROUP_MAX; i++)
	{
		stmp.Format("func%d", i);
		ReadSensorExpGainIniSection(sSensorName, stmp, pTab->Func_ExpGain + FUNC_ELEMENT*i, FUNC_ELEMENT);
	}

	ClearIniFileName();

	return 0;
}

UINT ReadSensorExpGainIniSection(string sSensorName, string sSection, UINT* pBuffer, int MaxNum)
{
	if(pBuffer == NULL)
	{
		return 0;
	}
	string stmp,sSect,sRight;
	string sLine;
	UINT nvalue = 0;

	UINT Count = 0;
	int i;
	sLine = ReadIniString(sSensorName, sSection, "");
	if(!sLine.IsEmpty())
	{
		for(i = 0; i <MaxNum; i++)
		{
			if(AfxExtractSubString(stmp, sLine, i, ','))
			{
				stmp.Trim();
				if(stmp.IsEmpty())
				{
					break;
				}
				stmp.MakeLower();
				if(!sscanf_s(stmp.c_str(), "0x%x", &nvalue)) //读取键值对数据	
					sscanf_s(stmp.c_str(), "%d", &nvalue);
				pBuffer[Count] = nvalue;
				Count++;
			}
			else
			{
				break;
			}
		}
	}
	return Count;
}

void ShowFocusArea(BYTE *pBmp, int width, int height, CPoint AFArea)
{
	int i,j;
	ULONG k;
	//high line
	for(j = AFArea.y - FOCUS_AREA_H-FOCUS_AREA_BORDER; j < AFArea.y - FOCUS_AREA_H+FOCUS_AREA_BORDER; j++)
	{
		i = AFArea.x - FOCUS_AREA_W;
		k = (width* j + i)*3;
		for(; i < AFArea.x+FOCUS_AREA_W; i++)
		{
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
		}
	}
	//low line
	for(j = AFArea.y + FOCUS_AREA_H-FOCUS_AREA_BORDER; j < AFArea.y + FOCUS_AREA_H+FOCUS_AREA_BORDER; j++)
	{
		i = AFArea.x - FOCUS_AREA_W;
		k = (width* j + i)*3;
		for(; i < AFArea.x + FOCUS_AREA_W; i++)
		{
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
		}
	}

	//left line
	for(j = AFArea.y - FOCUS_AREA_H; j < AFArea.y + FOCUS_AREA_H; j++)
	{
		i = AFArea.x - FOCUS_AREA_W-FOCUS_AREA_BORDER;
		k = (width* j + i)*3;
		for(; i < AFArea.x - FOCUS_AREA_W+FOCUS_AREA_BORDER; i++)
		{
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
		}
	}
	//right
	for(j = AFArea.y - FOCUS_AREA_H; j < AFArea.y + FOCUS_AREA_H; j++)
	{
		i = AFArea.x + FOCUS_AREA_W-FOCUS_AREA_BORDER;
		k = (width* j + i)*3;
		for(; i < AFArea.x + FOCUS_AREA_W+FOCUS_AREA_BORDER; i++)
		{
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
			pBmp[k++] = 0xff;
		}
	}
}

//20141127 added...
void CheckDiskInfo()
{
	SetIniFileNameInExePath("enum.ini");

	string sOldHardDisk = ReadIniString("Serial No", "SN", "");
	sOldHardDisk.Trim();
	

	char sCurHardDisk[_MAX_PATH] = {0};
	string temp;
	//if(!GetHardIDSerialNo(sCurHardDisk))
	if(!GetMacAddress(sCurHardDisk))
	{
		sCurHardDisk.empty();
	}
	if(sOldHardDisk. != temp(sCurHardDisk))
	{
		CFileFind find;
		if(find.FindFile(sIniFileName())) //查找并删除文件  
		{
			find.Close();
			try
			{
				DeleteFile(sIniFileName().c_str());
			}
			catch (CFileException* e)
			{
				e = e;//only for not warings
			}
		}

		WriteIniString("Serial No", "SN", sCurHardDisk);
	}
	ClearIniFileName();
}

void SetIniCurrentDeviceID(int DevID)
{
	m_nIniCurDev = DevID;
	m_sIniFile.empty();
}