#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

struct SceneSwitchPoint {
    cocos2d::Vec2 position;    // 传送点位置
    std::string targetMap;    // 目标地图名称
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

    void changeScene(const std::string& targetMap);

    void returnToMainScene();

    CREATE_FUNC(MainScene);

private:
    cocos2d::TMXTiledMap* map;
    cocos2d::Sprite* character;  // 在这里定义 character
    bool moveUp, moveDown, moveLeft, moveRight;//按键状态

    //地图传送点的信息储存
    std::vector<SceneSwitchPoint> sceneSwitchPoints;
};

#endif // __MainScene_SCENE_H__