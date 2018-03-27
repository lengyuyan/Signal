// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 ROBOTLIB_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// ROBOTLIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef ROBOTLIB_EXPORTS
#define ROBOTLIB_API __declspec(dllexport)
#else
#define ROBOTLIB_API __declspec(dllimport)
#endif

typedef int BOOL;

// 做一个动态加载的演示
ROBOTLIB_API int unitTest(void);

//.......................连接上机器人（并获取引用）..................................
// 连接上机器人
//   @Input:   iComPort         串口
//             iBaudRate        波特率 115200
//   @Return： ID号             连接成功
//             FALSE            其他
ROBOTLIB_API int connectToRobot(int iComPort, int iBaudRate);


//............................引用设置................................................
// Reference,引用
//   @Input:   ID                控制器ID
//             axisArray         轴号
//   @Return： TRUE              引用成功
//             FALSE             其他
ROBOTLIB_API BOOL Reference(int ID, char* axisArray);


// .........................设置软限位.................................................
// 设置对应控制器对应轴的软限位范围的最低值
//   @Input:   ID                控制器ID
//             axisArray         轴
//             SoftLimitLowEnd   设置软限位范围的最高值或者最低值
//             LowOrHigh         LowOrHigh=1为软限位范围的最高值,LowOrHigh=0为软限位范围的最低值
//   @Return： TRUE              引用成功
//             FALSE             其他
ROBOTLIB_API BOOL setPositionSoftLimit(int ID, const char* axisArr, const double* SoftLimitLowEnd, int LowOrHigh);


// .........................获取软限位.................................................
// 获取对应控制器对应轴的软限位范围的最高值或最低值
//   @Input:   ID                控制器ID
//             axisArray         轴                
//             LowOrHigh         LowOrHigh=1为软限位范围的最高值,LowOrHigh=0为软限位范围的最低值
//   @Output:  SoftLimitLowEnd   接收软限位范围的最高值或者最低值 
//   @Return： TRUE              获取软限位成功
//             FALSE             其他
ROBOTLIB_API BOOL getPositionSoftLimit(int ID, const char* axisArr, double* SoftLowLimit, int LowOrHigh);


// .........................获取当前位置的坐标.................................................
// 获取当前坐标
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Output:  posArr            接收当前坐标
//   @Return： TRUE              获取当前坐标成功
//             FALSE             其他
ROBOTLIB_API BOOL getCurrentPosition(int ID, const char* axisArr, double* posArr);

// .........................设置当前坐标.................................................
// 设置当前坐标
//   @Input:   ID                控制器ID
//             axisArray         轴                
//             posArr            设置当前坐标
//   @Return： TRUE              获取当前坐标成功
//             FALSE             其他
ROBOTLIB_API BOOL setCurrentPosition(int ID, const char* axisArr, const double* posArr);


// .........................设置轴心点坐标.................................................
// 设置轴心点坐标
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Return： TRUE              获取工具坐标系原点成功
//             FALSE             其他
ROBOTLIB_API BOOL setPivotPointCoordinates(int ID, const char* axisArr,const double* pdValueArray);


// .........................获取轴心点坐标.................................................
// 获取轴心点坐标
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Output:  posArr            接收工具坐标系原点值
//   @Return： TRUE              获取工具坐标系原点成功
//             FALSE             其他
ROBOTLIB_API BOOL getPivotPointCoordinates(int ID, const char* axisArr, double* posArr);



// .........................获取当前硬件限位范围.................................................
// 获取当前硬件限位范围
//   @Input:   ID                控制器ID
//             axisArray         轴                
//             LowOrHigh         LowOrHigh=1为获取当前硬件限位范围的最高值,LowOrHigh=0为获取当前硬件限位范围的最低值
//   @Output:  rangeArr          接收当前硬件限位范围，LowOrHigh=1为获取当前硬件限位范围的最高值,LowOrHigh=0为获取当前硬件限位范围的最低值
//   @Return： TRUE              获取当前硬件限位范围成功
//             FALSE             其他
ROBOTLIB_API BOOL getEndTravelRange(int ID, const char* axisArr, double* rangeArr, int LowOrHigh);



// 设置速度
//   @Input:   ID                控制器ID
//             axisArray         轴                
//             speedArr          设置的速度
//   @Return： TRUE              成功
//             FALSE             其他
ROBOTLIB_API BOOL setAxisVelocity(int ID, const char* axisArr, const double* speedArr);


//获取速度
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Output:  speedArr          获取速度
//   @Return： TRUE              成功
//             FALSE             其他
ROBOTLIB_API BOOL getCurrentVelocity(int ID, const char* axisArr, double* speedArr);


// 设置一次步进的距离
//   @Input:   ID                控制器ID
//             axisArray         轴                
//             speedArr          设置一次步进的距离
//   @Return： TRUE              成功
//             FALSE             其他
ROBOTLIB_API BOOL setStepSizeDistance(int ID, const char* axisArr, double* stepArr);

// 走到一个目标点
//   @Input:   ID                控制器ID
//             axisArray         轴                
//             targetArr          目标点的值
//   @Return： TRUE              成功
//             FALSE             其他
ROBOTLIB_API BOOL MoveTo(int ID, const char* axisArr, const double* targetArr);

// 回原点
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Return： TRUE              成功
//             FALSE             其他
ROBOTLIB_API BOOL GoToHomePosition(int ID, const char* axisArr);



//defines a new Tool coordinate system (KST type).
//   @Input:   ID                    控制器ID
//             szNameOfCoordSystem   新坐标系统的名称
//             axisArray             轴   
//             ToolCoordinateValue          
//   @Return： TRUE                  成功
//             FALSE                 其他
ROBOTLIB_API BOOL DefineToolCoordinate(int ID, const char* szNameOfCoordSystem, const char* axisArr, const double* ToolCoordinateValue);


//Move szAxes relative to current position and orientation in Tool coordinate system.
//   @Input:   ID                    控制器ID
//             axisArray             轴   
//             MoveToolDistance      移动所到点的值（工具坐标系）   
//   @Return： TRUE                  成功
//             FALSE                 其他
ROBOTLIB_API BOOL MoveRelativeToToolCoordinateSystem (int ID, const char* axisArr, const double* MoveToolDistance);

//Move szAxes relative to current position and orientation in Work coordinate system
//   @Input:   ID                    控制器ID
//             axisArray             轴   
//             MoveToolDistance      移动所到点的值（工作坐标系）   
//   @Return： TRUE                  成功
//             FALSE                 其他
ROBOTLIB_API BOOL MoveRelativeToWorkCoordinateSystem (int ID, const char* axisArr, const double* pdValueArray);


//defines a new Work coordinate system
//   @Input:   ID                    控制器ID
//             szNameOfCoordSystem   工作坐标系的名称
//             axisArray             轴   
//             MoveToolDistance      轴的值（工作坐标系）   
//   @Return： TRUE                  成功
//             FALSE                 其他
ROBOTLIB_API BOOL NewWorkCoordinateSystem (int ID, const char* szNameOfCoordSystem, const char* axisArr, const double* pdValueArray);


//Enables an already defined coordinate system.
//   @Input:   ID                    控制器ID
//             EnabledCoordinateName 要设置的坐标系名称
//   @Return： TRUE                  成功
//             FALSE                 其他
ROBOTLIB_API BOOL EnabledCoordinateSystems (int ID, const char* EnabledCoordinateName);

//Lists enabled coordinate systems by name.
//   @Input:   ID                        控制器ID
//             EnabledCoordinateNameList 要设置的坐标系名称
//             buffer                    缓存
//             bufsize                   缓存大小
//   @Return： TRUE                      成功
//             FALSE                     其他
ROBOTLIB_API BOOL ListsEnabledCoordinateSystems(int ID, const char* EnabledCoordinateNameList, char* buffer, int bufsize);