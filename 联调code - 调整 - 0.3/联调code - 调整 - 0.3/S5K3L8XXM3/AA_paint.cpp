# include "AA_paint.h"
#include "Config.h"
#include "AA_define_tool.h"

void CAAPaint::PaintCrossCenter(BYTE* pBmp,FrameInfo &frameInfo,int lineLength ,int lineWidth )
{
	for (int i= frameInfo.uWidth/2 -lineWidth;i<frameInfo.uWidth/2+lineWidth;i++)
	{
		for (int j= frameInfo.uHeight/2 -lineLength;j<frameInfo.uHeight/2 +lineLength;j++)
		{
			pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;						
		}
	}
	for (int j= frameInfo.uHeight/2 -lineWidth;j<frameInfo.uHeight/2+lineWidth;j++)
	{
		for (int i= frameInfo.uWidth/2-lineLength;i<frameInfo.uWidth/2+lineLength;i++)
		{
			pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;
		}
	}
}
void CAAPaint::PaintSpecialLine(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int lineLength,int lineWidth ,int color)//画水平或者竖直线
{
	for (int i= x;i<x+lineWidth;i++)
	{
		for (int j= y ;j<y +lineLength;j++)
		{
			switch(color)
			{		
			case 1://blue
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 255;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;
			case 2://green
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 255;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;
			case 3://red
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;	
				break;
			default:
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;

			}

		}
	}
}

void CAAPaint::PaintColor(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int width ,int height ,int type,int color)
{
	for (int i= x;i<x+width;i++)
	{
		for (int j= y ;j<y +height;j++)
		{
			switch(color)
			{		
			case 1://blue
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 255;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;
			case 2://green
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 255;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;
			case 3://red
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;	
				break;
			default:
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;

			}

		}
	}

}

void CAAPaint::PaintLine(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int Length ,int lineWidth ,int type,int color)
{
	if (type == 1)//竖直线
	{
		PaintColor(pBmp,frameInfo,x,y,lineWidth,Length,type,color);
	}
	else if(type == 0)//水平线
	{
		PaintColor(pBmp,frameInfo,x,y,Length,lineWidth,type,color);
	}	
	else
	{
		//斜线
	}
}

void CAAPaint::PaintRetangle(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int Length ,int width,int lineWidth ,int color)

{
	PaintLine(pBmp,frameInfo,x,y,Length,lineWidth,0,color);//top
	PaintLine(pBmp,frameInfo,x,y,width,lineWidth,1,color);//left
	PaintLine(pBmp,frameInfo,x+Length,y,width,lineWidth,1,color);//right
	PaintLine(pBmp,frameInfo,x,y+width,Length,lineWidth,0,color);//bottom


}
void CAAPaint::PaintSpecialRetangle(BYTE* pBmp,FrameInfo& frameInfo,int lineLength ,int lineWidth )
{
	int size = CFG_INSTANCE->get_cfg_int(AA_Globle_param,AA_size);
	int block = CFG_INSTANCE->get_cfg_int(AA_Globle_param,AA_block);
	double lenLarge = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_t_mapdislength);
	double lenSmall = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_t_mapdiswidth);
	double pixel_size = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_pixel_size);
	double scale = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_scale);

	//在原始图像是
	//pixel_size = pixel_size/scale;



	//中心的竖线
	lineLength = size/scale; //垂直方向上的长度
	lineWidth = 10;//水平反方向上的长度


	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineWidth/2,frameInfo.uHeight/2,lineLength,lineWidth,1);//右边线
	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineLength-lineWidth/2,frameInfo.uHeight/2,lineLength,lineWidth,1);//left
	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineLength,frameInfo.uHeight/2-lineWidth/2,lineWidth,lineLength,1);//top
	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineLength,frameInfo.uHeight/2+lineLength-lineWidth/2,lineWidth,lineLength,1);

}



