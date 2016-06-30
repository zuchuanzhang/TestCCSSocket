#ifndef __GAME_BASE_H__
#define __GAME_BASE_H__

#include "cocos2d.h"
USING_NS_CC;

enum DataType
{
	RUN,
	STAND,
	POSITION
};

struct GameData
{
	int dataSize;
	DataType dataType;	
	Vec2 position;
};

class GameBase : public Layer
{
public:
	//初始化方法
	virtual bool init();
	//进入时调用的方法
	void onEnter();
	//退出时调用的方法
	void onExit();

	//触摸相应方法
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

protected:
	//初始化UI
	void initUI();
	//初始化点击
	void initTouch();
	//初始化敌人
	void initEnemy();
	//初始化网路
	virtual void initNetwork() = 0;
	//触摸监听
	EventListenerTouchOneByOne* _touchListener;
	//序列帧动画，行走和站立
	Animate *_runAnim, *_standAnim;
	//敌人精灵
	Sprite* _enemy;
	//信息标签
	Label* _lblInfo;
	//敌人的移动速度
	float ENEMY_MOVE_SPEED;
};

#endif 
