/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     TreasureHunt.cpp
 * File Function: ʵ��ɳĮ�е�Ѱ�������ࡣ
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.19
 ****************************************************************/

#include "TreasureHunt.h"
#include "SimpleAudioEngine.h"
extern bool isTask2Completed;

 // ���캯��
TreasureHunt::TreasureHunt()
    : 
    visible_size_(cocos2d::Size::ZERO),  // ���������С��ʼ��Ϊ��
    map_(nullptr),              // ��ͼ��ʼ��Ϊ��
    hero_(nullptr),             // ��ɫ��ʼ��Ϊ��
    target_score_(0),           // Ŀ��÷ֳ�ʼ��Ϊ0
    current_score_(0),          // ��ǰ�÷ֳ�ʼ��Ϊ0
    time_limit_(0.0f),          // ʱ�����Ƴ�ʼ��Ϊ0.0
    elapsed_time_(0.0f),        // �Ѿ�����ʱ���ʼ��Ϊ0.0
    task_status_(TASK_IN_PROGRESS),   // ��ʼ����״̬Ϊ������
    is_task_active_(false),     // �����ʼ��Ϊδ����
    timer_label_(nullptr),      // ��ʱ����ǩ��ʼ��Ϊ��
    target_score_label_(nullptr) {  // Ŀ��÷ֱ�ǩ��ʼ��Ϊ��
}

// ��������
TreasureHunt::~TreasureHunt() 
{
    CleanupUI();  // ����UI��Դ
}

// ��ʼ������
bool TreasureHunt::init(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const cocos2d::Vec2& start_pos,
    const cocos2d::Vec2& end_pos,
    int target_score,
    float time_limit) 
{

    if (!Node::init()) 
    {
        return false;  // ��������ʼ��ʧ�ܣ��򷵻�false
    }

    // ��ʼ����Ա����
    visible_size_ = visible_size;  // ���ÿ��������С
    map_ = map;  // ���õ�ͼ
    hero_ = hero;  // ���ý�ɫ
    start_pos_ = start_pos;  // ���ÿ�ʼλ��
    end_pos_ = end_pos;  // ���ý���λ��
    target_score_ = target_score;  // ����Ŀ��÷�
    time_limit_ = time_limit;  // ����ʱ������

    return true;  // ��ʼ���ɹ�������true
}

// ��������������һ����ʼ���õ� TreasureHunt ����
TreasureHunt* TreasureHunt::create(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const cocos2d::Vec2& start_pos,
    const cocos2d::Vec2& end_pos,
    int target_score,
    float time_limit) 
{

    TreasureHunt* ret = new TreasureHunt();  // ����һ���µ� TreasureHunt ����
    if (ret && ret->init( visible_size, map, hero, start_pos, end_pos, target_score, time_limit)) {
        ret->autorelease();  // �Զ��ͷ��ڴ�
        return ret;  // ���س�ʼ���ɹ��Ķ���
    }
    else 
    {
        delete ret;  // �����ʼ��ʧ�ܣ�ɾ������
        return nullptr;  // ���ؿ�ָ��
    }
}

// ��������
void TreasureHunt::StartTask()
{
    current_score_ = 0;  // ��ʼ����ǰ�÷�Ϊ0
    elapsed_time_ = 0.0f;  // ��ʼ���ѹ�ʱ��Ϊ0
    is_task_active_ = true;  // ��������Ϊ����״̬
    task_status_ = TASK_IN_PROGRESS;  // ��������״̬Ϊ������

    // ������ʱ����ǩ
    timer_label_ = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 24);
    timer_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 50));  // ����λ��
    timer_label_->setTextColor(cocos2d::Color4B::RED);  // �����ı���ɫΪ��ɫ
    this->addChild(timer_label_);  // ����ʱ����ǩ��ӵ�������

    // ����Ŀ��÷ֱ�ǩ
    target_score_label_ = cocos2d::Label::createWithTTF("Target: " + std::to_string(target_score_), "fonts/Marker Felt.ttf", 24);
    target_score_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 100));  // ����λ��
    target_score_label_->setTextColor(cocos2d::Color4B::RED);  // �����ı���ɫΪ��ɫ
    this->addChild(target_score_label_);  // ��Ŀ��÷ֱ�ǩ��ӵ�������

    // �����޶�ʱ���ǩ
    time_limit_label_ = cocos2d::Label::createWithTTF("Limited Time: " + std::to_string(int(time_limit_)), "fonts/Marker Felt.ttf", 24);
    time_limit_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 130));  // ����λ��
    time_limit_label_->setTextColor(cocos2d::Color4B::RED);  // �����ı���ɫΪ��ɫ
    this->addChild(time_limit_label_);  // ���޶�ʱ���ǩ��ӵ�������

    // ע�ᶨʱ���ص�����
    cocos2d::Director::getInstance()->getScheduler()->schedule(
        [this](float delta) { Update(delta); },  // ÿ0.1�����һ��Update����
        this,
        0.1f,
        false,
        "treasure_hunt_timer");  // ��ʱ��IDΪ"treasure_hunt_timer"

    // ���ü��̼�����
    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
        {
            // �жϰ��µļ��Ƿ�Ϊ E ��
            if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_E)
            {
                // �жϽ�ɫ�Ƿ��ڱ�ʯͼ����
                if (IsHeroOnGemLayer())
                {
                    //�ɼ���ʯ��Ч
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/get_gem.mp3");

                    CollectGem(hero_->getPosition());  // ִ�вɼ���ʯ
                }
            }
        };

    // �����̼��������뵽�¼��ɷ�����
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// ��������״̬
void TreasureHunt::Update(float delta) 
{
    if (!is_task_active_) return;  // �������δ���ֱ�ӷ���

    elapsed_time_ += delta;  // �ۼ��ѹ�ȥ��ʱ��
    UpdateTimerLabel();  // ���¼�ʱ��UI

    // �ж��Ƿ�ʱ�䵽
    if (elapsed_time_ >= time_limit_) 
    {
        FailTask();  // ʱ�����������ʧ��
    }

    // �ж��Ƿ�ﵽĿ��÷֣����ҵ����յ�
    if (current_score_ >= target_score_ && hero_->getPosition().distance(end_pos_) < 50) 
    {
        CompleteTask();  // ����÷ִﵽĿ�겢�����յ㣬�������
    }
}

// ���¼�ʱ��UI
void TreasureHunt::UpdateTimerLabel() 
{
    timer_label_->setString(cocos2d::StringUtils::format("Time: %.1f\nScore: %d",
        time_limit_ - elapsed_time_,  // ��ʾʣ��ʱ��
        current_score_,               // ��ʾ��ǰ�÷�
        target_score_));              // ��ʾĿ��÷�
}

// �жϽ�ɫ�Ƿ��ڡ�gem��ͼ����
bool TreasureHunt::IsHeroOnGemLayer() 
{
    // ����ɫ��λ��ת��Ϊ��Ƭ����
    cocos2d::Vec2 hero_tile_coord = tileCoordForPosition(hero_->getPosition());

    // ��ȡ��ʯͼ��
    auto gem_layer = map_->getLayer("gem");
    if (!gem_layer) return false;  // ���û�б�ʯͼ�㣬����false

    // ��ȡ��ɫ��ǰλ�õ���ƬGID
    int tileGID = gem_layer->getTileGIDAt(cocos2d::Vec2(hero_tile_coord.x, hero_tile_coord.y));

    // �����ǰ��Ƭ�б�ʯ��GID��Ϊ0��������true
    return tileGID != 0;
}

// �ɼ���ʯ
void TreasureHunt::CollectGem(const cocos2d::Vec2& position) 
{
    if (!is_task_active_) return;  // �������δ���ֱ�ӷ���

    current_score_ += 5;  // ÿ�βɼ���ʯ����10��
    // ��ʾ�ɼ��ɹ�����Ϣ
    auto label = cocos2d::Label::createWithTTF("Gem Collected!", "fonts/Marker Felt.ttf", 48);  // ������Ϣ��ǩ
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2+100);  // ���ñ�ǩλ�þ���
    label->setTextColor(cocos2d::Color4B::WHITE);  // �����ı���ɫΪ��ɫ
    this->addChild(label);  // ����ǩ��ӵ�������

    // ��ǩ��ʾ3����Ƴ�
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(1.0f),  // �ӳ�3��
        cocos2d::RemoveSelf::create(), nullptr));  // �Ƴ���ǩ
}

// �������
void TreasureHunt::CompleteTask() 
{
    is_task_active_ = false;  // ��������Ϊδ����״̬
    task_status_ = TASK_COMPLETED;  // ��������״̬Ϊ���
    cocos2d::Director::getInstance()->getScheduler()->unschedule("treasure_hunt_timer", this);  // ȡ����ʱ��
    //���������Ч
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/task_finish.mp3");

    ShowMessage("Mission Success!");  // ��ʾ����ɹ���Ϣ
    CleanupUI();  // ����UI

    // �������2�Ľ���
    isTask2Completed = true;
}

// ����ʧ��
void TreasureHunt::FailTask()
{
    is_task_active_ = false;  // ��������Ϊδ����״̬
    task_status_ = TASK_FAILED;  // ��������״̬Ϊʧ��
    cocos2d::Director::getInstance()->getScheduler()->unschedule("treasure_hunt_timer", this);  // ȡ����ʱ��
    //����ʧ����Ч
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/task_fail.mp3");

    ShowMessage("Mission Failed!");  // ��ʾ����ʧ����Ϣ
    CleanupUI();  // ����UI
}

// ��ʾ����״̬��Ϣ
void TreasureHunt::ShowMessage(const std::string& message) 
{
    auto label = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 60);  // ������Ϣ��ǩ
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2);  // ���ñ�ǩλ�þ���
    label->setTextColor(cocos2d::Color4B::BLACK);  // �����ı���ɫΪ��ɫ
    this->addChild(label);  // ����ǩ��ӵ�������

    // ��ǩ��ʾ3����Ƴ�
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
        cocos2d::RemoveSelf::create(), nullptr));  // �Ƴ���ǩ
}

// ����UI
void TreasureHunt::CleanupUI()
{
    // �Ƴ���ʱ����ǩ
    if (timer_label_) 
    {
        timer_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
            cocos2d::CallFunc::create([this]() 
                {
                timer_label_->removeFromParent();  // �ӳ������Ƴ���ʱ����ǩ
                timer_label_ = nullptr;  // ��ռ�ʱ����ǩָ��
                }),
            nullptr));
    }

    // �Ƴ�Ŀ��÷ֱ�ǩ
    if (target_score_label_) 
    {
        target_score_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
            cocos2d::CallFunc::create([this]() 
                {
                target_score_label_->removeFromParent();  // �ӳ������Ƴ�Ŀ��÷ֱ�ǩ
                target_score_label_ = nullptr;  // ���Ŀ��÷ֱ�ǩָ��
                }),
            nullptr));
    }

    // �Ƴ�ʱ�����Ʊ�ǩ
    if (time_limit_label_)
    {
        time_limit_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
            cocos2d::CallFunc::create([this]()
                {
                    time_limit_label_->removeFromParent();  // �ӳ������Ƴ�Ŀ��÷ֱ�ǩ
                    time_limit_label_ = nullptr;  // ���Ŀ��÷ֱ�ǩָ��
                }),
            nullptr));
    }
}

// ��ȡ��ǰ����״̬
TreasureHunt::TaskStatus TreasureHunt::GetTaskStatus() const 
{
    return task_status_;  // ���ص�ǰ����״̬
}

Vec2 TreasureHunt::tileCoordForPosition(cocos2d::Vec2 position)
{
    float x = position.x;
    float y = position.y;

    // ����ÿ����Ƭ�Ĵ�С�� 16x16
    int tileWidth = 16;  // ÿ����Ƭ�Ŀ��
    int tileHeight = 16; // ÿ����Ƭ�ĸ߶�

    // ������Ƭ����
    int tileX = (int)(x / tileWidth);
    int tileY = (int)((map_->getMapSize().height * tileHeight - y) / tileHeight);

    // �����Ƭ����
    CCLOG("Converting position (%.2f, %.2f) to tile coordinates: (%.2f, %.2f)", x, y, tileX, tileY);

    return cocos2d::Vec2(tileX, tileY);
}