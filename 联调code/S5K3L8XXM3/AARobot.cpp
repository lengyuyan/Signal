
#include "AARobotLib.h"
#include"AARobot.h"
#include "AA_define_tool.h"

#define INVALID_VALUE -1
#define AAROBOTPORT 50000

int CAARobot::EnumerateAATCPIPDevices(char* szBuffer, int iBufferSize, const char* szFilter)//枚举pi设备
{
	int devNum =0;
	char *buf = NULL;  
	char *p[8] = {NULL};
	int in = 0;

	devNum = AARobotEnumerateTCPIPDevices (szBuffer, NETDEVICESLEN-1, "");
	return devNum;
}
CAARobot::CAARobot()

{
	m_robotID = INVALID_VALUE;
	m_Port = AAROBOTPORT;
	m_connected = false;
	

}

CAARobot::CAARobot(string	szIP,int Port,int robotID,bool connected)
{
	m_robotID = robotID;
	m_szIP = szIP;
	m_Port = Port;
	m_connected = connected;
}
CAARobot::~CAARobot()
{
	if (IsAAConnected (m_robotID))
	{
		CloseAAConnection(m_robotID);
	}
	
}

int CAARobot::GetAAAbsCurrentPosition(int robotID,const char *axisString, double *posArray)
{
	return AARobotCurrentPosition(robotID,axisString, posArray);
}

int CAARobot::MoveAARelative(int robotID,const char *axisString, const double *posArray)
{
	return AARobotMoveRelative(robotID,axisString, posArray);
}

int CAARobot::MoveAAAbsolute(int robotID,const char *axisString, const double *posArray)
{
	return AARobotAbsMove(robotID,axisString,posArray);
}


 bool CAARobot::IsAAConnected (int robotID)
{
	bool flag = AARobotIsConnected ( robotID);
	if (flag != true)
	{
		m_connected = false;
	}
	return flag;
}

 bool CAARobot::IsAAConnectedByIpAndPort(const char* ip, int port)
 {
	bool flag =  AARobotIsConnectedByIpAndPort(ip,port);
	/*if (flag != true)
	{
		m_connected = false;
	}*/

	return flag;
 }

int CAARobot::ConnectAARobotByIPAndPort(const char* ip, int port)
{
	int ret =-1;
	ret =  AARobotTCPIPInitOne(ip,port);
	if (ret >= 0)
	{
		m_robotID = ret;
		m_szIP = ip;
		m_Port = port;
		m_connected = true;
	}
	else
	{
		m_robotID = ret;
		m_szIP = ip;
		m_Port = port;
		m_connected = false;
	}
	return ret;
}

int CAARobot::ConnectAARobotbyDescription (const char* szDescription) 
{
	return AARobotConnectTCPIPbyDescription(szDescription);
}

 void CAARobot::CloseAAConnection (int robotID)
{
	return AARobotCloseConnection( robotID);
}

 string& CAARobot::GetAARobotIP()
 {
	 return m_szIP;
 }
 int CAARobot::GetAARobotPort()
 {
	 return m_Port;
 }
 int CAARobot::GetAARobotID()
 {
	 return m_robotID;
 }

 void CAARobot::SetAARobotIP(string& ip)
 {
	 m_szIP = ip;
 }
 void CAARobot::SetAARobotPort(int port)
 {
	 m_Port = port;
 }
 void CAARobot::SetAARobotID(int robotID)
 {
	 m_robotID = robotID;
 }

 bool CAARobot::GetAARobotConnected()
 {
	 return m_connected;
 }

 void CAARobot::SetAARobotConnected(bool flag)
 {
	 m_connected = flag;
 }

 int CAARobot::GetPivotPointCoordinates(int robotID, const char* axisArr, double* posArr)//SPI 不知道啥意思
 {
	 return getPivotPointCoordinates(robotID, axisArr, posArr);
 }

int CAARobot::GetKLTCoordinates(int ID, const char* szStartCoordSystem, const char* szEndCoordSystem, char* buffer, int bufsize)
{
	return getKLTCoordinates(ID, szStartCoordSystem, szEndCoordSystem, buffer, bufsize);
}

int CAARobot::GetSystemVelocity(int ID, double* speedArr)
{
	return getSystemVelocity(ID, speedArr);
}
int CAARobot::SetSystemVelocity(int ID,  double speedArr)
{
	return setSystemVelocity(ID, speedArr);
}
int  CAARobot::EnableCoordinate(int ID, const char* szNameOfCoordSystem)
{
	return enableCoordinate(ID, szNameOfCoordSystem);
}

int  CAARobot::DefineKSTCoordinate(int ID, const char* szNameOfCoordSystem, const char* axisArr,  double* ToolCoordinateValue)
{
	return defineKSTCoordinate(ID, szNameOfCoordSystem,axisArr,  ToolCoordinateValue);
}

int CAARobot::DeleteOneCoordinate(int ID, const char* szNameOfCoordSystem)
{
	return deleteOneCoordinate(ID, szNameOfCoordSystem);
}

int CAARobot::ActiveOneCoordinate(int ID, const char* szNameOfCoordSystem)
{
	return activeOneCoordinate(ID, szNameOfCoordSystem);
}
//查询所有激活的坐标系，按类型排列
int CAARobot::QueryActiveCoordinates(int ID, const char* szNamesOfCoordSystems, char* buffer, int bufsize)
{
	return  queryActiveCoordinates(ID, szNamesOfCoordSystems, buffer, bufsize);
}

//设置正软限位
int CAARobot::SetPosSoftLimit(int ID, const char* szAxes, const double* pdValueArray)
{
	return setPosSoftLimit(ID, szAxes, pdValueArray);
}
//设置负软限位
int CAARobot::SetNegSoftLimit(int ID, const char* szAxes, const double* pdValueArray)
{
	return setNegSoftLimit(ID, szAxes, pdValueArray);
}
//激活软限位
int CAARobot::ActiveSoftLimit(int ID, const char* szAxes, const int* pbValueArray)
{
	return activeSoftLimit(ID, szAxes, pbValueArray);
}
