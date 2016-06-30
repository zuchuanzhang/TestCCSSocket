#include "GameBase.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace cocostudio::timeline;

//初始化场景
bool GameBase::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	//初始化
	this->initUI();
	this->initTouch();
	this->initEnemy();
	this->initNetwork();

	return true;
}
/*
	初始化UI
*/
void GameBase::initUI()
{
	auto size = Director::getInstance()->getVisibleSize();
	//增加信息标签
	_lblInfo = Label::createWithSystemFont("This is GameBase", "Arial", 24);
	this->addChild(_lblInfo);
	_lblInfo->setPosition(Vec2(size.width * 0.5f, size.height * 0.8f));
}

/*
	初始化点击事件
*/
void GameBase::initTouch()
{
	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->onTouchBegan = CC_CALLBACK_2(GameBase::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(GameBase::onTouchMoved, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(GameBase::onTouchEnded, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

/*
	初始化敌人
*/
void GameBase::initEnemy()
{
	//初始化敌人移动速度
	ENEMY_MOVE_SPEED = 100.f;
	//初始化行走动画
	auto runAnimation = Animation::create();
	for (int i=0; i<5; ++i)
	{
		auto fileName = StringUtils::format("enemy/run%d.png", i);
		runAnimation->addSpriteFrameWithFile(fileName);
	}
	//每秒10帧
	runAnimation->setDelayPerUnit(0.1f);
	_runAnim = Animate::create(runAnimation);
	_runAnim->retain();

	//行走动画
	auto standAnimation = Animation::create();
	for (int i=0; i<3; ++i)
	{
		auto fileName = StringUtils::format("enemy/stand%d.png", i);
		standAnimation->addSpriteFrameWithFile(fileName);
	}
	standAnimation->setDelayPerUnit(0.2f);
	_standAnim = Animate::create(standAnimation);
	_standAnim->retain();

	//敌人精灵
	_enemy = Sprite::create();
	this->addChild(_enemy);
	//执行站立动画
	_enemy->runAction(RepeatForever::create(_standAnim));
}

void GameBase::onEnter()
{
	Layer::onEnter();
}

void GameBase::onExit()
{
	//移除点击事件
	Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
	Layer::onExit();
}

//消费时间
bool GameBase::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void GameBase::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void GameBase::onTouchEnded(Touch *touch, Event *unused_event)
{

}