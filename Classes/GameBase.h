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
	//��ʼ������
	virtual bool init();
	//����ʱ���õķ���
	void onEnter();
	//�˳�ʱ���õķ���
	void onExit();

	//������Ӧ����
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

protected:
	//��ʼ��UI
	void initUI();
	//��ʼ�����
	void initTouch();
	//��ʼ������
	void initEnemy();
	//��ʼ����·
	virtual void initNetwork() = 0;
	//��������
	EventListenerTouchOneByOne* _touchListener;
	//����֡���������ߺ�վ��
	Animate *_runAnim, *_standAnim;
	//���˾���
	Sprite* _enemy;
	//��Ϣ��ǩ
	Label* _lblInfo;
	//���˵��ƶ��ٶ�
	float ENEMY_MOVE_SPEED;
};

#endif 
