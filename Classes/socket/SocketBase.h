#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__
#include "cocos2d.h"
#include <list>
#include <thread>
USING_NS_CC;

//平台区分
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
#define HSocket SOCKET

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <error.h>
#include <arpa/inet.h>		// for inet_**
#include <netdb.h>			// for gethost**
#include <netinet/in.h>		// for sockaddr_in
#include <sys/types.h>		// for socket
#include <sys/socket.h>		// for socket
#include <unistd.h>
#include <stdio.h>		    // for printf
#include <stdlib.h>			// for exit
#include <string.h>			// for bzero
#define HSocket int
#endif 

//消息类型
enum MessageType
{
	DISCONNECT,
	RECEIVE,
	NEW_CONNECTION
};

//网络消息定义
class SocketMessage
{
private:
	MessageType msgType;
	Data* msgData;

public:
	/*
		构造一个SocketMessage
	*/
	SocketMessage(MessageType type, unsigned char* data, int dataLen)
	{
		msgType = type;
		msgData = new Data;
		msgData->copy(data, dataLen);
	}
	/*
		构造空消息
	*/
	SocketMessage(MessageType type)
	{
		msgType = type;
		msgData = nullptr;
	}
	//取得消息数据
	Data* getMsgData() { return msgData; }
	//取得消息的类型
	MessageType getMsgType() { return msgType; }
	//析构数据
	~SocketMessage()
	{
		if (msgData)
			CC_SAFE_DELETE(msgData);
	}
};

/*
	Socket基类定义
*/
class SocketBase : public Ref
{
public:
	SocketBase();
	~SocketBase();
	//非阻塞
	bool nonBlock(HSocket socket);

protected:
	//关闭连接
	void closeConnect(HSocket socket);
	//网络错误
	bool error(HSocket socket);
	
protected:
	//互斥锁
	std::mutex _mutex;

private:
	//是否初始化成功
	bool _bInitSuccess;
};

#endif