#ifndef INIFILERW_H
#define INIFILERW_H

#include <string>
using namespace std;

//测试数据库用，关闭收藏夹
#define _SEARCH_DOTHINKEY_FAVORITES_
#define GETYFROMBGR(R, G, B) ((1225*(R) + 2404*(G) + 467*(B) + 2048)>>12)

//如果需要ROI，请使用这个定义。。
//#define ROI_OUTPUT_ENABLE
#define ROI_OUTPUT_WIDTH   352
#define ROI_OUTPUT_HEIGHT  288

#define FOCUS_AREA_W 160
#define FOCUS_AREA_H 120
#define FOCUS_AREA_BORDER 3

#define EXP_GAIN_DATA_EMPTY 0xffffffff
#define FUNC_GROUP_MAX 20
#define FUNC_ELEMENT 5

enum RunState
{
	RUNSTATE_NORMAL = 0,
	RUNSTATE_ROI_B0,
	RUNSTATE_QUICK,
	RUNSTATE_FULL,
	RUNSTATE_AF,
	RUNSTATE_FAR,
	RUNSTATE_NEAR,
};

typedef struct _Exp_Gain_Tab
{
	UINT Exp_Range[2];
	UINT Gain_Range[2];

	UINT ExpReg[4];
	UINT GainReg[4];
	UINT ExpRegNum;
	UINT GainRegNum;

	UINT Func_ExpGain[FUNC_ELEMENT * FUNC_GROUP_MAX];
	_Exp_Gain_Tab()
	{
		ExpRegNum = 0;
		GainRegNum = 0;
		memset(ExpReg, EXP_GAIN_DATA_EMPTY, sizeof(ExpReg));
		memset(GainReg, EXP_GAIN_DATA_EMPTY, sizeof(GainReg));
		memset(Exp_Range, EXP_GAIN_DATA_EMPTY, sizeof(Exp_Range));
		memset(Gain_Range, EXP_GAIN_DATA_EMPTY, sizeof(Gain_Range));
		memset(Func_ExpGain, EXP_GAIN_DATA_EMPTY, sizeof(Func_ExpGain));
		
	}
}Exp_Gain_Tab, *pExp_Gain_Tab;

//
void ResetExpGain(pExp_Gain_Tab pTab);

//Get ini file name.
//获得当前exe所在路径
string sIniPathName();
//获得当前ini文件的名字，包括路径。
string sIniFileName();

//设置当前路径下的ini文件名字
//sFileName不要再有路径了！
void SetIniFileNameInExePath(string sFileName);
//要包含路径！
void SetIniFileName(string sFileName);

//清空当前ini的文件名
void ClearIniFileName();

//20100907 added
long ReadIniDataHex(string sSection,string sSectionKey,long nDefault);
BOOL WriteIniDataHex(string sSection, string sSectionKey, long nValue);

//读数字
int ReadIniData(string sSection,string sSectionKey,int nDefault);
//写数字
BOOL WriteIniData(string sSection, string sSectionKey, int nValue);
//-------------------------------------
//读字符串
string ReadIniString(string sSection,string sSectionKey,string sDefault);
//写字符串
BOOL WriteIniString(string sSection, string sSectionKey, string sValue);


//20141109 added...
UINT GetParaFromFile(string sFile, string sSection, UINT *pBuf);

void Raw10toRaw8(BYTE *pIn, long number);
void Raw12toRaw8(BYTE *pIn, long number);
void Raw16toRaw8(BYTE *pIn, long number);
void MipiRaw10ToP10(BYTE *pIn,BYTE *pOut, long number);
void MipiRaw10ToWord(BYTE *pIn,WORD *pOut, long number);  //20141128 added by leedo...


void MipiRaw10ToP8Raw(BYTE *pIn, USHORT DesW, USHORT DesH);
void MipiRaw12ToP8Raw(BYTE *pIn, USHORT DesW, USHORT DesH);

//宽度为4的整数倍
USHORT GetTrueSizeOfPixel(USHORT SizeInPixel, BYTE SensorType);
//宽度为2的整数倍
USHORT GetTrueSizeOfPixel2(USHORT SizeInPixel, BYTE SensorType);
USHORT GetPixelSizeOfData(USHORT SizeInBytes, BYTE SensorType);

UINT LoadSensorExpGainFromIni(string sSensorName, pExp_Gain_Tab pTab);
UINT ReadSensorExpGainIniSection(string sSensorName, string sSection, UINT* pBuffer, int MaxNum);

UINT LoadSensorCurrentExpGainFromIni(string sSensorName, pExp_Gain_Tab pTab, int State);
UINT SaveSensorCurrentExpGainIni(string sSensorName, pExp_Gain_Tab pTab, int State);

UINT WriteSensorCurrentExpGainToIni(string sSensorName, pExp_Gain_Tab pTab, int State);

void ShowFocusArea(BYTE *pBmp, int width, int height, CPoint AFArea);

//20141127 added by leedo...
void CheckDiskInfo();
//20141203 added by leedo...
void SetIniCurrentDeviceID(int DevID);

#endif