/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.cpp
 * File Function: 实现小地图(左上角的雷达)功能
 * Author:        叶贤伟
 * Update Date:   2024.12.13
 ****************************************************************/
#include "MiniMap.h"
#include "ui/CocosGUI.h"

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
    isExpanded_ = false;
    zoomLevel_ = 1.0f; // 初始缩放等级
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
   
    // 注册鼠标事件监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(MiniMap::onMouseDown, this);
    mouseListener->onMouseMove = CC_CALLBACK_1(MiniMap::onMouseMove, this);
    mouseListener->onMouseUp = CC_CALLBACK_1(MiniMap::onMouseUp, this);
    mouseListener->onMouseScroll = CC_CALLBACK_1(MiniMap::onMouseScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

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
    else 
    {
        heroMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::GREEN);
        clipper_->addChild(heroMarker_);
    }
    demonMarker_ = DrawNode::create(); // 敌人位置标记
    if (demonMarker_ == nullptr)
    {
        CCLOG("create demonMarker failed");
       
    }
    else 
    {
        demonMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::RED);
        clipper_->addChild(demonMarker_);
    }
    
    // 启动每帧调用 update()
    this->scheduleUpdate();  // 这行会将 update() 注册为每帧调用
};



// 更新小地图标记位置
void MiniMap::update(float dt )
{
    // 检查必要的变量是否初始化
    if (!map_ || !heroMarker_ || !demonMarker_) 
    {
        CCLOG("map_ or markers not properly initialized");
        return;
    }

    // 获取英雄节点
    auto hero = dynamic_cast<Hero*>(scene_->getChildByName("hero"));
    if (!hero) 
    {
        CCLOG("Hero node not found in the scene");
        return;
    }
    
    // 获取敌人节点
    auto demon = dynamic_cast<Enemy*>(scene_->getChildByName("demon"));
    if (!demon)
    {
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


void MiniMap::expandMiniMap()
{
    if (isExpanded_) return; // 防止重复执行
    isExpanded_ = true;

    // 创建放大的窗口背景
    expandedWindow_ = Node::create();
    if (expandedWindow_ == nullptr)
    {
        CCLOG("create expandedWindow failed");
        return;
    }
    expandedWindow_->setContentSize(Size(visibleSize_.width * 0.9f, visibleSize_.height * 0.9f)); // 设置方形窗口尺寸
    expandedWindow_->setAnchorPoint(Vec2(0.5f, 0.5f));
    expandedWindow_->setPosition(visibleSize_.width / 2, visibleSize_.height / 2);

    // 创建裁剪节点
    auto clippingNode = ClippingNode::create();
    if (clippingNode == nullptr)
    {
        CCLOG("create clippingNode failed");
        return;
    }
    clippingNode->setAlphaThreshold(0.5f);  // 设置透明度阈值，确保不透明部分会显示
    // 创建裁剪区域的遮罩
    auto stencil = DrawNode::create();
    if (stencil == nullptr)
    {
        CCLOG("create stencil failed");
        return;
    }

    // 绘制方形裁剪区域
    stencil->drawSolidRect(Vec2::ZERO, expandedWindow_->getContentSize(), Color4F::WHITE);

    clippingNode->setStencil(stencil);  // 设置裁剪区域

    // 设置裁剪节点的大小为窗口大小，确保裁剪区域覆盖整个窗口
    clippingNode->setContentSize(expandedWindow_->getContentSize());
    clippingNode->setPosition(Vec2::ZERO);  // 裁剪节点的位置与窗口对齐

    // 放大地图
    miniMapBackground_2_ = TMXTiledMap::create(file_);
    miniMapBackground_2_->setScale(0.9f); // 放大比例
    miniMapBackground_2_->setAnchorPoint(Vec2(0, 0));
    miniMapBackground_2_->setPosition(Vec2::ZERO);

    // 将放大的地图添加到裁剪节点中
    clippingNode->addChild(miniMapBackground_2_);
    expandedWindow_->addChild(clippingNode);  // 将裁剪节点添加到窗口中

    // 绘制白色边框
    auto border = DrawNode::create();
    if (border == nullptr)
    {
        CCLOG("create border failed");
        return;
    }
    // 绘制矩形边框，颜色为白色
    border->drawRect(Vec2::ZERO, expandedWindow_->getContentSize(), Color4F::WHITE);
    expandedWindow_->addChild(border);  // 将边框添加到窗口中

    // 将放大的窗口添加到场景中
    scene_->addChild(expandedWindow_, 10);

    // 创建关闭按钮
    closeButton_ = cocos2d::ui::Button::create("CloseNormal.png");
    closeButton_->setPosition(Vec2(expandedWindow_->getContentSize().width - 20, expandedWindow_->getContentSize().height - 20));
    closeButton_->addClickEventListener([=](Ref* sender)
        {
            this->closeExpandedMiniMap();
        });
    expandedWindow_->addChild(closeButton_);

    // 阴影遮罩
    shadowLayer_ = LayerColor::create(Color4B(0, 0, 0, 180)); // 半透明黑色遮罩
    shadowLayer_->setContentSize(visibleSize_);
    shadowLayer_->setPosition(0, 0);
    scene_->addChild(shadowLayer_, 4); // 添加阴影层
}



// 关闭放大的小地图
void MiniMap::closeExpandedMiniMap() 
{
    if (!isExpanded_) return; // 防止重复执行
    isExpanded_ = false;

    // 从场景移除放大窗口
    if (expandedWindow_) 
    {
        expandedWindow_->removeAllChildren();
        scene_->removeChild(expandedWindow_);
        expandedWindow_ = nullptr;
    }

    // 移除阴影层
    if (shadowLayer_) 
    {
        scene_->removeChild(shadowLayer_);
        shadowLayer_ = nullptr;
    }

    
}

void MiniMap::onMouseScroll(EventMouse* event)
{
    if (isExpanded_)
    {
        // 获取滚轮的垂直滚动值
        float scrollY = event->getScrollY();

        // 获取鼠标当前位置（屏幕坐标）
        Vec2 mousePos = event->getLocation();

        // 将鼠标位置从屏幕坐标转换为相对于放大地图窗口的局部坐标
        Vec2 localMousePos = expandedWindow_->convertToNodeSpace(mousePos);

        // 计算滚动后的缩放级别
        float newZoom = zoomLevel_ - scrollY * 0.1f;
        zoomLevel_ = clampf(newZoom, 0.9f, 2.0f); // 限制缩放范围

        // 计算缩放比例
        float scaleFactor = zoomLevel_ / zoomLevel_;

        // 获取当前的缩放中心
        Vec2 zoomCenter = localMousePos;

        // 计算新的缩放比例
        miniMapBackground_2_->setScale(zoomLevel_);

        // 更新缩放后的背景位置，使其以鼠标为中心
        Vec2 offset = zoomCenter - localMousePos;
        miniMapBackground_2_->setPosition(miniMapBackground_2_->getPosition() + offset);
    }
}

void MiniMap::onMouseDown(cocos2d::EventMouse* event)
{
    // 检查是否已经展开了小地图
    if (isExpanded_)
    {
        // 如果是左键按下
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        {
            isDragging_ = true;
            dragStartPos_ = event->getLocation(); // 记录起始位置
            dragStartMapPos_ = miniMapBackground_2_->getPosition(); // 记录起始地图位置
        }
    }

    // 获取鼠标点击的位置（屏幕坐标）
    Vec2 clickPos = event->getLocation();

    // 将点击位置转换为相对于小地图裁剪节点的坐标
    Vec2 localClickPos = clipper_->convertToNodeSpace(clickPos);

    // 计算 y 坐标的偏差，确保左上角坐标系统
    localClickPos.y = visibleSize_.height - localClickPos.y;  // 翻转 y 坐标

    // 判断点击位置是否在圆形区域内
    float distance = localClickPos.distance(miniMapCenter_);
    if (distance <= miniMapRadius_)
    {
        // 如果点击位置在圆形遮罩内，且小地图尚未放大，则执行放大操作
        expandMiniMap();
    }
    else
    {
        CCLOG("Click is outside the miniMap circular area.");
    }
}

void MiniMap::onMouseMove(EventMouse* event)
{
    if (isExpanded_ && isDragging_)
    {
        // 计算鼠标拖动的偏移量
        Vec2 dragOffset = event->getLocation() - dragStartPos_;

        // 修正 y 方向的翻转：在 Cocos2d 中，y轴的增大方向是向上，而鼠标事件的坐标系统是相反的
        dragOffset.y = -dragOffset.y;

        // 更新地图的位置
        miniMapBackground_2_->setPosition(dragStartMapPos_ + dragOffset);
    }
}


// 监听鼠标释放事件，结束拖动
void MiniMap::onMouseUp(EventMouse* event)
{
    if (isExpanded_ && event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        isDragging_ = false;
    }

}

