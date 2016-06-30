#ifndef __SERVER_LAYER_H__
#define __SERVER_LAYER_H__

#include "GameBase.h"
#include "socket/SocketServer.h"

class ServerLayer : public GameBase
{
public:
    static cocos2d::Scene* createScene();
	virtual bool init();
    CREATE_FUNC(ServerLayer);
	void onEnter();
	void onExit();

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	//更新
	void update(float dt);

protected:
	//初始化网络
	void initNetwork();
	//发送数据
	void sendData(DataType type);
	//走和站立的方法
	void runAndStand(const Vec2& pos);

private:
	//服务器对象
	SocketServer* _server;
};

#endif 
