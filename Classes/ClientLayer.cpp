#include "ClientLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace cocostudio::timeline;

Scene* ClientLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = ClientLayer::create();

	scene->addChild(layer);

	return scene;
}

bool ClientLayer::init()
{
	if ( !GameBase::init() )
	{
		return false;
	}

	log("client");
	return true;
}
//连接网络
void ClientLayer::initNetwork()
{
	//构造一个客户端
	_client = SocketClient::construct();
	//设置回调方法
	_client->onRecv = CC_CALLBACK_2(ClientLayer::onRecv, this);
	_client->onDisconnect = CC_CALLBACK_0(ClientLayer::onDisconnect, this);
	//信息标签
	_lblInfo->setString("This is Client");
	//连接服务器
	if (!_client->connectServer("127.0.0.1", 8000))
	{
		_lblInfo->setString("Client connect error");
	}
}

void ClientLayer::onEnter()
{
	GameBase::onEnter();
}

void ClientLayer::onExit()
{
	_client->destroy();
	_client = nullptr;

	GameBase::onExit();
}

bool ClientLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	
	return true;
}

void ClientLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void ClientLayer::onTouchEnded(Touch *touch, Event *unused_event)
{

}

//接受到消息，进行处理
void ClientLayer::onRecv(const char* data, int count)
{
	GameData* gameData = (GameData*)data;
	if (gameData->dataSize == sizeof(GameData))	//数据包长度一致，为正确的包
	{
		switch (gameData->dataType)
		{
		case RUN:
			//运行动画
			_enemy->stopAllActions();
			_enemy->runAction(RepeatForever::create(_runAnim));
			break;
		case STAND:
			_enemy->stopAllActions();
			_enemy->runAction(RepeatForever::create(_standAnim));
			break;
		case POSITION:
			if (gameData->position.x < _enemy->getPositionX())	//判断x坐标大小是否需要镜像
				_enemy->setFlippedX(true);
			else if (fabs(gameData->position.x  - _enemy->getPositionX()) > MATH_EPSILON)
				_enemy->setFlippedX(false);
			_enemy->setPosition(gameData->position);//设置精灵位置
			break;

		default:
			break;
		}
	}
	
}

void ClientLayer::onDisconnect()
{
	_lblInfo->setString("Client disconnect");
}