#include "SocketClient.h"

/*
	工厂方法，构造一个客户端
*/
SocketClient* SocketClient::construct()
{
	SocketClient* client = new SocketClient;
	return client;
}

//销毁客户端
void SocketClient::destroy()
{
	delete this;
}
//构造，初始化参数
SocketClient::SocketClient(void) :
	onRecv(nullptr),
	_socektClient(0)
{
}
//析构，清除数据
SocketClient::~SocketClient(void)
{
	this->clear();
}

//断开服务器连接，清除消息队列，解注册调度器
void SocketClient::clear()
{
	if (_socektClient != 0)
	{
		_mutex.lock();
		this->closeConnect(_socektClient);
		_mutex.unlock();
	}

	for (auto msg : _UIMessageQueue)
	{
		CC_SAFE_DELETE(msg);
	}
	_UIMessageQueue.clear();

	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}
//初始化客户端
bool SocketClient::initClient()
{
	this->clear();

	_socektClient = socket(AF_INET, SOCK_STREAM, 0);
	if (error(_socketServer))
	{
		log("init client error!");
		_socektClient = 0;
		return false;
	}
	//注册调度器
	Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
	return true;
}
//连接服务器
bool SocketClient::connectServer(const char* serverIP, unsigned short port)
{
	if (!this->initClient())
	{
		return false;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);

	int ret = 0;
	ret = connect(_socektClient, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		_socektClient = 0;
		return false;
	}

	std::thread recvThread(&SocketClient::recvMessage, this);
	recvThread.detach();

	return true;
}

void SocketClient::recvMessage()
{
	char recvBuf[1024];
	int ret = 0;
	while (true)
	{
		ret = recv(_socektClient, recvBuf, sizeof(recvBuf), 0);
		if (ret < 0)
		{
			log("recv error!");
			break;
		}
		if (ret > 0 && onRecv != nullptr)
		{
			std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
			SocketMessage * msg = new SocketMessage(RECEIVE, (unsigned char*)recvBuf, ret);
			_UIMessageQueue.push_back(msg);
		}
	}
	//接受消息错误，发送断开连接的消息
	_mutex.lock();
	this->closeConnect(_socektClient);
	if (onDisconnect != nullptr)
	{
		std::lock_guard<std::mutex> lk(_UIMessageQueueMutex);
		SocketMessage * msg = new SocketMessage(DISCONNECT);
		_UIMessageQueue.push_back(msg);
	}
	_socektClient = 0;
	_mutex.unlock();
}
//发送消息
void SocketClient::sendMessage(const char* data, int count)
{
	if (_socektClient != 0)
	{
		int ret = send(_socektClient, data, count, 0);
		if (ret < 0)
		{
			log("send error!");
		}
	}
}
//调度方法
void SocketClient::update(float dt)
{
	if (_UIMessageQueue.size() == 0)
	{
		return;
	}

	_UIMessageQueueMutex.lock();

	if (_UIMessageQueue.size() == 0)
	{
		_UIMessageQueueMutex.unlock();
		return;
	}
	//取一条消息
	SocketMessage *msg = *(_UIMessageQueue.begin());
	_UIMessageQueue.pop_front();

	//处理消息
	switch (msg->getMsgType())
	{
	case DISCONNECT:
		if (onDisconnect)
		{
			this->onDisconnect();
		}
		break;
	case RECEIVE:
		if (onRecv)
		{
			this->onRecv((const char*)msg->getMsgData()->getBytes(), msg->getMsgData()->getSize());
		}
		break;
	default:
		break;
	}

	CC_SAFE_DELETE(msg);
	_UIMessageQueueMutex.unlock();
}