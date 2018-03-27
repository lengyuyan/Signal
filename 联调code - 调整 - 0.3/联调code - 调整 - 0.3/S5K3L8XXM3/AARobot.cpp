
#include "AARobotLib.h"
#include"AARobot.h"
#include "AA_define_tool.h"

#define INVALID_VALUE -1
#define AAROBOTPORT 50000

int CAARobot::EnumerateAATCPIPDevices(char* szBuffer, int iBufferSize, const char* szFilter)//Ã¶¾ÙpiÉè±¸
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