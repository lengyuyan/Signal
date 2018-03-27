#pragma once
#ifdef AAROBOTLIB_EXPORTS
#define AAROBOTLIB_API __declspec(dllexport)
#else
#define AAROBOTLIB_API __declspec(dllimport)
#endif

#define AA_PI_HAVE_CONNECT -2 //暂时用的错误码,后续可以共用一个错误码文件


/********************************************************************/
/*函数名:AARobotConnectTCPIPbyDescription                                     */
/*功能:关闭连接                                                     */
/*输入:const char* szDescription 机器人网络标识                    */
/*                                    */
/*                                     */
/*输出:无                                                           */
/*返回值:int 控制器id，<0错误码                                 */
/********************************************************************/
AAROBOTLIB_API int AARobotConnectTCPIPbyDescription (const char* szDescription) ;


/********************************************************************/
/*函数名:AARobotEnumerateTCPIPDevices                                     */
/*功能:关闭连接                                                     */
/*输入:char* szBuffer 枚举到的所有机器人网络标识                    */
/*    int iBufferSize    szBuffer大小                               */
/*    const char* szFilter  过滤器                                  */
/*输出:无                                                           */
/*返回值:int 枚举到的数量，<0错误码                                 */
/********************************************************************/
AAROBOTLIB_API int AARobotEnumerateTCPIPDevices (char* szBuffer, int iBufferSize, const char* szFilter) ;

/*.......................机器人c串口连接初始化..................................
 *   @Input:   comPort         串口号
 *   @Return： >= 0 成功； < 0 失败
 */
AAROBOTLIB_API int AARobotInit(int comPort);

/*.......................机器人TCPIP连接初始化..................................
 *   @Input:   NULL
 *   @Return： >= 0 成功； < 0 失败
 */
AAROBOTLIB_API int AARobotTCPIPInit();//待删除

/********************************************************************/
/*函数名:AARobotTCPIPInitOne                                 */
/*功能:初始化指定ip和端口的控制器                                   */
/*输入:const char* ip 控制器ip                                      */
/*输入:int port 控制器端口                                          */
/*输出:无                                                           */
/*返回值:0，成功；其他:失败                                         */
/********************************************************************/
AAROBOTLIB_API int AARobotTCPIPInitOne(const  char* ip, int port);//



/********************************************************************/
/*函数名:AARobotIsConnected                              */
/*功能:通过id检测控制器是否连接                                  */
/*输入:int ID                                      */
/*                                          */
/*输出:无                                                           */
/*返回值:1，已连接；没连接                                        */
/********************************************************************/
AAROBOTLIB_API  bool AARobotIsConnected (int ID) ;

/********************************************************************/
/*函数名:AARobotIsConnectedByIpAndPort                              */
/*功能:通过ip和port检测控制器是否连接                                  */
/*输入:int ID                                      */
/*                                          */
/*输出:无                                                           */
/*返回值:1，已连接；没连接                                        */
/********************************************************************/
AAROBOTLIB_API  bool AARobotIsConnectedByIpAndPort(const char* ip, int port) ;


/********************************************************************/
/*函数名:AARobotCloseConnection                               */
/*功能:关闭连接                                  */
/*输入:int ID                                      */
/*                                          */
/*输出:无                                                           */
/*返回值:void                                     */
/********************************************************************/
AAROBOTLIB_API void AARobotCloseConnection (int ID);

/*.......................机器人走相对偏移，或围绕自身做相对旋转..................................
 *   @Input:		axisString			需要运动的轴编号组成的字符串
 *												轴编号为  X、Y、Z、U、V、W六个
 *												轴编号中间，需要用空格分隔
 *												例如，需要移动X、Y、Z时，axisString 为 ：   "X  Y  Z"
 *												需要选择U、V时，axisString为：      "U  V"
 *					posArray			需要走的偏移量（毫米）或偏移角度（度）
 *												是一个数组，轴有几个数据就需要包含几个元素
 *												例如，需要X、Y、Z都移动0.1mm，则posArray数据长度为3，3个元素都为0.1
 *   @Return： >= 0 成功； < 0 失败
 */
AAROBOTLIB_API int AARobotMoveRelative2(const char *axisString, const double *posArray);

AAROBOTLIB_API int AARobotMoveRelative(int controlID,const char *axisString, const double *posArray);


/*............................获取机器人对应轴的当前位置..........................................
 *   @Input:		axisString			需要运动的轴编号组成的字符串
 *												轴编号为  X、Y、Z、U、V、W六个
 *												轴编号中间，需要用空格分隔
 *												例如，需要获取X、Y、Z轴当前位置时，axisString 为 ：   "X  Y  Z"
 *												需要选择U、V时，axisString为：      "U  V"
 *   @Output:		posArray			获取轴的当前的偏移量（毫米）或当前偏移角度（度）
 *												是一个数组，轴有几个数据就需要包含几个元素									
 *   @Return： >= 0 成功； < 0 失败
 */
AAROBOTLIB_API int AARobotCurrentPosition2(const char *axisString, double *posArray);//待删除，
AAROBOTLIB_API int AARobotCurrentPosition(int controlID,const char *axisString, double *posArray);


//移动到绝对位置
AAROBOTLIB_API int AARobotAbsMove(int controlID,const char *axisString,const double* pos);//

//定义当前位置为原点
AAROBOTLIB_API int AARobotDfhHome(int controlID,const char *axisString);//

/***************************************************************/
/*机器人回原,原点默认是世界坐标系原点

*/
AAROBOTLIB_API int AARobotGoHome(int controlID,const char *axisString);//没用


// 设置系统速度
//   @Input:   ID                控制器ID              
//             speedArr          设置的速度
//   @Return： TRUE              成功
//             FALSE             其他
AAROBOTLIB_API int setSystemVelocity(int ID,  double speedArr);


//获取系统速度
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Output:  speedArr          获取速度
//   @Return： TRUE              成功
//             FALSE             其他
AAROBOTLIB_API int getSystemVelocity(int ID, double* speedArr);




// .........................获取轴心点坐标.................................................
// 获取轴心点坐标
//   @Input:   ID                控制器ID
//             axisArray         轴                
//   @Output:  posArr            接收工具坐标系原点值
//   @Return： TRUE              获取工具坐标系原点成功
//             FALSE             其他
AAROBOTLIB_API int getPivotPointCoordinates(int ID, const char* axisArr, double* posArr);




AAROBOTLIB_API int getKLTCoordinates(int ID, const char* szStartCoordSystem, const char* szEndCoordSystem, char* buffer, int bufsize);


//Enables an already defined coordinate system.
AAROBOTLIB_API int enableCoordinate(int ID, const char* szNameOfCoordSystem);


//定义一个KST坐标系
AAROBOTLIB_API int defineKSTCoordinate(int ID, const char* szNameOfCoordSystem, const char* axisArr,  double* ToolCoordinateValue);
//删除一个坐标系
AAROBOTLIB_API int deleteOneCoordinate(int ID, const char* szNameOfCoordSystem);
//激活一个坐标系
 AAROBOTLIB_API int activeOneCoordinate(int ID, const char* szNameOfCoordSystem);
 //查询所有激活的坐标系，按类型排列
 AAROBOTLIB_API int queryActiveCoordinates(int ID, const char* szNamesOfCoordSystems, char* buffer, int bufsize);
 //设置正软限位
 AAROBOTLIB_API int setPosSoftLimit(int ID, const char* szAxes, const double* pdValueArray);
 //设置负软限位
 AAROBOTLIB_API int setNegSoftLimit(int ID, const char* szAxes, const double* pdValueArray);
 //激活软限位
 AAROBOTLIB_API int activeSoftLimit(int ID, const char* szAxes, const int* pbValueArray);