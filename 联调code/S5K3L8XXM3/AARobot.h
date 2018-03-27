/***********************************************************************
AARobot类：封装的PI机器人类
***********************************************************************/

#pragma once
#include <string>
using namespace  std;
#define MAX_IPV4_LEN 16

class CAARobot
{
public:
	static int EnumerateAATCPIPDevices(char* szBuffer, int iBufferSize, const char* szFilter);
	CAARobot();
	CAARobot(	string	szIP,int Port,int robotID,bool connected);
	~CAARobot();

	/*static */int ConnectAARobotByIPAndPort(const char* ip, int port);//连接
	int GetAAAbsCurrentPosition(int controlID,const char *axisString, double *posArray);
	int ConnectAARobotbyDescription(const char* szDescription) ;
	int MoveAARelative(int robotID,const char *axisString, const double *posArray);//走相对位置
	int MoveAAAbsolute(int robotID,const char *axisString, const double *posArray);//走绝对位置
	bool IsAAConnected (int robotID);
	static bool IsAAConnectedByIpAndPort(const char* ip, int port);
	void CloseAAConnection (int robotID);

	string& GetAARobotIP();
	int GetAARobotPort();
	int GetAARobotID();
	bool GetAARobotConnected();

	void SetAARobotIP(string& ip);
	void SetAARobotPort(int port);
	void SetAARobotID(int robotID);
	void SetAARobotConnected(bool flag);

	//获取SPI坐标系信息,暂时没用
	int GetPivotPointCoordinates(int robotID, const char* axisArr, double* posArr);
	//获取KLT坐标系信息
	int GetKLTCoordinates(int ID, const char* szStartCoordSystem, const char* szEndCoordSystem, char* buffer, int bufsize);
	//获取系统速度mm/s
	int GetSystemVelocity(int ID, double* speedArr);
	//设置系统速度
	int SetSystemVelocity(int ID,  double speedArr);
	//使能一个坐标系
	int EnableCoordinate(int ID, const char* szNameOfCoordSystem);

	//定义一个kst坐标系
	int DefineKSTCoordinate(int ID, const char* szNameOfCoordSystem, const char* axisArr,  double* ToolCoordinateValue);
	//删除一个坐标系
	int DeleteOneCoordinate(int ID, const char* szNameOfCoordSystem);
	//激活一个坐标系
	int ActiveOneCoordinate(int ID, const char* szNameOfCoordSystem);
	//查询所有激活的坐标系，按类型排列
    int QueryActiveCoordinates(int ID, const char* szNamesOfCoordSystems, char* buffer, int bufsize);
	//设置正软限位
	int SetPosSoftLimit(int ID, const char* szAxes, const double* pdValueArray);
	//设置负软限位
	int SetNegSoftLimit(int ID, const char* szAxes, const double* pdValueArray);
	//激活软限位
	int ActiveSoftLimit(int ID, const char* szAxes, const int* pbValueArray);
	
private:
	string				m_szIP;
	int					m_Port;
	int					m_robotID;
	bool				m_connected;


};