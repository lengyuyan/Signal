// RobotLib.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "RobotLib.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>

#include "DynamicLoad.h"

#include "mtGCS.h"


ROBOTLIB_API int unitTest(void)
{
	return init();
}

//连接机器人
 ROBOTLIB_API int connectToRobot(int iComPort, int iBaudRate)
{
	int ID=ConnectRobot(iComPort,iBaudRate);
	return ID;
}




//Referebce引用
ROBOTLIB_API BOOL Reference(int ID, char* axis)
{
	if(ReferenceStage(ID,axis)==FALSE)
	{
	//	printf("Referebce falied\n");
		return FALSE;
	}
	return TRUE;
}


// 设置软限位：LowOrHigh=1，设置最大值；LowOrHigh=0，设置最小值
ROBOTLIB_API BOOL setPositionSoftLimit(int ID, const char* axisArr, const double* SoftLimitLowEnd, int LowOrHigh)
{
	if(sSoftLimit( ID,axisArr, SoftLimitLowEnd, LowOrHigh)==FALSE)
	{
	//	printf("set the position soft limit\n");
		return FALSE;
	}
	return TRUE;
}

// 获取软限位：LowOrHigh=1，获取最大值；LowOrHigh=0，获取最小值
ROBOTLIB_API BOOL getPositionSoftLimit(int ID, const char* axisArr, double* SoftLowLimit, int LowOrHigh)
{
	if(gSoftLimit(ID,axisArr,SoftLowLimit,LowOrHigh)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//获取当前坐标
ROBOTLIB_API BOOL getCurrentPosition(int ID, const char* axisArr, double* posArr)
{
	if(gCurrentPosition(ID,axisArr,posArr)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//设置当前坐标
ROBOTLIB_API BOOL setCurrentPosition(int ID, const char* axisArr, const double* posArr)
{
	if(sCurrentPosition(ID,axisArr,posArr)== FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//设置轴心点坐标
ROBOTLIB_API BOOL setPivotPointCoordinates(int ID, const char* axisArr,const double* pdValueArray)
{
	if(sPivotPointCoordinates(ID,axisArr,pdValueArray)==FALSE)
	{
		return FALSE; 
	}
	return TRUE;
}


// 获取轴心点坐标
ROBOTLIB_API BOOL getPivotPointCoordinates(int ID, const char* axisArr, double* posArr)
{
	if(gPivotPointCoordinates(ID, axisArr,posArr)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//获取当前硬件限位范围：LowOrHigh=1，获取最大值；LowOrHigh=0，获取最小值
ROBOTLIB_API BOOL getEndTravelRange(int ID, const char* axisArr, double* rangeArr, int LowOrHigh)
{
	if((gEndTravelRange(ID, axisArr, rangeArr, LowOrHigh))==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

// 设置速度
ROBOTLIB_API BOOL setAxisVelocity(int ID, const char* axisArr, const double* speedArr)
{
	if((sAxisVelocity(ID, axisArr,speedArr))==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//获取速度
ROBOTLIB_API BOOL getCurrentVelocity(int ID, const char* axisArr, double* speedArr)
{
	if((gCurrentVelocity(ID,axisArr, speedArr))==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

// 设置一次步进的距离
ROBOTLIB_API BOOL setStepSizeDistance(int ID, const char* axisArr, double* stepArr)
{
	if((sStepSizeDistance(ID, axisArr,stepArr))==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

// 走到一个目标点
ROBOTLIB_API BOOL MoveTo(int ID, const char* axisArr, const double* targetArr)
{
	if((MoveToTarget(ID,axisArr,targetArr))==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

// 回原点
ROBOTLIB_API BOOL GoToHomePosition(int ID, const char* axisArr)
{
	if((GoHomePosition(ID,axisArr))==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}


//defines a new Tool coordinate system (KST type).
ROBOTLIB_API BOOL DefineToolCoordinate(int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray)
{
	if(sDefineToolCoordinate(ID,szNameOfCoordSystem,szAxes,pdValueArray)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}


//Move szAxes relative to current position and orientation in Tool coordinate system.
ROBOTLIB_API BOOL MoveRelativeToToolCoordinateSystem (int ID, const char* szAxes, const double* pdValueArray)
{
	if(sMoveRelativeToToolCoordinateSystem (ID,szAxes,pdValueArray)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//Move szAxes relative to current position and orientation in Work coordinate system
ROBOTLIB_API BOOL MoveRelativeToWorkCoordinateSystem (int ID, const char* szAxes, const double* pdValueArray)
{
	if(sMoveRelativeToWorkCoordinateSystem ( ID, szAxes,  pdValueArray)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//defines a new Work coordinate system
ROBOTLIB_API BOOL NewWorkCoordinateSystem (int ID, const char* szNameOfCoordSystem, const char* axisArr, const double* pdValueArray)
{
	if(!sNewWorkCoordinateSystem(ID, szNameOfCoordSystem, axisArr, pdValueArray))
	{
		return FALSE;
	}
	return TRUE;
}



//Enables an already defined coordinate system.
//   @Input:   ID                    控制器ID
//             EnabledCoordinateName 要设置的坐标系名称
//   @Return： TRUE                  成功
//             FALSE                 其他
ROBOTLIB_API BOOL EnabledCoordinateSystems (int ID, const char* EnabledCoordinateName)
{
	if(sEnabledCoordinateSystems (ID,EnabledCoordinateName)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

//Lists enabled coordinate systems by name.
//   @Input:   ID                        控制器ID
//             EnabledCoordinateNameList 要设置的坐标系名称
//             buffer                    缓存
//             bufsize                   缓存大小
//   @Return： TRUE                      成功
//             FALSE                     其他
ROBOTLIB_API BOOL ListsEnabledCoordinateSystems(int ID, const char* EnabledCoordinateNameList, char* buffer, int bufsize)
{
	if(gListsEnabledCoordinateSystems(ID,EnabledCoordinateNameList,buffer,bufsize)==FALSE)
	{
		return FALSE;
	}
	return TRUE;

}