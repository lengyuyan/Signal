/////////////////////////////////////////////////////////////
// All Rights Reserved by DOTHINKEY
// Last Update:2010 05 22
/////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "AAvision.h"

#include "threads.h"
#include <process.h>    //for thread
#include "tvdecode.h"

#include "dtccm2.h" 

#include "AA_paint.h"
#include "AA_define_tool.h"

//FrameInfo m_FrameInfoA;
//FrameInfo m_FrameInfoB;


extern  AA_Globle_Param g_AAParam;//引入全局变量
 
//UINT AAFrameCnt = 0;
//UINT AAPlayCnt = 0;
void Camera_ThreadA(void *param)
{

	
    CAAVision *pVideo = (CAAVision *)param;
	pVideo->AAPlayCnt++;
	UINT timeStart = 0;
	UINT timeStop = 0;
	UINT FrameNum = 0;
	ULONG RetSize = 0;
	
	UINT DeviceErrCnt = 0;
	CString FpsInfo;
	int x = 0;

	int bRet = 0;
	WORD ImgH = 0, ImgV = 0;

	UINT CrcCount;
	
	GetMipiCrcErrorCount(&CrcCount,pVideo->m_iDevIDA);
	USHORT OldCrcCount = CrcCount;
	timeStart = GetTickCount();
	while (pVideo->m_iWorkStatusA != TESTOR_STOP)
	{
		if(pVideo->m_iWorkStatusA == TESTOR_START)
		{
			bRet = GrabFrame(pVideo->m_pCameraBufferA, pVideo->m_GrabSizeA,&RetSize,&(pVideo->m_FrameInfoA),pVideo->m_iDevIDA);
			
			/*if(bRet == DT_ERROR_OK)
			{
				GetMipiCrcErrorCount(&CrcCount,pVideo->m_iDevIDA);
				bRet = (CrcCount == OldCrcCount) ? DT_ERROR_OK  : DT_ERROR_FAILED;
				OldCrcCount = CrcCount;
			}*/

			if( bRet == DT_ERROR_OK)
			{
				pVideo->TripleBufferFlipA(&pVideo->m_pCameraBufferA, TRUE);

				pVideo->AAFrameCnt++;

				pVideo->m_iFrameCntA++; 

				if(pVideo->m_iFrameCntA % 10 == 0)
				{
					timeStop = GetTickCount() - timeStart;
					timeStart = GetTickCount();
					pVideo->m_fFrameFpsA = (float)(10000.0/timeStop);//这里时间是毫秒，需要转换为秒。
				}
				DeviceErrCnt = 0;
			}
			else if(bRet == DT_ERROR_FAILED)
			{
				pVideo->m_iFrameErrA++;
				if(pVideo->m_iFrameErrA > 300)
				{
					AfxMessageBox("Err image too many!");
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
					//break;
				}
			}
		}
	}
	if(pVideo->m_iWorkStatusA == TESTOR_STOP)
	{
		pVideo->AAFrameCnt = 0;
	}
    SetEvent(pVideo->m_hCameraThreadEventDeadA);
   // _endthread();
	 _endthreadex(0);
}

/*
* Display_Thread
* -------------
* function: while 'playing' get latest sensor buffer, apply colerconversion and display 
* in:       pointer to the CSimpleDlg class
* out:
* special:  this function is called and run as a threadread.
*/
void Display_ThreadA(void *param)
{
	CAAVision  *pVideo = (CAAVision *)param;
	UINT type = pVideo->CurrentSensorA.type;
	UINT width = pVideo->m_PreviewWidthA;
	UINT height = pVideo->m_PreviewHeightA;
	BYTE* pBmp = (BYTE*)malloc(width*height*4);
    while (pVideo->m_iWorkStatusA != TESTOR_STOP)
    {
		//change display buffer.
		if((pVideo->m_iWorkStatusA == TESTOR_START))
		{
			if (!pVideo->TripleBufferFlipA(&pVideo->m_pDisplayBufferA, FALSE))
			{
				Sleep(1);
				continue;
			}
			
			if(pVideo->AAFrameCnt)
			{
				ImageProcess(pVideo->m_pDisplayBufferA, pBmp, width, height, &(pVideo->m_FrameInfoA),pVideo->m_iDevIDA);
				//在图像上画线
				if (pVideo->m_paintflag == TRUE)
				{				
					//竖线
					int lineLength = 50;
					int lineWidth = 10;

					pVideo->PaintCrossCenter(pBmp,pVideo->m_FrameInfoA,lineLength,lineWidth);

					int lengthS = g_AAParam.size/(g_AAParam.scale);//小矩形长度
					int lengthB = g_AAParam.block/(g_AAParam.scale);//大矩形长度

					int lenS = g_AAParam.mapdislength/(g_AAParam.scale); //中心矩形跟边上矩形的距离
					int lenB = g_AAParam.mapdiswidth/(g_AAParam.scale);

					//根据选取的点作为中心来画不同的矩形
					int x =0;
					int y =0;
					int bigx = 0;
					int bigy =0;
					pVideo->DecisionRectByTemplateType(g_AAParam.temPlate[pVideo->m_iDevIDA],lengthS,lengthB,pVideo->m_FrameInfoA.uWidth,pVideo->m_FrameInfoA.uHeight,x,y,bigx,bigy);
					if (x>=0 &&x<pVideo->m_FrameInfoA.uWidth &&y >=0 &&y<pVideo->m_FrameInfoA.uHeight)
					{					
						pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,x,y,lengthS,lengthS,lineWidth,2);//中心小矩形	
					}
					if (bigx>=0 &&bigx<pVideo->m_FrameInfoA.uWidth &&bigy >=0 &&bigy<pVideo->m_FrameInfoA.uHeight)
					pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,bigx,bigy,lengthB,lengthB,lineWidth,1);//中心大矩形

					
					pVideo->DecisionRectByTemplateType(g_AAParam.temPlate[pVideo->m_iDevIDA],lengthS,lengthB,pVideo->m_FrameInfoA.uWidth,pVideo->m_FrameInfoA.uHeight,x,y,bigx,bigy);
					x = (x- lenS) >0?(x- lenS):0;
					bigx = (bigx- lenS)>0?(bigx- lenS):0 ;
					if (x>=0 &&x<pVideo->m_FrameInfoA.uWidth &&y >=0 &&y<pVideo->m_FrameInfoA.uHeight)
					{
						pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,x,y,lengthS,lengthS,lineWidth,2);//left小矩形
						
					}
					if (bigx>=0 &&bigx<pVideo->m_FrameInfoA.uWidth &&bigy >=0 &&bigy<pVideo->m_FrameInfoA.uHeight)
					pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,bigx,bigy,lengthB,lengthB,lineWidth,1);//left大矩形


					
					pVideo->DecisionRectByTemplateType(g_AAParam.temPlate[pVideo->m_iDevIDA],lengthS,lengthB,pVideo->m_FrameInfoA.uWidth,pVideo->m_FrameInfoA.uHeight,x,y,bigx,bigy);
					x = (x + lenS)< pVideo->m_FrameInfoA.uWidth?(x + lenS):pVideo->m_FrameInfoA.uWidth;
					bigx = (bigx+ lenS)<pVideo->m_FrameInfoA.uWidth? (bigx+ lenS):pVideo->m_FrameInfoA.uWidth;
					if (x>=0 &&x<pVideo->m_FrameInfoA.uWidth &&y >=0 &&y<pVideo->m_FrameInfoA.uHeight)
					{
						pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,x,y,lengthS,lengthS,lineWidth,2);//right小矩形

					}
					if (bigx>=0 &&bigx<pVideo->m_FrameInfoA.uWidth &&bigy >=0 &&bigy<pVideo->m_FrameInfoA.uHeight)
					pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,bigx,bigy,lengthB,lengthB,lineWidth,1);//right大矩形


					pVideo->DecisionRectByTemplateType(g_AAParam.temPlate[pVideo->m_iDevIDA],lengthS,lengthB,pVideo->m_FrameInfoA.uWidth,pVideo->m_FrameInfoA.uHeight,x,y,bigx,bigy);
					y =(y - lenB)>0?(y - lenB):0;
					bigy = (bigy- lenB)>0?(bigy- lenB):0 ;
					if (x>=0 &&x<pVideo->m_FrameInfoA.uWidth &&y >=0 &&y<pVideo->m_FrameInfoA.uHeight)
					{
					pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,x,y,lengthS,lengthS,lineWidth,2);//top小矩形

					}
					if (bigx>=0 &&bigx<pVideo->m_FrameInfoA.uWidth &&bigy >=0 &&bigy<pVideo->m_FrameInfoA.uHeight)
					pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,bigx,bigy,lengthB,lengthB,lineWidth,1);//top大矩形

					pVideo->DecisionRectByTemplateType(g_AAParam.temPlate[pVideo->m_iDevIDA],lengthS,lengthB,pVideo->m_FrameInfoA.uWidth,pVideo->m_FrameInfoA.uHeight,x,y,bigx,bigy);
					y = y + lenB ;
					bigy = bigy+ lenB ;
					if (x>=0 &&x<pVideo->m_FrameInfoA.uWidth &&y >=0 &&y<pVideo->m_FrameInfoA.uHeight)
					{
						pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,x,y,lengthS,lengthS,lineWidth,2);//bottom小矩形

					}
					if (bigx>=0 &&bigx<pVideo->m_FrameInfoA.uWidth &&bigy >=0 &&bigy<pVideo->m_FrameInfoA.uHeight)
					pVideo->PaintRetangle(pBmp,pVideo->m_FrameInfoA,bigx,bigy,lengthB,lengthB,lineWidth,1);//bottom大矩形
	

				}		

				DisplayRGB24(pBmp, &pVideo->m_FrameInfoA,pVideo->m_iDevIDA);
			}
		}
		else
		{
			  Sleep(50);
		}
    }
	free(pBmp);
	pBmp = NULL;
    SetEvent(pVideo->m_hDisplayThreadEventDeadA);
    _endthread();
}

