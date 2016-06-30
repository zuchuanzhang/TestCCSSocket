#include "SocketServer.h"

//初始化服务器单例实例
SocketServer* SocketServer::s_server = nullptr;
//取得服务器实例
SocketServer* SocketServer::getInstance()
{
	if (s_server == nullptr)
	{
		s_server = new SocketServer;
	}

	return s_server;
}

//销毁服务器实例
void SocketServer::destroyInstance()
{
	CC_SAFE_DELETE(s_server);
}
//构造方法，初始化套接字，回调函数
SocketServer::SocketServer() :
	_socketServer(0),
	onRecv(nullptr),
	onStart(nullptr),
	onNewConnection(nullptr)
{

}
//析构方法，清除数据
SocketServer::~SocketServer()
{
	this->clear();
};
//清除服务器数据
void SocketServer::clear()
{
	//如果有服务器实例
	if (_socketServer)
	{
		//锁住
		_mutex.lock();
		//关闭连接
		this->closeConnect(_socketServer);
		//解锁
		_mutex.unlock();
	}
	//清空消息队列
	for (auto msg : _UIMessageQueue)
	{
		CC_SAFE_DELETE(msg);
	}
	_UIMessageQueue.clear();
	//解除所有定时器
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}
/*
	启动服务器
*/
bool SocketServer::startServer(unsigned short port)
{
	if (!initServer(port))
	{
		return false;
	}

	return true;
}

/*
	初始化服务器
*/
bool SocketServer::initServer(unsigned short port)
{
	//如果存在服务器套接字，先关闭连接
	if (_socketServer != 0)
	{
		this->closeConnect(_socketServer);
	}
	//取得套接字
	_socketServer = socket(AF_INET, SOCK_STREAM, 0);
	//验证套接字是否正确
	if (error(_socketServer))
	{
		log("socket error!");
		_socketServer = 0;
		return false;
	}

	do 
	{
		//服务器地址
		struct sockaddr_in sockAddr;
		//初始化结构体
		memset(&sockAddr, 0, sizeof(sockAddr));
		//设定端口号
		_serverPort = port;		// save the port

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(_serverPort);
		//自动设定机器的IP
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		int ret = 0;
		//绑定接口
		ret = bind(_socketServer, (const sockaddr*)&sockAddr, sizeof(sockAddr));
		if (ret < 0)
		{
			log("bind error!");
			break;
		}
		//监听，允许连接数
		ret = listen(_socketServer, 5);
		if (ret < 0)
		{
			log("listen error!");
			break;
		}
		// start 取得主机信息 
		char hostName[256];
		gethostname(hostName, sizeof(hostName));
		struct hostent* hostInfo = gethostbyname(hostName);
		char* ip = inet_ntoa(*(struct in_addr *)*hostInfo->h_addr_list);
		//启动接受客户端线程
		this->acceptClient();
		//调用onStart()生命周期
		if (onStart != nullptr)
		{
			log("start server!");
			this->onStart(ip);
		}

		return true;

	} while (false);
	//上面没返回就是失败了
	//关闭连接
	this->closeConnect(_socketServer);
	_socketServer = 0;
	return false;
}
//接受客户端
void SocketServer::acceptClient()
{
	std::thread th(&SocketServer::acceptFunc, this);
	//开始线程
	th.detach();
}
//接受的线程
void SocketServer::acceptFunc()
{
	int len = sizeof(sockaddr);
	struct sockaddr_in sockAddr;
	//轮询接受客户端连接
	while (true)
	{	
		HSocket clientSock = accept(_socketServer, (sockaddr*)&sockAddr, &len);
		if (error(clientSock))
		{
			log("accept error!");
		}
		else
		{
			this->newClientConnected(clientSock);
		}
	}
}
/*
	新的客户端连接进来调用的方法
*/
void SocketServer::newClientConnected(HSocket socket)
{
	log("new connect!");
	//将套接字放到队列中
	_clientSockets.push_back(socket);
	//启动一个接受消息的线程
	std::thread th(&SocketServer::recvMessage, this, socket);
	th.detach();

	//新连接的回调
	if (onNewConnection)
	{
		//方法锁
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		//填加新连接消息，到消息队列
		SocketMessage * msg = new SocketMessage(NEW_CONNECTION, (unsigned char*)&socket, sizeof(HSocket));
		_UIMessageQueue.push_back(msg);
	}
}
/*
	处理接收到的消息
*/
void SocketServer::recvMessage(HSocket socket)
{
	//创建buff
	char buff[1024];
	int ret = 0;
	//循环接受消息
	while (true)
	{
		ret = recv(socket, buff, sizeof(buff), 0);
		if (ret < 0) //接收错误
		{
			log("recv(%d) error!", socket);
			break;
		}
		else  
		{
			if (ret > 0 && onRecv != nullptr)
			{
				//方法锁
				std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
				//接收的数据
				RecvData recvData;
				//构建接受消息，并加入队列
				recvData.socketClient = socket;
				memcpy(recvData.data, buff, ret);
				recvData.dataLen = ret;
				SocketMessage * msg = new SocketMessage(RECEIVE, (unsigned char*)&recvData, sizeof(RecvData));
				_UIMessageQueue.push_back(msg);
			}
		}
	}

	_mutex.lock();
	this->closeConnect(socket);
	if (onDisconnect != nullptr)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(DISCONNECT, (unsigned char*)&socket, sizeof(HSocket));
		_UIMessageQueue.push_back(msg);
	}
	_mutex.unlock();
}

//发送数据方法
void SocketServer::sendMessage(HSocket socket, const char* data, int count)
{
	for (auto& sock : _clientSockets) //遍历所有连接的数据
	{
		if (sock == socket)
		{
			int ret = send(socket, data, count, 0);
			if (ret < 0) //发送错误
			{
				log("send error!");
			}
			break;
		}
	}
}
//广播发送
void SocketServer::sendMessage(const char* data, int count)
{
	for (auto& socket : _clientSockets)
	{
		int ret = send(socket, data, count, 0);
		if (ret < 0)
		{
			log("send error!");
		}
	}
}
//调度方法
void SocketServer::update(float dt)
{
	if (_UIMessageQueue.size() == 0)
	{
		return;
	}

	//队列锁住
	_UIMessageQueueMutex.lock();

	if (_UIMessageQueue.size() == 0)
	{
		_UIMessageQueueMutex.unlock();
		return;
	}
	//取一条消息
	SocketMessage *msg = *(_UIMessageQueue.begin());
	_UIMessageQueue.pop_front();

	//辨别消息类型，调用回调方法
	switch (msg->getMsgType())
	{
	case NEW_CONNECTION:
		if (onNewConnection)
		{
			this->onNewConnection(*(HSocket*)msg->getMsgData()->getBytes());
		}
		break;
	case DISCONNECT:
		if (onDisconnect)
		{
			this->onDisconnect(*(HSocket*)msg->getMsgData()->getBytes());
		}
		break;
	case RECEIVE:
		if (onRecv)
		{
			RecvData* recvData = (RecvData*)msg->getMsgData()->getBytes();
			this->onRecv(recvData->socketClient, (const char*)recvData->data, recvData->dataLen);
		}
		break;
	default:
		break;
	}
	//清除消息
	CC_SAFE_DELETE(msg);
	//释放锁
	_UIMessageQueueMutex.unlock();
}