/////////////////////////////////////////////////////////////
// All Rights Reserved by DOTHINKEY
// Last Update:2010 05 22
/////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "HSFoxx.h"
#include "HSFoxxDlg.h"
#include "threads.h"
#include <process.h>    //for thread
#include "tvdecode.h"

FrameInfo m_FrameInfo;

 
UINT FrameCnt = 0;
UINT PlayCnt = 0;
void Camera_Thread(void *param)
{
    CHSFoxxDlg *pVideo = (CHSFoxxDlg *)param;
	PlayCnt++;
	UINT timeStart = 0;
	UINT timeStop = 0;
	UINT FrameNum = 0;
	ULONG RetSize = 0;
	
	UINT FrameErrCnt = 0;
	UINT DeviceErrCnt = 0;
	float Framefps = 0.00;
	CString FpsInfo;
	FrameCnt = 0;
	int x = 0;

	int bRet = 0;
	WORD ImgH = 0, ImgV = 0;

	UINT CrcCount;

	GetMipiCrcErrorCount(&CrcCount);
	USHORT OldCrcCount = CrcCount;
	timeStart = GetTickCount();
	while (pVideo->m_RunMode != RUNMODE_STOP)
	{
		if(pVideo->m_RunMode == RUNMODE_PLAY)
		{
			if(pVideo->m_isTV)
			{
				bRet = GrabFrame(pVideo->m_pCameraBuffer, pVideo->m_GrabSize,&RetSize,&m_FrameInfo,pVideo->m_nDevID);
				if(bRet == 1)
				{
					bRet = GrabFrame(pVideo->m_pCameraBuffer+pVideo->m_GrabSize, pVideo->m_GrabSize,&RetSize,&m_FrameInfo,pVideo->m_nDevID);
				}
			}
			else
			{
				bRet = GrabFrame(pVideo->m_pCameraBuffer, pVideo->m_GrabSize,&RetSize,&m_FrameInfo,pVideo->m_nDevID);
			}
			
			if(bRet == DT_ERROR_OK)
			{
				GetMipiCrcErrorCount(&CrcCount,pVideo->m_nDevID);
				bRet = (CrcCount == OldCrcCount) ? DT_ERROR_OK  : DT_ERROR_FAILED;
				OldCrcCount = CrcCount;
			}

			if( bRet == DT_ERROR_OK)
			{
				pVideo->TripleBufferFlip(&pVideo->m_pCameraBuffer, TRUE);

				FrameCnt++;
				
				FrameNum++;

				if(FrameNum % 10 == 0)
				{
					timeStop = GetTickCount() - timeStart;
					timeStart = GetTickCount();
					Framefps = (float)(10000.0/timeStop);
				}
				DeviceErrCnt = 0;
			}
			else if(bRet == DT_ERROR_FAILED)
			{
				FrameErrCnt++;
				if(FrameErrCnt > 300)
				{
					AfxMessageBox(_T("Err image too many!"));
					
					break;
				}
				DeviceErrCnt = 0;
				Sleep(1);
			}
			else
			{
				DeviceErrCnt++;
				if(DeviceErrCnt > 2)
				{
					//AfxMessageBox("Can't grab image!");
					break;
				}
			}
		}
	}
	if(pVideo->m_RunMode == RUNMODE_STOP)
	{
		FrameCnt = 0;
	}
    SetEvent(pVideo->m_hCameraThreadEventDead);
    _endthread();
}

/*
* Display_Thread
* -------------
* function: while 'playing' get latest sensor buffer, apply colerconversion and display 
* in:       pointer to the CSimpleDlg class
* out:
* special:  this function is called and run as a threadread.
*/
void Display_Thread(void *param)
{
	CHSFoxxDlg  *pVideo = (CHSFoxxDlg *)param;
	UINT type = pVideo->CurrentSensor.type;
	UINT width = pVideo->m_PreviewWidth;
	UINT height = pVideo->m_PreviewHeight;
	BYTE* pBmp = (BYTE*)malloc(width*height*4);
    while (pVideo->m_RunMode != RUNMODE_STOP)
    {
		//change display buffer.
		if((pVideo->m_RunMode == RUNMODE_PLAY))
		{
			if (!pVideo->TripleBufferFlip(&pVideo->m_pDisplayBuffer, FALSE))
			{
				Sleep(1);
				continue;
			}
			
			if(FrameCnt)
			{
				if(pVideo->m_isTV)
				{   
					DynamicModule(pVideo->m_pDisplayBuffer, width, height);//¶¯Ì¬µ÷±ä
				}
#ifndef _ONLY_GRAB_IMAGE_
				if(pVideo->m_roi_state)
				{
					ImageProcess(pVideo->m_pDisplayBuffer, pBmp, 640, 480, &m_FrameInfo,pVideo->m_nDevID);
					DisplayRGB24(pBmp, &m_FrameInfo, pVideo->m_nDevID);
				}
				else
				{
					ImageProcess(pVideo->m_pDisplayBuffer, pBmp, width, height, &m_FrameInfo,pVideo->m_nDevID);
					DisplayRGB24(pBmp, &m_FrameInfo,pVideo->m_nDevID);
				}
#endif
			}
		}
		else
		{
			  Sleep(50);
		}
    }
	free(pBmp);
	pBmp = NULL;
    SetEvent(pVideo->m_hDisplayThreadEventDead);
    _endthread();
}