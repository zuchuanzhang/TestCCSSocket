#include "ServerLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace cocostudio::timeline;

Scene* ServerLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = ServerLayer::create();

	scene->addChild(layer);

	return scene;
}

bool ServerLayer::init()
{
	if (!GameBase::init())
	{
		return false;
	}
	//注册调度器
	this->scheduleUpdate();
	_lblInfo->setString("This is Server");
	log("server");
	return true;
}
//初始化网络
void ServerLayer::initNetwork()
{
	//启动服务器，绑定8000端口
	_server = SocketServer::getInstance();
	_server->startServer(8000);
}
//发送数据，敌人的坐标点
void ServerLayer::sendData(DataType type)
{
	GameData data;
	data.dataType = type;
	data.dataSize = sizeof(GameData);
	data.position = _enemy->getPosition();
	_server->sendMessage((const char*)&data, sizeof(GameData));
}

//跑或者站立
void ServerLayer::runAndStand(const Vec2& pos)
{
	float distance = _enemy->getPosition().getDistance(pos);
	auto moveTo = MoveTo::create(distance / ENEMY_MOVE_SPEED, pos);
	auto standCall = CallFunc::create([=]{
		this->sendData(STAND);
		_enemy->stopAllActions();
		_enemy->runAction(RepeatForever::create(_standAnim));
	});

	this->sendData(RUN); //发送行走数据
	_enemy->stopAllActions();
	_enemy->setFlippedX(pos.x < _enemy->getPosition().x);
	_enemy->runAction(RepeatForever::create(_runAnim));
	//行走结束，调用站立的回调
	_enemy->runAction(Sequence::createWithTwoActions(moveTo, standCall));
}

void ServerLayer::onEnter()
{
	GameBase::onEnter();
}

void ServerLayer::onExit()
{	
	//退出销毁服务器实例
	_server->destroyInstance();
	GameBase::onExit();
}
//触摸事件处理
bool ServerLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	this->runAndStand(touch->getLocation());
	return true;
}

void ServerLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
	
}

void ServerLayer::onTouchEnded(Touch *touch, Event *unused_event)
{

}

//调度器
void ServerLayer::update(float dt)
{
	CCLOG("ServerLayer::update(),dt:%f", dt);
	static float curTime = 0;
	curTime += dt;
	CCLOG("ServerLayer::update(),dt:%f，curTime:%f", dt,curTime);
	//累计事件大于调度2倍时间
	if (curTime > dt * 2.f)
	{
		//发送一次数据
		this->sendData(POSITION); //发送位置数据
		//减去多余的
		curTime -= dt * 2.f;
		CCLOG("ServerLayer::update(),dt:%f，curTime:%f===========>>>", dt,curTime);
	}
}