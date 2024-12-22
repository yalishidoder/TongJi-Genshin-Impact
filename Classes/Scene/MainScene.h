#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "Character/PlayerPanel.h"




struct SceneSwitchPoint {
    cocos2d::Vec2 position;    // 传送点位置
    std::string targetMap;    // 目标地图名称
    cocos2d::Vec2 no_position;//拒绝传送后人物微调位置
    bool isActive = true; // 是否激活传送点
};

struct PositionSwitchPoint {
    cocos2d::Vec2 position;    // 传送点位置
    bool isActive = false;
};

class MainScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void update(float dt);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onMouseDown(cocos2d::EventMouse* event);

    void PointCenter(cocos2d::Vec2 positon);

    void showSelectionPopup();
    cocos2d::Vec2 TranslatePos(cocos2d::Vec2 origin);
    //检查位置是否发生碰撞
    bool checkCollision(cocos2d::Vec2 position);
    //窗口到瓦片地图坐标转化
    cocos2d::Vec2 tileCoordForPosition(cocos2d::Vec2 position);
    void hidePopup();

    void operatemyPanel();
    void showmyPanel();
    void hidemyPanel();

    CREATE_FUNC(MainScene);

    // 生成迷雾
    void initFog();
    // 完成任务1
    void completeTask1();
    // 完成任务2
    void completeTask2();

private:
    cocos2d::TMXTiledMap* map;

    //地图切换点的信息储存
    std::vector<SceneSwitchPoint> sceneSwitchPoints;
    //位置传送点的信息储存
    std::vector<PositionSwitchPoint> positionSwitchPoints;
    //当前是否有地图切换点弹窗
    bool isDialogActive = false;
    //当前是否有位置传送点弹窗    
    bool isPopupVisible = false;
    //当前是否显示玩家角色面板
    bool isPanelVisible = false;
    //记录 E 键是否被按下
    bool isKeyPressedE = false;
    //记录 P 键是否被按下（玩家面板）
    bool isKeyPressedP = false;
    //记录 B 键是否被按下（背包）
    bool isKeyPressedB = false;

    // 面板
    PlayerPanel* m_playerPanel = nullptr;

    // 迷雾层
    cocos2d::Layer* fogLowerLayer = nullptr;
    cocos2d::Layer* fogUpperLayer = nullptr;
    // 迷雾层对应的碰撞层
    TMXLayer* fogSandLayer = nullptr;
    TMXLayer* fogTownLayer = nullptr;
};

#endif // __MainScene_SCENE_H__