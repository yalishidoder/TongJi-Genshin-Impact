/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.cpp
 * File Function: 实现小地图(左上角的雷达)功能
 * Author:        叶贤伟
 * Update Date:   2024.12.13
 ****************************************************************/
#include "MiniMap.h"

USING_NS_CC;

// 创建 MiniMap 对象的工厂方法
MiniMap* MiniMap::create(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene) {
    MiniMap* ret = new (std::nothrow) MiniMap();
    if (ret && ret->init(map, file, visibleSize, scene)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

// 初始化方法
bool MiniMap::init(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene) {
    if (!Node::init()) {
        return false;
    }
    //初始化变量
    scene_ = scene;
    file_ = file;
    visibleSize_ = visibleSize;
    map_ = map;
    /////////////////////////////
        // 添加小地图
        /////////////////////////////
    miniMap_ = Node::create(); // 创建小地图节点
    if (miniMap_ == nullptr)
    {
        CCLOG("create miniMap failed");
        return false;
    }
    miniMapScale_ = 0.45f;     // 小地图缩放比例，根据实际地图大小调整
    miniMapRadius_ = 100.0f; // 小地图圆形边框半径
    miniMapCenter_ = Vec2(miniMapRadius_ + 10, visibleSize_.height - miniMapRadius_ - 10); // 小地图中心位置

    // 创建裁剪节点用于实现圆形遮罩
    clipper_ = ClippingNode::create();
    if (clipper_ == nullptr)
    {
        CCLOG("create clipper failed");
        return false;
    }
    clipper_->setPosition(Vec2::ZERO); // 设置裁剪节点位置
    clipper_->setAlphaThreshold(0.5f); // 不透明部分保留，透明部分裁剪

    // 创建圆形遮罩
    auto stencil = DrawNode::create();
    if (stencil == nullptr)
    {
        CCLOG("create stencil failed");
        return false;
    }
    stencil->drawSolidCircle(miniMapCenter_, miniMapRadius_, CC_DEGREES_TO_RADIANS(360), 100, Color4F::WHITE);
    clipper_->setStencil(stencil);

    // 缩小主地图并添加到裁剪节点中
    miniMapBackground_ = TMXTiledMap::create(file);
    if (miniMapBackground_ == nullptr)
    {
        CCLOG("create miniMapBackground failed");
        return false;
    }

    miniMapBackground_->setScale(map_->getScale() * miniMapScale_);
    miniMapBackground_->setAnchorPoint(Vec2(0, 0));
    miniMapBackground_->setPosition(Vec2(miniMapCenter_.x - miniMapBackground_->getContentSize().width * miniMapScale_ / 2,
        miniMapCenter_.y - miniMapBackground_->getContentSize().height * miniMapScale_ / 2));
    clipper_->addChild(miniMapBackground_);


    scene_->addChild(clipper_, 6); // 添加裁剪节点到场景中

    // 绘制小地图边框
    miniMapBorder_ = DrawNode::create();
    if (miniMapBorder_ == nullptr)
    {
        CCLOG("create miniMapBorder failed");
        return false;
    }
    miniMapBorder_->drawCircle(miniMapCenter_, miniMapRadius_, CC_DEGREES_TO_RADIANS(360), 100, false, Color4F::WHITE);
    miniMapBorder_->drawSolidCircle(miniMapCenter_, miniMapRadius_, CC_DEGREES_TO_RADIANS(360), 100, Color4F(0, 0, 0, 1)); // 半透明背景
    scene_->addChild(miniMapBorder_, 5); // 添加边框到场景中

    // 同步小地图上的角色位置和视图区域
    setCharacter();
   
    return true;
}
void MiniMap::setCharacter()
{
    // 同步主地图角色到小地图
    heroMarker_ = DrawNode::create(); // 英雄位置标记
    if (heroMarker_ == nullptr)
    {
        CCLOG("create heroMarker failed");
    }
    else {
        heroMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::GREEN);
        clipper_->addChild(heroMarker_);
    }
    demonMarker_ = DrawNode::create(); // 敌人位置标记
    if (demonMarker_ == nullptr)
    {
        CCLOG("create demonMarker failed");
       
    }
    else {
        demonMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::RED);
        clipper_->addChild(demonMarker_);
    }
    
    // 启动每帧调用 update()
    this->scheduleUpdate();  // 这行会将 update() 注册为每帧调用
};



// 更新小地图标记位置
void MiniMap::update(float dt ) {
    // 检查必要的变量是否初始化
    if (!map_ || !heroMarker_ || !demonMarker_) {
        CCLOG("map_ or markers not properly initialized");
        return;
    }

    // 获取英雄节点
    auto hero = dynamic_cast<Character*>(scene_->getChildByName("hero"));
    if (!hero) {
        CCLOG("Hero node not found in the scene");
        return;
    }
    
    // 获取敌人节点
    auto demon = dynamic_cast<Enemy*>(scene_->getChildByName("demon"));
    if (!demon) {
        CCLOG("Demon node not found in the scene");
    }

    // 更新英雄的位置
    Vec2 heroPos = hero->getPosition();
    Vec2 miniHeroPos = (heroPos - map_->getPosition() + Vec2(map_->getContentSize().width / 2, map_->getContentSize().height / 2)) * miniMapScale_;
    heroMarker_->setPosition(miniMapCenter_ - miniHeroPos + heroPos * miniMapScale_);

    // 更新小地图视图位置
    Vec2 offset = miniMapCenter_ - miniHeroPos;
    miniMapBackground_->setPosition(offset);

    //有敌人时更新敌人的位置
    if (demon) 
    {
        // 更新敌人的位置
        Vec2 demonPos = demon->getPosition();
        Vec2 miniDemonPos = (demonPos - map_->getPosition() + Vec2(map_->getContentSize().width / 2, map_->getContentSize().height / 2)) * miniMapScale_;
        demonMarker_->setPosition(miniMapCenter_ - miniHeroPos + demonPos * miniMapScale_);
    }
}
