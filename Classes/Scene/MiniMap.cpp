/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.cpp
 * File Function: ʵ��С��ͼ(���Ͻǵ��״�)���� ����ͨ��������������š��϶�����ת�Ȳ������Լ�ͬ������ͼ��ɫλ�õ�С��ͼ��
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.13
 ****************************************************************/
#include "MiniMap.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

extern bool isTask1Completed;
extern bool isTask2Completed;

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

    if (file == "Scene/MainScene/mainmap.tmx")
    {
        initFogOnMiniMap(); // ��ʼ��С��ͼ�����
    }

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

// �����������㣬�ֱ𸲸��Ұ벿�ֵ��������㣬����ʹ���Զ���� PNG ͼƬ��Ϊ��Ӱ
void MiniMap::initFogOnMiniMap() 
{
    // ��ȡС��ͼ�����ĳߴ�
    Size miniMapSize = miniMapBackground_->getContentSize();
    float miniMapOriginX = miniMapBackground_->getPositionX();
    float miniMapOriginY = miniMapBackground_->getPositionY();

    // �ϲ������������ϰ벿�֣�
    miniFogUpperLayer = Layer::create();
    miniFogUpperLayer->setContentSize(Size(miniMapSize.width / 2, miniMapSize.height / 2));
    miniFogUpperLayer->setPosition(Vec2(miniMapOriginX + miniMapSize.width / 2,
        miniMapOriginY + miniMapSize.height / 2));
    miniFogUpperLayer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT); // ê��Ϊ���½�
    miniMapBackground_->addChild(miniFogUpperLayer, 10);

    // �����ϲ������ͼƬ
    auto upperFogSprite = Sprite::create("Scene/MainScene/fogUpper.png");
    upperFogSprite->setContentSize(miniFogUpperLayer->getContentSize());
    upperFogSprite->setAnchorPoint(Vec2(-0.1f, 1.0f)); // ����ê��Ϊ���ģ�����x�����ƶ�������y�����ƶ�
    upperFogSprite->setOpacity(255); // ��͸��
    upperFogSprite->setScale(1.5f); // ������ͼƬ��󣬵�������������Ҫ
    miniFogUpperLayer->addChild(upperFogSprite,2);

    // �²������������°벿�֣�
    miniFogLowerLayer = Layer::create();
    miniFogLowerLayer->setContentSize(Size(miniMapSize.width / 2, miniMapSize.height / 2));
    miniFogLowerLayer->setPosition(Vec2(miniMapOriginX + miniMapSize.width / 2,
        miniMapOriginY));
    miniFogLowerLayer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    miniMapBackground_->addChild(miniFogLowerLayer, 10);

    // �����²������ͼƬ
    auto lowerFogSprite = Sprite::create("Scene/MainScene/fogLower.png");
    lowerFogSprite->setContentSize(miniFogLowerLayer->getContentSize());
    lowerFogSprite->setAnchorPoint(Vec2(0, 0.8f)); // ����ê��Ϊ���ģ�����x�����ƶ�������y�����ƶ�
    lowerFogSprite->setOpacity(255); // ��͸��
    lowerFogSprite->setScale(1.8f); // ������ͼƬ��󣬵�������������Ҫ
    miniFogLowerLayer->addChild(lowerFogSprite,2);

    // ���ó�ʼ�����ɼ���
    miniFogUpperLayer->setVisible(true);
    miniFogLowerLayer->setVisible(true);
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
    

    // ����ÿ֡���� update()
    this->scheduleUpdate();  // ���лὫ update() ע��Ϊÿ֡����
};



// ����С��ͼ���λ��
void MiniMap::update(float dt)
{
    if (isExpanded_) {
        auto hero = dynamic_cast<Hero*>(scene_->getChildByName("hero"));
        hero->m_moveUp = false;
        hero->m_moveDown = false;
        hero->m_moveLeft = false;
        hero->m_moveRight = false;
    }
    Node::update(dt);

    // ������һ���
    if (isTask1Completed == true && file_ == "Scene/MainScene/mainmap.tmx")
    {
   
            // ����С��ͼ�ϲ������ɼ�
            miniFogUpperLayer->setVisible(false);
        // ���÷Ŵ󴰿��ϲ������ɼ�
        if (isExpanded_)
            expandedWindowFogUpperLayer->setVisible(false);
    }
    // ����������
    if (isTask2Completed == true && file_ == "Scene/MainScene/mainmap.tmx")
    {
        // ����С��ͼ�²������ɼ�
        miniFogLowerLayer->setVisible(false);
        // ���÷Ŵ󴰿��²������ɼ�
        if (isExpanded_)
            expandedWindowFogLowerLayer->setVisible(false);
    }

    // ����Ҫ�ı����Ƿ��ʼ��
    if (!map_ || !heroMarker_)
    {
        CCLOG("map_ or markers not properly initialized");
        return;
    }

    // ��ȡӢ�۽ڵ�
    hero_ = dynamic_cast<Hero*>(scene_->getChildByName("hero"));
    if (!hero_)
    {
        CCLOG("Hero node not found in the scene");
        return;
    }

    // ����Ӣ�۵�λ��
    Vec2 heroPos = hero_->getPosition();
    miniHeroPos = (heroPos - map_->getPosition() + Vec2(map_->getContentSize().width / 2, map_->getContentSize().height / 2)) * miniMapScale_;
    heroMarker_->setPosition(miniMapCenter_ - miniHeroPos + heroPos * miniMapScale_);

    // ����С��ͼ��ͼλ��
    Vec2 offset = miniMapCenter_ - miniHeroPos;
    miniMapBackground_->setPosition(offset);

    // �������е��˱�ǵ�λ��
    updateDemonMarkers();
}

// �������е��˱�ǵ�λ��
void MiniMap::updateDemonMarkers()
{
    // ������е��˱��
    for (auto marker : demonMarkers_)
    {
        marker->removeFromParentAndCleanup(true);
    }
    
    // ��յ����б���ӵ�������
    demons_.clear();  // ��յ����б�
    demonMarkers_.clear();  // ��յ��˱���б�
    // ��ȡ���е��˲�������ӵ� demons_ ������
    for (auto demonNode : scene_->getChildren())
    {
        // �ж��Ƿ�Ϊ���˽ڵ�
        Enemy* demon = dynamic_cast<Enemy*>(demonNode);
        if (demon)
        {
            // ��������ӵ� demons_ ����
            demons_.push_back(demon);
            // Ϊÿ�����˴���һ�����
            demonMarker_ = DrawNode::create();
            if (demonMarker_)
            {
                demonMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::RED);
                clipper_->addChild(demonMarker_);
                // ��ӵ��˱�ǵ��б�
                demonMarkers_.push_back(demonMarker_);
            }
        }
    }

    // ���µ��˵�λ��
    for (size_t i = 0; i < demons_.size(); ++i)
    {
        Enemy* demon = demons_[i];
        if (demon)
        {
            Vec2 demonPos = demon->getPosition();
            Vec2 miniDemonPos = (demonPos - map_->getPosition() + Vec2(map_->getContentSize().width / 2, map_->getContentSize().height / 2)) * miniMapScale_;
            demonMarkers_[i]->setPosition(miniMapCenter_ - miniHeroPos + demonPos * miniMapScale_);
        }
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
    if (file_ == "Scene/MainScene/mainmap.tmx")
    {
        initFogOnExpandedWindow(); // ��ʼ���Ŵ󴰿ڵ������
    }

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

// �����������㣬�ֱ𸲸��Ұ벿�ֵ��������㣬����ʹ���Զ���� PNG ͼƬ��Ϊ��Ӱ
void MiniMap::initFogOnExpandedWindow()
{
    // ��ȡС��ͼ�����ĳߴ�
    Size expandedWindowMapSize = miniMapBackground_2_->getContentSize();
    float miniMapOriginX = miniMapBackground_2_->getPositionX();
    float miniMapOriginY = miniMapBackground_2_->getPositionY();

    // �ϲ������������ϰ벿�֣�
    expandedWindowFogUpperLayer = Layer::create();
    expandedWindowFogUpperLayer->setContentSize(Size(visibleSize_.width * 0.9f/2, visibleSize_.height * 0.9f/2));
    expandedWindowFogUpperLayer->setPosition(Vec2(miniMapOriginX + expandedWindowMapSize.width / 2,
        miniMapOriginY + expandedWindowMapSize.height / 2));
    expandedWindowFogUpperLayer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT); // ê��Ϊ���½�
    miniMapBackground_2_->addChild(expandedWindowFogUpperLayer, 10);

    // �����ϲ������ͼƬ
    auto upperFogSprite = Sprite::create("Scene/MainScene/fogUpper.png");
    upperFogSprite->setContentSize(expandedWindowFogUpperLayer->getContentSize());
    upperFogSprite->setAnchorPoint(Vec2(0.1f, 0.1f)); // ����ê��Ϊ���ģ�����x�����ƶ�������y�����ƶ�
    upperFogSprite->setOpacity(255); // ��͸��
    upperFogSprite->setScale(1.5f); // ������ͼƬ��󣬵�������������Ҫ
    expandedWindowFogUpperLayer->addChild(upperFogSprite, 1);

    // �²������������°벿�֣�
    expandedWindowFogLowerLayer = Layer::create();
    expandedWindowFogLowerLayer->setContentSize(Size(visibleSize_.width * 0.9f/2, visibleSize_.height * 0.9f/2));
    expandedWindowFogLowerLayer->setPosition(Vec2(miniMapOriginX + expandedWindowMapSize.width / 2,
        miniMapOriginY));
    expandedWindowFogLowerLayer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    miniMapBackground_2_->addChild(expandedWindowFogLowerLayer, 10);

    // �����²������ͼƬ
    auto lowerFogSprite = Sprite::create("Scene/MainScene/fogLower.png");
    lowerFogSprite->setContentSize(expandedWindowFogLowerLayer->getContentSize());
    lowerFogSprite->setAnchorPoint(Vec2(0.2f, 0.15f)); // ����ê��Ϊ���ģ�����x�����ƶ�������y�����ƶ�
    lowerFogSprite->setOpacity(255); // ��͸��
    lowerFogSprite->setScale(1.8f); // ������ͼƬ��󣬵�������������Ҫ
    expandedWindowFogLowerLayer->addChild(lowerFogSprite, 1);

    // ���ó�ʼ�����ɼ���
    expandedWindowFogUpperLayer->setVisible(true);
    expandedWindowFogLowerLayer->setVisible(true);
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