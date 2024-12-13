/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.h
 * File Function: 实现小地图(左上角的雷达)功能的类声明
 * Author:        叶贤伟
 * Update Date:   2024.12.13
 ****************************************************************/

#ifndef MINIMAP_H
#define MINIMAP_H

#include "cocos2d.h"
#include "Character/Hero/Hero.h"
#include "Character/Enemy/Enemy.h"
USING_NS_CC;

class MiniMap : public cocos2d::Node {
public:
    // Factory method to create the MiniMap
    static MiniMap* create(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene);

    /*  
       创造小地图
       参数：地图文件名，屏幕大小，场景节点
   */
    bool init(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene);

    /*  
       设置小地图上的角色位置并进行同步
       参数：无
       */
    void setCharacter();



    /*
        同步小地图上的角色位置
        参数：无
    */
    void update(float dt);

private:
    // 主地图
    cocos2d::TMXTiledMap* map_;
    // 主地图文件名
    std::string file_;
    // 屏幕大小
    cocos2d::Size visibleSize_;
    // 主地图的场景节点
    cocos2d::Scene* scene_;
    // 小地图节点
    cocos2d::Node* miniMap_;
    // 小地图缩放比例
    float miniMapScale_;
    // 小地图圆形边框半径
    float miniMapRadius_;
    // 小地图中心位置
    cocos2d::Vec2 miniMapCenter_;
    // 小地图的裁剪节点
    cocos2d::ClippingNode* clipper_;
    // 小地图的边框
    cocos2d::DrawNode* miniMapBorder_;
    // 小地图的背景
    cocos2d::TMXTiledMap * miniMapBackground_;
    // 小地图的主角
    Character* hero_;
    // 小地图的敌人
    Enemy* demon_;
    // 小地图的主角标记节点
    DrawNode* heroMarker_;
    // 小地图的敌人标记节点
    DrawNode* demonMarker_;
};

#endif // MINIMAP_H
