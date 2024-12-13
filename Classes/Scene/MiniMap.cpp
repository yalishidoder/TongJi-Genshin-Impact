/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     MiniMap.cpp
 * File Function: ʵ��С��ͼ(���Ͻǵ��״�)����
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.13
 ****************************************************************/
#include "MiniMap.h"

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
    else {
        heroMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::GREEN);
        clipper_->addChild(heroMarker_);
    }
    demonMarker_ = DrawNode::create(); // ����λ�ñ��
    if (demonMarker_ == nullptr)
    {
        CCLOG("create demonMarker failed");
       
    }
    else {
        demonMarker_->drawDot(Vec2::ZERO, 3.0f, Color4F::RED);
        clipper_->addChild(demonMarker_);
    }
    
    // ����ÿ֡���� update()
    this->scheduleUpdate();  // ���лὫ update() ע��Ϊÿ֡����
};



// ����С��ͼ���λ��
void MiniMap::update(float dt ) {
    // ����Ҫ�ı����Ƿ��ʼ��
    if (!map_ || !heroMarker_ || !demonMarker_) {
        CCLOG("map_ or markers not properly initialized");
        return;
    }

    // ��ȡӢ�۽ڵ�
    auto hero = dynamic_cast<Character*>(scene_->getChildByName("hero"));
    if (!hero) {
        CCLOG("Hero node not found in the scene");
        return;
    }
    
    // ��ȡ���˽ڵ�
    auto demon = dynamic_cast<Enemy*>(scene_->getChildByName("demon"));
    if (!demon) {
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
