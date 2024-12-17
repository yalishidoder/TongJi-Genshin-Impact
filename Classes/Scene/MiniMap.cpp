/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.cpp
 * File Function: ʵ��С��ͼ(���Ͻǵ��״�)����
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.13
 ****************************************************************/
#include "MiniMap.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

// ���� MiniMap ����Ĺ�������
MiniMap* MiniMap::create(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene) {
    MiniMap* ret = new (std::nothrow) MiniMap();
    if (ret && ret->init(map, file, visibleSize, scene)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

// ��ʼ������
bool MiniMap::init(TMXTiledMap* map, std::string file, Size visibleSize, Scene* scene) {
    if (!Node::init()) {
        return false;
    }
    //��ʼ������
    scene_ = scene;
    file_ = file;
    visibleSize_ = visibleSize;
    map_ = map;
    isExpanded_ = false;
    zoomLevel_ = 1.0f; // ��ʼ���ŵȼ�
    /////////////////////////////
        // ���С��ͼ
        /////////////////////////////
    miniMap_ = Node::create(); // ����С��ͼ�ڵ�
    if (miniMap_ == nullptr)
    {
        CCLOG("create miniMap failed");
        return false;
    }
    miniMapScale_ = 0.45f;     // С��ͼ���ű���������ʵ�ʵ�ͼ��С����
    miniMapRadius_ = 100.0f; // С��ͼԲ�α߿�뾶
    miniMapCenter_ = Vec2(miniMapRadius_ + 10, visibleSize_.height - miniMapRadius_ - 10); // С��ͼ����λ��

    // �����ü��ڵ�����ʵ��Բ������
    clipper_ = ClippingNode::create();
    if (clipper_ == nullptr)
    {
        CCLOG("create clipper failed");
        return false;
    }
    clipper_->setPosition(Vec2::ZERO); // ���òü��ڵ�λ��
    clipper_->setAlphaThreshold(0.5f); // ��͸�����ֱ�����͸�����ֲü�

    // ����Բ������
    auto stencil = DrawNode::create();
    if (stencil == nullptr)
    {
        CCLOG("create stencil failed");
        return false;
    }
    stencil->drawSolidCircle(miniMapCenter_, miniMapRadius_, CC_DEGREES_TO_RADIANS(360), 100, Color4F::WHITE);
    clipper_->setStencil(stencil);

    // ��С����ͼ����ӵ��ü��ڵ���
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


    scene_->addChild(clipper_, 6); // ��Ӳü��ڵ㵽������

    // ����С��ͼ�߿�
    miniMapBorder_ = DrawNode::create();
    if (miniMapBorder_ == nullptr)
    {
        CCLOG("create miniMapBorder failed");
        return false;
    }
    miniMapBorder_->drawCircle(miniMapCenter_, miniMapRadius_, CC_DEGREES_TO_RADIANS(360), 100, false, Color4F::WHITE);
    miniMapBorder_->drawSolidCircle(miniMapCenter_, miniMapRadius_, CC_DEGREES_TO_RADIANS(360), 100, Color4F(0, 0, 0, 1)); // ��͸������
    scene_->addChild(miniMapBorder_, 5); // ��ӱ߿򵽳�����

    // ͬ��С��ͼ�ϵĽ�ɫλ�ú���ͼ����
    setCharacter();
   
    // ע������¼�������
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
    // ͬ������ͼ��ɫ��С��ͼ
    heroMarker_ = DrawNode::create(); // Ӣ��λ�ñ��
    if (heroMarker_ == nullptr)
    {
        CCLOG("create heroMarker failed");
    }
    else 
    {
        heroMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::GREEN);
        clipper_->addChild(heroMarker_);
    }
    demonMarker_ = DrawNode::create(); // ����λ�ñ��
    if (demonMarker_ == nullptr)
    {
        CCLOG("create demonMarker failed");
       
    }
    else 
    {
        demonMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::RED);
        clipper_->addChild(demonMarker_);
    }
    
    // ����ÿ֡���� update()
    this->scheduleUpdate();  // ���лὫ update() ע��Ϊÿ֡����
};



// ����С��ͼ���λ��
void MiniMap::update(float dt )
{
    // ����Ҫ�ı����Ƿ��ʼ��
    if (!map_ || !heroMarker_ || !demonMarker_) 
    {
        CCLOG("map_ or markers not properly initialized");
        return;
    }

    // ��ȡӢ�۽ڵ�
    auto hero = dynamic_cast<Hero*>(scene_->getChildByName("hero"));
    if (!hero) 
    {
        CCLOG("Hero node not found in the scene");
        return;
    }
    
    // ��ȡ���˽ڵ�
    auto demon = dynamic_cast<Enemy*>(scene_->getChildByName("demon"));
    if (!demon)
    {
        CCLOG("Demon node not found in the scene");
    }

    // ����Ӣ�۵�λ��
    Vec2 heroPos = hero->getPosition();
    Vec2 miniHeroPos = (heroPos - map_->getPosition() + Vec2(map_->getContentSize().width / 2, map_->getContentSize().height / 2)) * miniMapScale_;
    heroMarker_->setPosition(miniMapCenter_ - miniHeroPos + heroPos * miniMapScale_);

    // ����С��ͼ��ͼλ��
    Vec2 offset = miniMapCenter_ - miniHeroPos;
    miniMapBackground_->setPosition(offset);

    //�е���ʱ���µ��˵�λ��
    if (demon) 
    {
        // ���µ��˵�λ��
        Vec2 demonPos = demon->getPosition();
        Vec2 miniDemonPos = (demonPos - map_->getPosition() + Vec2(map_->getContentSize().width / 2, map_->getContentSize().height / 2)) * miniMapScale_;
        demonMarker_->setPosition(miniMapCenter_ - miniHeroPos + demonPos * miniMapScale_);
    }
}


void MiniMap::expandMiniMap()
{
    if (isExpanded_) return; // ��ֹ�ظ�ִ��
    isExpanded_ = true;

    // �����Ŵ�Ĵ��ڱ���
    expandedWindow_ = Node::create();
    if (expandedWindow_ == nullptr)
    {
        CCLOG("create expandedWindow failed");
        return;
    }
    expandedWindow_->setContentSize(Size(visibleSize_.width * 0.9f, visibleSize_.height * 0.9f)); // ���÷��δ��ڳߴ�
    expandedWindow_->setAnchorPoint(Vec2(0.5f, 0.5f));
    expandedWindow_->setPosition(visibleSize_.width / 2, visibleSize_.height / 2);

    // �����ü��ڵ�
    auto clippingNode = ClippingNode::create();
    if (clippingNode == nullptr)
    {
        CCLOG("create clippingNode failed");
        return;
    }
    clippingNode->setAlphaThreshold(0.5f);  // ����͸������ֵ��ȷ����͸�����ֻ���ʾ
    // �����ü����������
    auto stencil = DrawNode::create();
    if (stencil == nullptr)
    {
        CCLOG("create stencil failed");
        return;
    }

    // ���Ʒ��βü�����
    stencil->drawSolidRect(Vec2::ZERO, expandedWindow_->getContentSize(), Color4F::WHITE);

    clippingNode->setStencil(stencil);  // ���òü�����

    // ���òü��ڵ�Ĵ�СΪ���ڴ�С��ȷ���ü����򸲸���������
    clippingNode->setContentSize(expandedWindow_->getContentSize());
    clippingNode->setPosition(Vec2::ZERO);  // �ü��ڵ��λ���봰�ڶ���

    // �Ŵ��ͼ
    miniMapBackground_2_ = TMXTiledMap::create(file_);
    miniMapBackground_2_->setScale(0.9f); // �Ŵ����
    miniMapBackground_2_->setAnchorPoint(Vec2(0, 0));
    miniMapBackground_2_->setPosition(Vec2::ZERO);

    // ���Ŵ�ĵ�ͼ��ӵ��ü��ڵ���
    clippingNode->addChild(miniMapBackground_2_);
    expandedWindow_->addChild(clippingNode);  // ���ü��ڵ���ӵ�������

    // ���ư�ɫ�߿�
    auto border = DrawNode::create();
    if (border == nullptr)
    {
        CCLOG("create border failed");
        return;
    }
    // ���ƾ��α߿���ɫΪ��ɫ
    border->drawRect(Vec2::ZERO, expandedWindow_->getContentSize(), Color4F::WHITE);
    expandedWindow_->addChild(border);  // ���߿���ӵ�������

    // ���Ŵ�Ĵ�����ӵ�������
    scene_->addChild(expandedWindow_, 10);

    // �����رհ�ť
    closeButton_ = cocos2d::ui::Button::create("CloseNormal.png");
    closeButton_->setPosition(Vec2(expandedWindow_->getContentSize().width - 20, expandedWindow_->getContentSize().height - 20));
    closeButton_->addClickEventListener([=](Ref* sender)
        {
            this->closeExpandedMiniMap();
        });
    expandedWindow_->addChild(closeButton_);

    // ��Ӱ����
    shadowLayer_ = LayerColor::create(Color4B(0, 0, 0, 180)); // ��͸����ɫ����
    shadowLayer_->setContentSize(visibleSize_);
    shadowLayer_->setPosition(0, 0);
    scene_->addChild(shadowLayer_, 4); // �����Ӱ��
}



// �رշŴ��С��ͼ
void MiniMap::closeExpandedMiniMap() 
{
    if (!isExpanded_) return; // ��ֹ�ظ�ִ��
    isExpanded_ = false;

    // �ӳ����Ƴ��Ŵ󴰿�
    if (expandedWindow_) 
    {
        expandedWindow_->removeAllChildren();
        scene_->removeChild(expandedWindow_);
        expandedWindow_ = nullptr;
    }

    // �Ƴ���Ӱ��
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
        // ��ȡ���ֵĴ�ֱ����ֵ
        float scrollY = event->getScrollY();

        // ��ȡ��굱ǰλ�ã���Ļ���꣩
        Vec2 mousePos = event->getLocation();

        // �����λ�ô���Ļ����ת��Ϊ����ڷŴ��ͼ���ڵľֲ�����
        Vec2 localMousePos = expandedWindow_->convertToNodeSpace(mousePos);

        // �������������ż���
        float newZoom = zoomLevel_ - scrollY * 0.1f;
        zoomLevel_ = clampf(newZoom, 0.9f, 2.0f); // �������ŷ�Χ

        // �������ű���
        float scaleFactor = zoomLevel_ / zoomLevel_;

        // ��ȡ��ǰ����������
        Vec2 zoomCenter = localMousePos;

        // �����µ����ű���
        miniMapBackground_2_->setScale(zoomLevel_);

        // �������ź�ı���λ�ã�ʹ�������Ϊ����
        Vec2 offset = zoomCenter - localMousePos;
        miniMapBackground_2_->setPosition(miniMapBackground_2_->getPosition() + offset);
    }
}

void MiniMap::onMouseDown(cocos2d::EventMouse* event)
{
    // ����Ƿ��Ѿ�չ����С��ͼ
    if (isExpanded_)
    {
        // ������������
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        {
            isDragging_ = true;
            dragStartPos_ = event->getLocation(); // ��¼��ʼλ��
            dragStartMapPos_ = miniMapBackground_2_->getPosition(); // ��¼��ʼ��ͼλ��
        }
    }

    // ��ȡ�������λ�ã���Ļ���꣩
    Vec2 clickPos = event->getLocation();

    // �����λ��ת��Ϊ�����С��ͼ�ü��ڵ������
    Vec2 localClickPos = clipper_->convertToNodeSpace(clickPos);

    // ���� y �����ƫ�ȷ�����Ͻ�����ϵͳ
    localClickPos.y = visibleSize_.height - localClickPos.y;  // ��ת y ����

    // �жϵ��λ���Ƿ���Բ��������
    float distance = localClickPos.distance(miniMapCenter_);
    if (distance <= miniMapRadius_)
    {
        // ������λ����Բ�������ڣ���С��ͼ��δ�Ŵ���ִ�зŴ����
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
        // ��������϶���ƫ����
        Vec2 dragOffset = event->getLocation() - dragStartPos_;

        // ���� y ����ķ�ת���� Cocos2d �У�y��������������ϣ�������¼�������ϵͳ���෴��
        dragOffset.y = -dragOffset.y;

        // ���µ�ͼ��λ��
        miniMapBackground_2_->setPosition(dragStartMapPos_ + dragOffset);
    }
}


// ��������ͷ��¼��������϶�
void MiniMap::onMouseUp(EventMouse* event)
{
    if (isExpanded_ && event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        isDragging_ = false;
    }

}

