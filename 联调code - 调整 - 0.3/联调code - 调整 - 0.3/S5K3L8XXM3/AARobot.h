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

	
private:
	string				m_szIP;
	int					m_Port;
	int					m_robotID;
	bool				m_connected;


};