/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     TreasureHunt.cpp
 * File Function: 实现沙漠中的寻宝任务类。
 * Author:        叶贤伟
 * Update Date:   2024.12.19
 ****************************************************************/

#include "TreasureHunt.h"
#include "SimpleAudioEngine.h"
extern bool isTask2Completed;

 // 构造函数
TreasureHunt::TreasureHunt()
    : 
    visible_size_(cocos2d::Size::ZERO),  // 可视区域大小初始化为零
    map_(nullptr),              // 地图初始化为空
    hero_(nullptr),             // 角色初始化为空
    target_score_(0),           // 目标得分初始化为0
    current_score_(0),          // 当前得分初始化为0
    time_limit_(0.0f),          // 时间限制初始化为0.0
    elapsed_time_(0.0f),        // 已经过的时间初始化为0.0
    task_status_(TASK_IN_PROGRESS),   // 初始任务状态为进行中
    is_task_active_(false),     // 任务初始化为未激活
    timer_label_(nullptr),      // 计时器标签初始化为空
    target_score_label_(nullptr) {  // 目标得分标签初始化为空
}

// 析构函数
TreasureHunt::~TreasureHunt() 
{
    CleanupUI();  // 清理UI资源
}

// 初始化函数
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
        return false;  // 如果父类初始化失败，则返回false
    }

    // 初始化成员变量
    visible_size_ = visible_size;  // 设置可视区域大小
    map_ = map;  // 设置地图
    hero_ = hero;  // 设置角色
    start_pos_ = start_pos;  // 设置开始位置
    end_pos_ = end_pos;  // 设置结束位置
    target_score_ = target_score;  // 设置目标得分
    time_limit_ = time_limit;  // 设置时间限制

    return true;  // 初始化成功，返回true
}

// 创建函数，返回一个初始化好的 TreasureHunt 对象
TreasureHunt* TreasureHunt::create(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const cocos2d::Vec2& start_pos,
    const cocos2d::Vec2& end_pos,
    int target_score,
    float time_limit) 
{

    TreasureHunt* ret = new TreasureHunt();  // 创建一个新的 TreasureHunt 对象
    if (ret && ret->init( visible_size, map, hero, start_pos, end_pos, target_score, time_limit)) {
        ret->autorelease();  // 自动释放内存
        return ret;  // 返回初始化成功的对象
    }
    else 
    {
        delete ret;  // 如果初始化失败，删除对象
        return nullptr;  // 返回空指针
    }
}

// 启动任务
void TreasureHunt::StartTask()
{
    current_score_ = 0;  // 初始化当前得分为0
    elapsed_time_ = 0.0f;  // 初始化已过时间为0
    is_task_active_ = true;  // 设置任务为激活状态
    task_status_ = TASK_IN_PROGRESS;  // 设置任务状态为进行中

    // 创建计时器标签
    timer_label_ = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 24);
    timer_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 50));  // 设置位置
    timer_label_->setTextColor(cocos2d::Color4B::RED);  // 设置文本颜色为红色
    this->addChild(timer_label_);  // 将计时器标签添加到场景中

    // 创建目标得分标签
    target_score_label_ = cocos2d::Label::createWithTTF("Target: " + std::to_string(target_score_), "fonts/Marker Felt.ttf", 24);
    target_score_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 100));  // 设置位置
    target_score_label_->setTextColor(cocos2d::Color4B::RED);  // 设置文本颜色为红色
    this->addChild(target_score_label_);  // 将目标得分标签添加到场景中

    // 创建限定时间标签
    time_limit_label_ = cocos2d::Label::createWithTTF("Limited Time: " + std::to_string(int(time_limit_)), "fonts/Marker Felt.ttf", 24);
    time_limit_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 130));  // 设置位置
    time_limit_label_->setTextColor(cocos2d::Color4B::RED);  // 设置文本颜色为红色
    this->addChild(time_limit_label_);  // 将限定时间标签添加到场景中

    // 注册定时器回调函数
    cocos2d::Director::getInstance()->getScheduler()->schedule(
        [this](float delta) { Update(delta); },  // 每0.1秒调用一次Update函数
        this,
        0.1f,
        false,
        "treasure_hunt_timer");  // 定时器ID为"treasure_hunt_timer"

    // 设置键盘监听器
    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
        {
            // 判断按下的键是否为 E 键
            if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_E)
            {
                // 判断角色是否在宝石图层上
                if (IsHeroOnGemLayer())
                {
                    //采集矿石音效
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/get_gem.mp3");

                    CollectGem(hero_->getPosition());  // 执行采集矿石
                }
            }
        };

    // 将键盘监听器加入到事件派发器中
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

// 更新任务状态
void TreasureHunt::Update(float delta) 
{
    if (!is_task_active_) return;  // 如果任务未激活，直接返回

    elapsed_time_ += delta;  // 累加已过去的时间
    UpdateTimerLabel();  // 更新计时器UI

    // 判断是否时间到
    if (elapsed_time_ >= time_limit_) 
    {
        FailTask();  // 时间结束，任务失败
    }

    // 判断是否达到目标得分，并且到达终点
    if (current_score_ >= target_score_ && hero_->getPosition().distance(end_pos_) < 50) 
    {
        CompleteTask();  // 如果得分达到目标并到达终点，任务完成
    }
}

// 更新计时器UI
void TreasureHunt::UpdateTimerLabel() 
{
    timer_label_->setString(cocos2d::StringUtils::format("Time: %.1f\nScore: %d",
        time_limit_ - elapsed_time_,  // 显示剩余时间
        current_score_,               // 显示当前得分
        target_score_));              // 显示目标得分
}

// 判断角色是否在“gem”图层上
bool TreasureHunt::IsHeroOnGemLayer() 
{
    // 将角色的位置转换为瓦片坐标
    cocos2d::Vec2 hero_tile_coord = tileCoordForPosition(hero_->getPosition());

    // 获取宝石图层
    auto gem_layer = map_->getLayer("gem");
    if (!gem_layer) return false;  // 如果没有宝石图层，返回false

    // 获取角色当前位置的瓦片GID
    int tileGID = gem_layer->getTileGIDAt(cocos2d::Vec2(hero_tile_coord.x, hero_tile_coord.y));

    // 如果当前瓦片有宝石（GID不为0），返回true
    return tileGID != 0;
}

// 采集矿石
void TreasureHunt::CollectGem(const cocos2d::Vec2& position) 
{
    if (!is_task_active_) return;  // 如果任务未激活，直接返回

    current_score_ += 5;  // 每次采集宝石，得10分
    // 显示采集成功的消息
    auto label = cocos2d::Label::createWithTTF("Gem Collected!", "fonts/Marker Felt.ttf", 48);  // 创建消息标签
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2+100);  // 设置标签位置居中
    label->setTextColor(cocos2d::Color4B::WHITE);  // 设置文本颜色为白色
    this->addChild(label);  // 将标签添加到场景中

    // 标签显示3秒后移除
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(1.0f),  // 延迟3秒
        cocos2d::RemoveSelf::create(), nullptr));  // 移除标签
}

// 任务完成
void TreasureHunt::CompleteTask() 
{
    is_task_active_ = false;  // 设置任务为未激活状态
    task_status_ = TASK_COMPLETED;  // 设置任务状态为完成
    cocos2d::Director::getInstance()->getScheduler()->unschedule("treasure_hunt_timer", this);  // 取消定时器
    //任务完成音效
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/task_finish.mp3");

    ShowMessage("Mission Success!");  // 显示任务成功消息
    CleanupUI();  // 清理UI

    // 完成任务2的奖励
    isTask2Completed = true;
}

// 任务失败
void TreasureHunt::FailTask()
{
    is_task_active_ = false;  // 设置任务为未激活状态
    task_status_ = TASK_FAILED;  // 设置任务状态为失败
    cocos2d::Director::getInstance()->getScheduler()->unschedule("treasure_hunt_timer", this);  // 取消定时器
    //任务失败音效
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/task_fail.mp3");

    ShowMessage("Mission Failed!");  // 显示任务失败消息
    CleanupUI();  // 清理UI
}

// 显示任务状态消息
void TreasureHunt::ShowMessage(const std::string& message) 
{
    auto label = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 60);  // 创建消息标签
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2);  // 设置标签位置居中
    label->setTextColor(cocos2d::Color4B::BLACK);  // 设置文本颜色为黑色
    this->addChild(label);  // 将标签添加到场景中

    // 标签显示3秒后移除
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),  // 延迟3秒
        cocos2d::RemoveSelf::create(), nullptr));  // 移除标签
}

// 清理UI
void TreasureHunt::CleanupUI()
{
    // 移除计时器标签
    if (timer_label_) 
    {
        timer_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // 延迟3秒
            cocos2d::CallFunc::create([this]() 
                {
                timer_label_->removeFromParent();  // 从场景中移除计时器标签
                timer_label_ = nullptr;  // 清空计时器标签指针
                }),
            nullptr));
    }

    // 移除目标得分标签
    if (target_score_label_) 
    {
        target_score_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // 延迟3秒
            cocos2d::CallFunc::create([this]() 
                {
                target_score_label_->removeFromParent();  // 从场景中移除目标得分标签
                target_score_label_ = nullptr;  // 清空目标得分标签指针
                }),
            nullptr));
    }

    // 移除时间限制标签
    if (time_limit_label_)
    {
        time_limit_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // 延迟3秒
            cocos2d::CallFunc::create([this]()
                {
                    time_limit_label_->removeFromParent();  // 从场景中移除目标得分标签
                    time_limit_label_ = nullptr;  // 清空目标得分标签指针
                }),
            nullptr));
    }
}

// 获取当前任务状态
TreasureHunt::TaskStatus TreasureHunt::GetTaskStatus() const 
{
    return task_status_;  // 返回当前任务状态
}

Vec2 TreasureHunt::tileCoordForPosition(cocos2d::Vec2 position)
{
    float x = position.x;
    float y = position.y;

    // 假设每个瓦片的大小是 16x16
    int tileWidth = 16;  // 每个瓦片的宽度
    int tileHeight = 16; // 每个瓦片的高度

    // 计算瓦片坐标
    int tileX = (int)(x / tileWidth);
    int tileY = (int)((map_->getMapSize().height * tileHeight - y) / tileHeight);

    // 输出瓦片坐标
    CCLOG("Converting position (%.2f, %.2f) to tile coordinates: (%.2f, %.2f)", x, y, tileX, tileY);

    return cocos2d::Vec2(tileX, tileY);
}