#ifndef __DESERT_SCENE_H__
#define __DESERT_SCENE_H__

#include "cocos2d.h"
#include "Character/Hero/Hero.h"

class DesertScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void update(float dt);

	void returnToLastScene();

	CREATE_FUNC(DesertScene);

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
private:
	cocos2d::TMXTiledMap* desertmap;

	//地图传送点的信息储存
	//std::vector<SceneSwitchPoint> sceneSwitchPoints;

};
#endif // __DesertScene_SCENE_H__