
#ifndef DYNAMICLOAD_H
#define DYNAMICLOAD_H

// 初步封装的接口在这里定义

//初始化
int init();

//机器人连接
int  ConnectRobot(int iComPort, int iBaudRate);

//引用
bool ReferenceStage(int iID,char* szAxis);

//设置对应控制器对应轴的软限位范围的最低最高值,LowOrHigh的值:high=1,low=0
BOOL sSoftLimit(int ID, const char* axisArr, const double* SoftLimitLowEnd, int LowOrHigh);

// 获取对应控制器对应轴的软限位范围的最低最高值,LowOrHigh的值:high=1,low=0
BOOL gSoftLimit(int ID, const char* axisArr, double* SoftLowLimit, int LowOrHigh);

// 获取当前坐标
BOOL gCurrentPosition(int ID, const char* axisArr, double* posArr);

//设置当前坐标
BOOL sCurrentPosition(int ID, const char* axisArr,const double* posArr);

//设置轴心点坐标
BOOL sPivotPointCoordinates(int ID, const char* axisArr,const double* Cordinate);

//获取轴心点坐标
BOOL gPivotPointCoordinates(int ID, const char* axisArr, double* posArr);

//获取当前硬件限位范围：LowOrHigh=1，获取最大值；LowOrHigh=0，获取最小值
BOOL gEndTravelRange(int ID, const char* axisArr, double* rangeArr, int LowOrHigh);


//设置速度
BOOL sAxisVelocity(int ID, const char* axisArr, const double* speedArr);

//获取当速度
 BOOL gCurrentVelocity(int ID, const char* axisArr, double* speedArr);

// 设置一次步进的距离
 BOOL sStepSizeDistance(int ID, const char* axisArr, double* stepArr);

// 走到一个目标点
BOOL MoveToTarget(int ID, const char* axisArr, const double* targetArr);

// 回原点
BOOL GoHomePosition(int ID, const char* axisArr);

//defines a new Tool coordinate system (KST type).
BOOL sDefineToolCoordinate(int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray);


//Move szAxes relative to current position and orientation in Tool coordinate system.
BOOL sMoveRelativeToToolCoordinateSystem (int ID, const char* szAxes, const double* pdValueArray);

//Move szAxes relative to current position and orientation in Work coordinate system
BOOL sMoveRelativeToWorkCoordinateSystem (int ID, const char* szAxes, const double* pdValueArray);


//defines a new Work coordinate system
BOOL sNewWorkCoordinateSystem (int ID, const char* szNameOfCoordSystem, const char* szAxes, const double* pdValueArray);

//Enables an already defined coordinate system.
BOOL sEnabledCoordinateSystems (int ID, const char* EnabledCoordinateName);

//Lists enabled coordinate systems by name.
BOOL gListsEnabledCoordinateSystems(int ID, const char* EnabledCoordinateNameList, char* buffer, int bufsize);
#endif
