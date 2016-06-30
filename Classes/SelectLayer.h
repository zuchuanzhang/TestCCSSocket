#ifndef __SELECT_LAYER_H__
#define __SELECT_LAYER_H__

#include "cocos2d.h"
USING_NS_CC;

class SelectLayer : public Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(SelectLayer);
	//服务器按钮点击
	void menuServerClick(Ref* sender); 
	//客户端按钮点击
	void menuClientClick(Ref* sender);

private:
	void initMenu();

};

#endif 
