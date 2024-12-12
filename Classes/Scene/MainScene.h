#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

struct SceneSwitchPoint {
    cocos2d::Vec2 position;    // 传送点位置
    std::string targetMap;    // 目标地图名称
    bool isActive = true; // 是否激活传送点
};

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    
    void update(float dt);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void PointCenter(cocos2d::Vec2 positon);


    CREATE_FUNC(MainScene);

private:
    cocos2d::TMXTiledMap* map;

    //地图传送点的信息储存
    std::vector<SceneSwitchPoint> sceneSwitchPoints;

    // 当前是否有弹窗
    bool isDialogActive = false; 
};

#endif // __MainScene_SCENE_H__