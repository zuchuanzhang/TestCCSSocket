#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__
#include "SocketBase.h"

/*
	Socket客户端定义
*/
class SocketClient : public SocketBase
{
public:
	//构造
	static SocketClient* construct();
	//销毁
	void destroy();
	//连接服务器
	bool connectServer(const char* serverIP, unsigned short port);
	//发送消息
	void sendMessage(const char* data, int count);
	//接受消息的回调
	std::function<void(const char* data, int count)> onRecv;
	//断开了连接的回调
	std::function<void()> onDisconnect;
	//循环调度方法
	void update(float dt);

CC_CONSTRUCTOR_ACCESS:
	SocketClient(void);
	~SocketClient(void);

private:
	//初始化客户端
	bool initClient();
	//接受消息
	void recvMessage();
	//清除
	void clear();

private:
	//服务器套接字
	HSocket _socketServer;
	//客户端套接字
	HSocket _socektClient;
	//消息队列
	std::list<SocketMessage*> _UIMessageQueue;
	//消息队列锁
	std::mutex   _UIMessageQueueMutex;
};

#endif