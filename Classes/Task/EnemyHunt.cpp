#include "EnemyHunt.h"
#include "Character/Hero/Hero.h"
#include "Character/Enemy/Enemy.h"
#include "SimpleAudioEngine.h"


extern bool isTask3Completed ;
// 构造函数
EnemyHunt::EnemyHunt()
    : visible_size_(cocos2d::Size::ZERO),  // 可视区域大小初始化为零
    map_(nullptr),                      // 地图初始化为空
    hero_(nullptr),                     // 角色初始化为空
    target_kills_(0),                   // 目标击杀数初始化为0
    current_kills_(0),                  // 当前击杀数初始化为0
    task_status_(TASK_IN_PROGRESS),     // 初始任务状态为进行中
    is_task_active_(false),             // 任务初始化为未激活
    target_kills_label_(nullptr) {      // 目标击杀数标签初始化为空
}

// 析构函数
EnemyHunt::~EnemyHunt()
{
    CleanupUI();  // 清理UI资源
}

// 初始化函数
bool EnemyHunt::init(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const std::vector<Enemy*>& enemies,
    int target_kills)
{
    if (!Node::init())
    {
        return false;  // 如果父类初始化失败，则返回false
    }

    // 初始化成员变量
    visible_size_ = visible_size;  // 设置可视区域大小
    map_ = map;                    // 设置地图
    hero_ = hero;                  // 设置角色
    enemies_ = enemies;            // 设置敌人列表
    target_kills_ = target_kills;  // 设置目标击杀数

    return true;  // 初始化成功，返回true
}

// 创建函数，返回一个初始化好的 EnemyHunt 对象
EnemyHunt* EnemyHunt::create(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const std::vector<Enemy*>& enemies,
    int target_kills)
{
    EnemyHunt* ret = new EnemyHunt();  // 创建一个新的 EnemyHunt 对象
    if (ret && ret->init(visible_size, map, hero, enemies, target_kills))
    {
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
void EnemyHunt::StartTask()
{
    current_kills_ = 0;         // 初始化当前击杀数为0
    is_task_active_ = true;     // 设置任务为激活状态
    task_status_ = TASK_IN_PROGRESS;  // 设置任务状态为进行中

    // 创建目标击杀数标签
    target_kills_label_ = cocos2d::Label::createWithTTF("Target Kills: " + std::to_string(target_kills_), "fonts/Marker Felt.ttf", 24);
    target_kills_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 50));  // 设置位置
    target_kills_label_->setTextColor(cocos2d::Color4B::RED);  // 设置文本颜色为红色
    this->addChild(target_kills_label_);  // 将目标击杀数标签添加到场景中

    // 注册定时器回调函数
    cocos2d::Director::getInstance()->getScheduler()->schedule(
        [this](float delta) { Update(); },  // 每0.1秒调用一次Update函数
        this,
        0.1f,
        false,
        "enemy");  // 定时器ID为"enemy"
}

// 更新任务状态
void EnemyHunt::Update()
{
    if (!is_task_active_) return;  // 如果任务未激活，直接返回
    // 模拟击败敌人
    for (auto enemy : enemies_)
    {
        if (!enemy->isAlive() && enemy->task_alive == 1)
        {
            enemy->task_alive = 0;
            DefeatEnemy(enemy);  // 每次调用 DefeatEnemy 函数时，都会增加一个击杀数并显示消息
        }
    }
    // 判断是否达到目标击杀数
    if (current_kills_ >= target_kills_)
    {
        CompleteTask();  // 如果击杀数达到目标，任务完成
    }
}

// 击败敌人
void EnemyHunt::DefeatEnemy(Enemy* enemy)
{
    if (!is_task_active_ || !enemy) return;  // 如果任务未激活，或敌人为空，直接返回

    current_kills_++;  // 增加击杀数

    // 显示击杀成功的消息
    auto label = cocos2d::Label::createWithTTF("Enemy Defeated!", "fonts/Marker Felt.ttf", 48);  // 创建消息标签
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2 + 100);  // 设置标签位置居中
    label->setTextColor(cocos2d::Color4B::WHITE);  // 设置文本颜色为白色
    this->addChild(label);  // 将标签添加到场景中

    // 标签显示3秒后移除
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(1.0f),  // 延迟3秒
        cocos2d::RemoveSelf::create(), nullptr));  // 移除标签
}

// 任务完成
void EnemyHunt::CompleteTask()
{
    is_task_active_ = false;  // 设置任务为未激活状态
    task_status_ = TASK_COMPLETED;  // 设置任务状态为完成
    //任务完成音效
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/task_finish.mp3");

    ShowMessage("Mission Success!");  // 显示任务成功消息
    CleanupUI();  // 清理UI

    // 任务3完成，之后可以填写奖励
    isTask3Completed = true;
    int target_level = hero_->getLevel() + 10;
    if (target_level >= 99) {
        hero_->setLevel(99);
    }
    else {
        hero_->setLevel(target_level);
    }
}

// 任务失败
void EnemyHunt::FailTask()
{
    is_task_active_ = false;  // 设置任务为未激活状态
    task_status_ = TASK_FAILED;  // 设置任务状态为失败
    ShowMessage("Mission Failed!");  // 显示任务失败消息
    CleanupUI();  // 清理UI
}

// 显示任务状态消息
void EnemyHunt::ShowMessage(const std::string& message)
{
    auto label = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 60);  // 创建消息标签
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2);  // 设置标签位置居中
    label->setTextColor(cocos2d::Color4B::BLACK);  // 设置文本颜色为黑色
    this->addChild(label);  // 将标签添加到场景中

    // 标签显示3秒后移除
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),
        cocos2d::RemoveSelf::create(), nullptr));  // 移除标签
}

EnemyHunt::TaskStatus EnemyHunt::GetTaskStatus() const
{
    return task_status_;  // 返回当前任务状态
}

// 清理UI
void EnemyHunt::CleanupUI()
{
    // 移除目标击杀数标签
    if (target_kills_label_)
    {
        target_kills_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),
            cocos2d::CallFunc::create([this]()
                {
                    target_kills_label_->removeFromParent();
                    target_kills_label_ = nullptr;
                }),
            nullptr));
    }
}
