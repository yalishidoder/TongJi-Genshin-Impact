#ifndef __DESERT_SCENE_H__
#define __DESERT_SCENE_H__
#include "Scene/MainScene.h"
#include "cocos2d.h"
#include "Character/Hero/Hero.h"


class OtherScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene(const std::string& mapFile);

	virtual bool init(const std::string& mapFile);
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void update(float dt);

	void returnToLastScene();

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
private:
	cocos2d::TMXTiledMap* othermap;
	std::string mapname;
	//地图传送点的信息储存
	std::vector<SceneSwitchPoint> sceneSwitchPoints;

	// 当前是否有弹窗
	bool isDialogActive = false;

};
#endif // __OtherScene_SCENE_H__