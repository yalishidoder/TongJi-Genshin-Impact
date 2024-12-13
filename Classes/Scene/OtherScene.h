#ifndef __OTHER_SCENE_H__
#define __OTHER_SCENE_H__
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


	void showSelectionPopup();

	void hidePopup();
private:
	cocos2d::TMXTiledMap* othermap;
	std::string mapname;
	//地图切换点的信息储存
	std::vector<SceneSwitchPoint> sceneSwitchPoints;

	//位置传送点的信息储存
	std::vector<PositionSwitchPoint> positionSwitchPoints;
	//当前是否有地图切换点弹窗
	bool isDialogActive = false;
	//当前是否有位置传送点弹窗    
	bool isPopupVisible = false;
	//记录 E 键是否被按下
	bool isKeyPressedE = false;


};
#endif // __OtherScene_SCENE_H__