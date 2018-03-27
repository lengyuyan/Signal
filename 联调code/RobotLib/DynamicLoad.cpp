// To build and execute the sample application, the following additional files are required:
// - PI_GCS2_DLL.h
// - PI_GCS2_DLL.lib
// - PI_GCS2_DLL.dll
// - PI_GCS2_DLL_x64.lib
// - PI_GCS2_DLL_x64.dll
//
// Proceed as follows to make these files available for the application:
// 1) Install the feature PI_Programming_Files_PI_GCS2_DLL_Setup.exe?from the product CD. Afterwards, the required files will be located in 揅:\Users\Public\PI\PI_Programming_Files_PI_GCS2_DLL?
// 2) Copy these files to the location where the source code is built (.h. and .lib) and where the application is executed (.dll).
//
//				: Sample Program to demonstrate usage of Lib file to link dll statically in MS VC++
//					- opening a connection via RS232 or optional TCP/IP  
//					- referencing the hexapod ("FRF")
//					- random movement with X axis of hexapod

#include "stdafx.h"

#define GCS_DLLNAME  "mtGCS.dll"
#define GCS_FUNC_PREFIX "PI_"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
typedef BOOL (WINAPI *FP_ConnectRS232)( int , int);
typedef BOOL (WINAPI *FP_CloseConnection)( int );
typedef BOOL (WINAPI *FP_qIDN)( int , char*, int);
typedef BOOL (WINAPI *FP_STP)( int);
typedef BOOL (WINAPI *FP_GcsCommandset)( int , char*);
typedef BOOL (WINAPI *FP_GcsGetAnswer)(int,char*,int);
typedef BOOL (WINAPI *FP_GcsGetAnswerSize)(int,int*);
typedef BOOL (WINAPI *FP_qERR)(int, int*);
typedef BOOL (WINAPI *FP_IsMoving)(int, const char*,BOOL*);
typedef BOOL (WINAPI *FP_qFRF)(int, const char*,BOOL*);
typedef BOOL (WINAPI *FP_FRF)(int ID, const char* szAxes);
typedef BOOL (WINAPI *FP_qTMX)(int ID, const char* szAxes, double* pdValueArray);
typedef long (WINAPI *FP_ConnectTCPIP)(const char* , long );
typedef long (WINAPI *FP_EnumerateTCPIPDevices)(char* , long , const char* );
typedef BOOL (WINAPI *FP_IsControllerReady)(int ID, int* piControllerReady);
typedef BOOL (WINAPI *FP_SSL) (int , const char* , const BOOL* );
typedef BOOL (WINAPI *FP_qSSL)(int, const char*,BOOL*);
typedef BOOL (WINAPI *FP_NLM )(int , const char*, const double*);
typedef BOOL (WINAPI *FP_qNLM )(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_qPLM )(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_PLM )(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_POS )(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_qPOS )(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_FRF )(int ID, const char* szAxes);
typedef BOOL (WINAPI *FP_RON )(const int ID, const char * szAxes, BOOL * pbValarray);
typedef BOOL (WINAPI *FP_DFH )(int ID, const char* szAxes);
typedef BOOL (WINAPI *FP_qDFH )(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_qTMN )(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_SVO )(int ID, const char* szAxes, const BOOL* pbValueArray);
typedef BOOL (WINAPI *FP_qSVO )(int ID, const char* szAxes, BOOL* pbValueArray);
typedef BOOL (WINAPI *FP_qVEL )(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_VEL )(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_SST )(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_MOV )(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_GOH )(int ID, const char* szAxes);
typedef BOOL (WINAPI *FP_SPI) (int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_qSPI)(int ID, const char* szAxes, double* pdValueArray);
typedef BOOL (WINAPI *FP_MRT)(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_MRW)(int ID, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_KST)(int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_KSW) (int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray);
typedef BOOL (WINAPI *FP_KEN) (int ID, const char* szNameOfCoordSystem);
typedef BOOL (WINAPI *FP_qKEN) (int ID, const char* szNamesOfCoordSystems, char* buffer, int bufsize);



// Function Pointer Variables
FP_ConnectRS232 pConnectRS232;
FP_qIDN pqIDN;
FP_CloseConnection pCloseConnection;
FP_GcsCommandset pGcsCommandset;
FP_GcsGetAnswer pGcsGetAnswer;
FP_GcsGetAnswerSize pGcsGetAnswerSize;
FP_qERR pqERR;
FP_IsMoving pIsMoving;
FP_qFRF	pqFRF;
FP_FRF pFRF;
FP_ConnectTCPIP pConnectTCPIP;
FP_EnumerateTCPIPDevices pEnumerateTCPIPDevices;
FP_qTMX pqTMX;
FP_STP pSTP;
FP_IsControllerReady pIsControllerReady;

FP_SSL pSSL;
FP_qSSL pqSSL;
FP_NLM pNLM;
FP_qNLM pqNLM;
FP_qPLM pqPLM;
FP_PLM pPLM;
FP_POS pPOS;
FP_qPOS pqPOS;
FP_RON pRON;
FP_DFH pDFH;
FP_qDFH pqDFH;
FP_qTMN pqTMN;
FP_SVO pSVO;
FP_qSVO pqSVO;
FP_qVEL pqVEL;
FP_VEL pVEL;
FP_SST pSST;
FP_MOV pMOV;
FP_GOH pGOH;

FP_SPI pSPI;
FP_qSPI pqSPI;
FP_MRT pMRT;
FP_MRW pMRW;
FP_KST pKST;
FP_KSW pKSW;
FP_KEN pKEN;
FP_qKEN pqKEN;

bool IsControllerReady(int iID)
{
	int iAS=0;
// you have to make sure the answer buffer is emptied before calling this function
	pGcsCommandset(iID,"\7");
	while(iAS==0)
	{
		Sleep(50);
		pGcsGetAnswerSize(iID,&iAS);
	}
	char szAns[10];
	pGcsGetAnswer(iID,szAns,9);
	return szAns[0]&1;
	
}


bool MoveTo(int iID,char* szAxis,double dTarget)
{
	BOOL bOK = true;
	char szCommand[50];
	sprintf(szCommand,"MOV %s %g",szAxis,dTarget);
	if(!pGcsCommandset(iID,szCommand))
		return false;
	BOOL bFlag;
	printf("moving...");
	do
	{	printf(".");
		Sleep(100);
		bOK &=pIsMoving(iID,szAxis,&bFlag);
		
		if (_kbhit() )
			return true;

	}while (bFlag);

	sprintf(szCommand,"POS? %s",szAxis);
	if(!pGcsCommandset(iID,szCommand))
		return false;
	int iAS=0;
	while(iAS==0)
	{
		Sleep(50);
		bOK &=pGcsGetAnswerSize(iID,&iAS);
	}

	char szAns[20];
	bOK &= pGcsGetAnswer(iID,szAns,19);
	printf("\nPosition after move: %s\n",szAns);
	return (bOK==TRUE);
}


//动态加载库
void LoadFunctions(HINSTANCE hPI_Dll)
{
	char* szFuncName[150];
	try
	{
		// get function pointers
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "ConnectRS232");
		pConnectRS232 = (FP_ConnectRS232)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pConnectRS232==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qIDN");
		pqIDN = (FP_qIDN)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqIDN==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "CloseConnection");
		pCloseConnection = (FP_CloseConnection)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pCloseConnection==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "GcsCommandset");
		pGcsCommandset = (FP_GcsCommandset)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pGcsCommandset==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "GcsGetAnswer");
		pGcsGetAnswer = (FP_GcsGetAnswer)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pGcsGetAnswer==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "GcsGetAnswerSize");
		pGcsGetAnswerSize = (FP_GcsGetAnswerSize)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pGcsGetAnswerSize==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qERR");
		pqERR = (FP_qERR)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqERR==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "IsMoving");
		pIsMoving = (FP_IsMoving)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pIsMoving==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qFRF");
		pqFRF = (FP_qFRF)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqFRF==NULL)
			throw(-1);

		
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "FRF");
		pFRF = (FP_FRF)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pFRF==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qTMX");
		pqTMX = (FP_qTMX)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqTMX==NULL)
			throw(-1);

		
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "IsControllerReady");
		pIsControllerReady = (FP_IsControllerReady )GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pIsControllerReady==NULL)
			throw(-1);

		

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "STP");
		pSTP = (FP_STP)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pSTP==NULL)
			throw(-1);


		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "ConnectTCPIP");
		pConnectTCPIP = (FP_ConnectTCPIP)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pConnectTCPIP==NULL)
			throw(-1);
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "EnumerateTCPIPDevices");
		pEnumerateTCPIPDevices = (FP_EnumerateTCPIPDevices)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pEnumerateTCPIPDevices==NULL)
			throw(-1);


		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "SSL");
		pSSL = (FP_SSL)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pSSL==NULL)
			throw(-1);
			
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qSSL");
		pqSSL = (FP_qSSL)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqSSL==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "NLM");
		pNLM = (FP_NLM)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pNLM==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qNLM");
		pqNLM = (FP_qNLM)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqNLM==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qPLM");
		pqPLM = (FP_qPLM)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqPLM==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "PLM");
		pPLM = (FP_PLM)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pPLM==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "POS");
		pPOS = (FP_POS)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pPOS==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qPOS");
		pqPOS = (FP_qPOS)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqPOS==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "RON");
		pRON = (FP_RON)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pRON==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "DFH");
		pDFH = (FP_DFH)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pDFH==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qDFH");
		pqDFH = (FP_qDFH)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqDFH==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qTMN");
		pqTMN = (FP_qTMN)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqTMN==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "SVO");
		pSVO = (FP_SVO)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pSVO==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qSVO");
		pqSVO = (FP_qSVO)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqSVO==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "VEL");
		pVEL = (FP_VEL)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pVEL==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qVEL");
		pqVEL = (FP_qVEL)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqVEL==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "SST");
		pSST = (FP_SST)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pSST==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "MOV");
		pMOV = (FP_MOV)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pMOV==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "GOH");
		pGOH = (FP_GOH)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pGOH==NULL)
			throw(-1);	


	   sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "SPI");
		pSPI = (FP_SPI)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pSPI==NULL)
			throw(-1);

		 sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qSPI");
		pqSPI = (FP_qSPI)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqSPI==NULL)
			throw(-1);

	
		 sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "MRT");
		pMRT = (FP_MRT)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pMRT==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "MRW");
		pMRW = (FP_MRW)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pMRW==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "KST");
		pKST = (FP_KST)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pKST==NULL)
			throw(-1);

		
		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "KSW");
		pKSW = (FP_KSW)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pKSW==NULL)
			throw(-1);



		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "KEN");
		pKEN = (FP_KEN)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pKEN==NULL)
			throw(-1);

		sprintf((char*)szFuncName,"%s%s",GCS_FUNC_PREFIX , "qKEN");
		pqKEN = (FP_qKEN)GetProcAddress(hPI_Dll,(LPCSTR)szFuncName);
		if(pqKEN==NULL)
			throw(-1);

		


	}
	catch(...)
	{
		char* msg = new char[100];
		sprintf(msg,"Loading %s failed",szFuncName);
		throw(msg);
	}
}


//初始化
int init()
{
	HINSTANCE hPI_Dll = LoadLibraryA(GCS_DLLNAME);
	if(hPI_Dll==NULL)
	{
		printf("LoadLibrary falied\n");
		return -1;
	}
	printf("LoadLibrary(\"%s\") successfull\n",GCS_DLLNAME);
	try
	{
			LoadFunctions(hPI_Dll);
			printf("All function pointers are loaded\n");
	}
	catch(...)
	{
		char* msg = new char[100];


		printf("Loading function  failed");
		throw(msg);
	}
	
	return 0;
}

//引用
bool ReferenceStage(int iID,char* szAxis)
{
	BOOL bReferenced;
	if (!pqFRF(iID, szAxis, &bReferenced))
		return FALSE;
	if (!bReferenced) {
		// if needed, reference the axis using the refence switch
		printf("Referencing axis %s...\n", szAxis);
		if (!pFRF(iID, szAxis)) return FALSE;
		// Wait until the reference move is done.
		BOOL bFlag = false;
		while (bFlag != TRUE) {
			if (!pIsControllerReady(iID, &bFlag)) return FALSE;
		}
	}

	return TRUE;
}

//机器人连接
int  ConnectRobot(int iComPort, int iBaudRate)
{
	//int iComPort = 4; //COM口
	//int iBaudRate = 115200;  //波特率
	int ID;
	ID=  pConnectRS232(iComPort,iBaudRate);
	if (ID<0)
	{
		printf("Connect the Robot failed. Exiting.\n");
		return -1;
	}

	char szIDN[2000];
	if(pqIDN(ID,szIDN,199) == FALSE)
	{
		printf("qIDN failed. Exiting.\n");
		return -1;
	}
	return ID;
}

//设置对应控制器对应轴的软限位范围的最低最高值,LowOrHigh的值:high=1,low=0
BOOL sSoftLimit(int ID, const char* axisArr, const double* SoftLimitLowEnd, int LowOrHigh)
{
	BOOL bFlag;
	int iTimeout = 100;
	do
	{// wait for motion to stop
		pIsMoving(ID, axisArr, &bFlag);
		bFlag &= ((iTimeout--) > 0);
		Sleep(20);
	} while (bFlag);

	BOOL Soft_Limit = TRUE;
	BOOL Soft_Limit_State;
	if (pSSL(ID, axisArr, &Soft_Limit) == FALSE || pqSSL(ID, axisArr, &Soft_Limit_State) == FALSE) {
		printf("set or get state failed.\n");
		return FALSE;
	}
	if (!(Soft_Limit_State == TRUE)) {
		printf("Activates states wrong.\n");
		return FALSE;
	}
	else {
		if (LowOrHigh == 0) {
			if (pNLM(ID, axisArr, SoftLimitLowEnd) == FALSE) {
				printf("Set low limit failed.\n");
				return FALSE;
			}
		}
		else if (LowOrHigh == 1) {
			if (pPLM(ID, axisArr, SoftLimitLowEnd) == FALSE) {
				printf("Set high limit failed.\n");
				return FALSE;
			}
		}
		else {
			printf("LowOrHigh value error.\n");
			return FALSE;
		}
	}

	return TRUE;
}

// 获取对应控制器对应轴的软限位范围的最低最高值,LowOrHigh的值:high=1,low=0
BOOL gSoftLimit(int ID, const char* axisArr, double* SoftLowLimit, int LowOrHigh)
{
	if (LowOrHigh == 0) {
		if (!pqNLM(ID, axisArr, SoftLowLimit)) {
			printf("Get the limit failed\n");
			return FALSE;
		}
	}
	else if (LowOrHigh == 1) {
		if (!pqPLM(ID, axisArr, SoftLowLimit)) {
			printf("Get the limit failed\n");
			return FALSE;
		}
	}
	else {
		printf("LowOrHigh value error.\n");
		return FALSE;
	}

	return TRUE;
}

// 获取当前坐标
BOOL gCurrentPosition(int ID, const char* axisArr, double* posArr)
{
	if (!pqPOS(ID, axisArr, posArr)) {
		printf("get current position failed\n");
		return FALSE;
	}

	return TRUE;
}


//设置当前坐标
BOOL sCurrentPosition(int ID, const char* axisArr,const double* posArr)
{
	int ref;
	if (!pqFRF(ID, axisArr, &ref) || ref != TRUE) {
		printf("axis not referenced\n");
		return FALSE;
	}
	int mode;
	if (!pRON(ID, axisArr, &mode) || mode != 0) {
		printf("referencing mode error\n");
		return FALSE;
	}
	if (!pPOS(ID, axisArr, posArr)) {
		printf("set current position failed\n");
		return FALSE;
	}

	return TRUE;
}

//设置轴心点坐标
BOOL sPivotPointCoordinates(int ID, const char* axisArr,const double* Cordinate)
{
	if (!pSPI(ID, axisArr,Cordinate)) {
		printf("set PivotPoint failed\n");
		return FALSE;
	}

	return TRUE;
}

// 获取轴心点坐标
BOOL gPivotPointCoordinates(int ID, const char* axisArr, double* posArr)
{
	if (!pqSPI(ID, axisArr, posArr)) {
		printf("get position failed\n");
		return FALSE;
	}

	return TRUE;
}

//获取当前硬件限位范围：
BOOL gEndTravelRange(int ID, const char* axisArr, double* rangeArr, int LowOrHigh)
{
	if (LowOrHigh == 0) {
		if (!pqTMN(ID, axisArr, rangeArr)) {
			printf("Get travel range failed\n");
			return FALSE;
		}
	}
	else if (LowOrHigh == 1) {
		if (!pqTMX(ID, axisArr, rangeArr)) {
			printf("Get travel range failed\n");
			return FALSE;
		}
	}
	else {
		printf("LowOrHigh value error.\n");
		return FALSE;
	}

	return TRUE;
}

//设置速度
BOOL sAxisVelocity(int ID, const char* axisArr, const double* speedArr)
{
	//close为1, open为0
	int CloseOrOpen;
	if (!pqSVO(ID, axisArr, &CloseOrOpen)) {
		printf("Get servo-control mode failed\n");
		return FALSE;
	}
	if(CloseOrOpen == 1)
	{
		printf("serve off");
		return FALSE;
	}

	if (CloseOrOpen == 1) {
		if (!pVEL(ID, axisArr, speedArr)) {
			printf("Set axis velocity failed\n");
			return FALSE;
		}
	}
	else {
		printf("Not in Close-loop Or Open-loop mode\n");
		return FALSE;
	}

	return TRUE;

}



//获取当速度
 BOOL gCurrentVelocity(int ID, const char* axisArr, double* speedArr)
 {
	 //close为1, open为0
	int CloseOrOpen;
	if (!pqSVO(ID, axisArr, &CloseOrOpen)) {
		printf("Get servo-control mode failed\n");
		return FALSE;
	}

	if (CloseOrOpen == 1) {
		if (!pqVEL(ID, axisArr, speedArr)) {
			printf("Get axis velocity failed\n");
			return FALSE;
		}
	}
	else {
		printf("Not in Close-loop Or Open-loop mode\n");
		return FALSE;
	}

	return TRUE;
 }

// 设置一次步进的距离
 BOOL sStepSizeDistance(int ID, const char* axisArr, double* stepArr)
 {
	 if (!pSST(ID, axisArr, stepArr)) {
		printf("Set Step Distance failed\n");
		return FALSE;
	}

	return TRUE;
 }

// 走到一个目标点
BOOL MoveToTarget(int ID, const char* axisArr, const double* targetArr)
{
	if (!pMOV(ID, axisArr, targetArr)) {
		printf("Move failed\n");
		return FALSE;
	}
	// Wait until the closed loop move is done.
	BOOL bIsMoving = TRUE;
	double dPos;
	while (bIsMoving == TRUE)
	{
		if(!pqPOS(ID, axisArr, &dPos))
			return FALSE;
		if(!pIsMoving(ID, axisArr, &bIsMoving))
			return FALSE;
		//printf("Current position(%s): %g\n",axis,dPos);
	}
	double posArr[10];
	pqPOS(ID, axisArr, posArr);
	return TRUE;
}

// 回原点
BOOL GoHomePosition(int ID, const char* axisArr)
{
	if (!pGOH(ID, axisArr)) {
		printf("Go Home failed\n");
		return FALSE;
	}

	return TRUE;
}





//defines a new Tool coordinate system (KST type).
BOOL sDefineToolCoordinate(int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray)
{
	//pKST(ID,szNameOfCoordSystem,szAxes,pdValueArray);

	if(!pKST(ID,szNameOfCoordSystem,szAxes,pdValueArray)) 
	{
		printf("defines a new Tool coordinate system failed\n");
		return FALSE;
	}
		return TRUE;
}


//Move szAxes relative to current position and orientation in Tool coordinate system.
BOOL sMoveRelativeToToolCoordinateSystem (int ID, const char* szAxes, const double* pdValueArray)
{

	if(!pMRT(ID,szAxes, pdValueArray))
	{
		printf("Move szAxes relative to current position and orientation in Tool coordinate system failed\n");
		return FALSE;
	}
		return TRUE;
}

//Move szAxes relative to current position and orientation in Work coordinate system
BOOL sMoveRelativeToWorkCoordinateSystem (int ID, const char* szAxes, const double* pdValueArray)
{

	if(!pMRW(ID,szAxes, pdValueArray))
	{
		printf("Move szAxes relative to current position and orientation in Work coordinate system failed\n");
		return FALSE;
	}
		return TRUE;
}


BOOL sNewWorkCoordinateSystem (int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray)
{
	if(!pKSW(ID,szAxes,szNameOfCoordSystem, pdValueArray))
	{
		printf("Defines a new Work coordinate system failed\n");
		return FALSE;
	}
		return TRUE;
}


//Enables an already defined coordinate system.
BOOL sEnabledCoordinateSystems (int ID, const char* EnabledCoordinateName)
{
	if(!pKEN(ID,EnabledCoordinateName))
	{
		printf("Enables an already defined coordinate system failed\n");
		return FALSE;
	}
		return TRUE;
}

//Lists enabled coordinate systems by name.
BOOL gListsEnabledCoordinateSystems(int ID, const char* EnabledCoordinateNameList, char* buffer, int bufsize)
{
	if(!pqKEN(ID,EnabledCoordinateNameList,buffer,bufsize))
	{
		printf("Lists enabled coordinate systems by name failed\n");
		return FALSE;
	}
		return TRUE;
}