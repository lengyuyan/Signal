#pragma once
#include <string>
#include <Windows.h>
#include "imagekit.h"
#include "Config.h"


class CAAPaint
{
public:

	//CAAPaint();
	//~CAAPaint();



	 void PaintCrossCenter(BYTE* pBmp,FrameInfo& FrameInfo,int lineLength = 50,int lineWidth = 10);//在中心画十字交叉线
	 void PaintSpecialRetangle(BYTE* pBmp,FrameInfo& FrameInfo,int lineLength,int lineWidth);//
	 void PaintSpecialLine(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int lineLength ,int lineWidth ,int color);

	 //type =0 水平线，type = 1竖直线 color:1,2,3 blue green red
	 void PaintLine(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int Length ,int lineWidth ,int type,int color);//

	  void PaintRetangle(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int Length ,int width,int lineWidth ,int color);//

private:
	 void PaintColor(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int width ,int height ,int type,int color);//
	
};