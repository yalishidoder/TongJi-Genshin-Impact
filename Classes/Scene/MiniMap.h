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
#include "ui/CocosGUI.h"

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

    void onMouseDown(EventMouse* event); // 鼠标点击事件处理函数
    void expandMiniMap();                          // 放大小地图功能
    void closeExpandedMiniMap();                   // 关闭放大的小地图
    void onMouseScroll(EventMouse* event);
    // 监听鼠标移动事件，实现拖动
    void onMouseMove(EventMouse* event);
    void onMouseUp(EventMouse* event);


private:
    // 主地图
    TMXTiledMap* map_;
    // 主地图文件名
    std::string file_;
    // 屏幕大小
    Size visibleSize_;
    // 主地图的场景节点
    Scene* scene_;
    // 小地图节点
    Node* miniMap_;
    // 小地图缩放比例
    float miniMapScale_;
    // 小地图圆形边框半径
    float miniMapRadius_;
    // 小地图中心位置
    Vec2 miniMapCenter_;
    // 小地图的裁剪节点
    ClippingNode* clipper_;
    // 小地图的边框
    DrawNode* miniMapBorder_;
    // 小地图的背景
    TMXTiledMap * miniMapBackground_;
    // 小地图的主角
    Hero* hero_;
    // 小地图的敌人
    Enemy* demon_;
    // 小地图的主角标记节点
    DrawNode* heroMarker_;
    // 小地图的敌人标记节点
    DrawNode* demonMarker_;

    // 小地图的背景
    TMXTiledMap* miniMapBackground_2_;
    // 小地图是否被放大
    bool isExpanded_;      
    // 阴影遮罩层
    LayerColor* shadowLayer_;    
    // 关闭按钮
    cocos2d::ui::Button* closeButton_;
    // 方形窗口
    Node* expandedWindow_;   
    // 地图缩放等级
    float zoomLevel_;       
    // 是否正在拖动
    bool isDragging_ = false;   
    // 鼠标按下时的位置
    Vec2 dragStartPos_;    
    // 鼠标按下时的地图位置
    Vec2 dragStartMapPos_;       

    
};

#endif // MINIMAP_H
