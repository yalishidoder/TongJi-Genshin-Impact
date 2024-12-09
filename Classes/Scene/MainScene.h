
#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    //手动实现“static create()”方法
    CREATE_FUNC(MainScene);
private:
    cocos2d::TMXTiledMap* map;

};

#endif // __MAIN_SCENE_H__