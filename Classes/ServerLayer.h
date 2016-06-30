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
	//����
	void update(float dt);

protected:
	//��ʼ������
	void initNetwork();
	//��������
	void sendData(DataType type);
	//�ߺ�վ���ķ���
	void runAndStand(const Vec2& pos);

private:
	//����������
	SocketServer* _server;
};

#endif 
