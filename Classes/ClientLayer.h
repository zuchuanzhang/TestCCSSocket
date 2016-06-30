#ifndef __CLIENT_LAYER_H__
#define __CLIENT_LAYER_H__

#include "GameBase.h"
#include "socket/SocketClient.h"

class ClientLayer : public GameBase
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(ClientLayer);

	//进入与退出
	void onEnter();
	void onExit();

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	//接受数据的方法
	void onRecv(const char* data, int count);
	//断开连接的方法
	void onDisconnect();

protected:
	//初始化网络
	void initNetwork();
	//敌人精灵集合
	std::vector<Sprite*> _enemies;
	//Socket客户端
	SocketClient* _client;
};

#endif 
