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
	//ע�������
	this->scheduleUpdate();
	_lblInfo->setString("This is Server");
	log("server");
	return true;
}
//��ʼ������
void ServerLayer::initNetwork()
{
	//��������������8000�˿�
	_server = SocketServer::getInstance();
	_server->startServer(8000);
}
//�������ݣ����˵������
void ServerLayer::sendData(DataType type)
{
	GameData data;
	data.dataType = type;
	data.dataSize = sizeof(GameData);
	data.position = _enemy->getPosition();
	_server->sendMessage((const char*)&data, sizeof(GameData));
}

//�ܻ���վ��
void ServerLayer::runAndStand(const Vec2& pos)
{
	float distance = _enemy->getPosition().getDistance(pos);
	auto moveTo = MoveTo::create(distance / ENEMY_MOVE_SPEED, pos);
	auto standCall = CallFunc::create([=]{
		this->sendData(STAND);
		_enemy->stopAllActions();
		_enemy->runAction(RepeatForever::create(_standAnim));
	});

	this->sendData(RUN); //������������
	_enemy->stopAllActions();
	_enemy->setFlippedX(pos.x < _enemy->getPosition().x);
	_enemy->runAction(RepeatForever::create(_runAnim));
	//���߽���������վ���Ļص�
	_enemy->runAction(Sequence::createWithTwoActions(moveTo, standCall));
}

void ServerLayer::onEnter()
{
	GameBase::onEnter();
}

void ServerLayer::onExit()
{	
	//�˳����ٷ�����ʵ��
	_server->destroyInstance();
	GameBase::onExit();
}
//�����¼�����
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

//������
void ServerLayer::update(float dt)
{
	CCLOG("ServerLayer::update(),dt:%f", dt);
	static float curTime = 0;
	curTime += dt;
	CCLOG("ServerLayer::update(),dt:%f��curTime:%f", dt,curTime);
	//�ۼ��¼����ڵ���2��ʱ��
	if (curTime > dt * 2.f)
	{
		//����һ������
		this->sendData(POSITION); //����λ������
		//��ȥ�����
		curTime -= dt * 2.f;
		CCLOG("ServerLayer::update(),dt:%f��curTime:%f===========>>>", dt,curTime);
	}
}