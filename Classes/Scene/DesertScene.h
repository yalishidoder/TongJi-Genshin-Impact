#ifndef __DESERT_SCENE_H__
#define __DESERT_SCENE_H__

#include "cocos2d.h"

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
private:
	cocos2d::TMXTiledMap* desertmap;

	cocos2d::Sprite* character;  // 在这里定义 character

	bool moveUp, moveDown, moveLeft, moveRight;//按键状态

	//地图传送点的信息储存
	//std::vector<SceneSwitchPoint> sceneSwitchPoints;

};
#endif // __DesertScene_SCENE_H__