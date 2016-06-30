#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include "SocketBase.h"

//接受数据结构封装
struct RecvData
{
	//客户端的套接字
	HSocket socketClient;
	//数据长度
	int dataLen;
	//数据Buffer
	char data[1024];
};

class SocketServer : public SocketBase
{
public:
	//单例服务器
	static SocketServer* getInstance();
	//销毁实例
	void destroyInstance();
	//启动服务器的方法
	bool startServer(unsigned short port);
	//发送消息的方法
	void sendMessage(HSocket socket, const char* data, int count);
	//发送消息的方法
	void sendMessage(const char* data, int count);
	//循环调用方法
	void update(float dt);

	//创建回调函数
	//开始的回调
	std::function<void(const char* ip)> onStart;
	//新连接的回调
	std::function<void(HSocket socket)> onNewConnection;
	//收到数据的回调
	std::function<void(HSocket socket, const char* data, int count)> onRecv;
	//断开连接的回调
	std::function<void(HSocket socket)> onDisconnect;

CC_CONSTRUCTOR_ACCESS:
	SocketServer();
	~SocketServer();

private:
	//清除数据方法
	void clear();
	//初始化服务器方法
	bool initServer(unsigned short port);
	//接受客户端的方法
	void acceptClient();
	//接受方法
	void acceptFunc();
	//新客户端连接的方法
	void newClientConnected(HSocket socket);
	//接受消息的方法
	void recvMessage(HSocket socket);
	
private:
	//服务器单例实例
	static SocketServer* s_server;
	//服务器套接字
	HSocket _socketServer;
	//服务器绑定的端口号
	unsigned short _serverPort;

private:
	//客户列表
	std::list<HSocket> _clientSockets;
	//消息队列
	std::list<SocketMessage*> _UIMessageQueue;
	//消息队列互斥锁
	std::mutex   _UIMessageQueueMutex;

};

#endif