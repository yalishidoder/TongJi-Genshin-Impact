/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Maze.cpp
 * File Function: 定义迷宫任务类的实现。要求实现迷宫任务的创建、启动、更新、完成、失败、UI显示等功能。
 * Description:   迷宫任务的内容是在限定时间内，通过移动角色到达终点。
 * Author:        叶贤伟
 * Last Updated:  2024-12-19
 ****************************************************************/


#include "Maze.h"

extern bool isTask1Completed;


 // 构造函数
Maze::Maze()
    : time_limit_(30.0f),         // 设置默认时间限制为30秒
    elapsed_time_(0.0f),         // 初始化已用时间为0
    is_task_active_(false),      // 任务初始状态为未激活
    timer_label_(nullptr),       // 计时器标签指针初始化为空
    player_(nullptr),            // 玩家节点指针初始化为空
    task_status_(false) {}

// 析构函数
 Maze::~Maze()
{
    Cleanup();  // 清理计时器和UI元素
}

// 工厂方法：创建并初始化Maze对象
Maze* Maze::Create(const std::string& maze_name, const cocos2d::Vec2& start,
    const cocos2d::Vec2& end, float time_limit,
    const std::string& reward) 
{
    Maze* maze = new (std::nothrow) Maze();
    if (maze && maze->Init(maze_name, start, end, time_limit, reward)) 
    {
        maze->autorelease();  // 自动释放
        return maze;
    }
    delete maze;  // 创建失败时删除对象
    return nullptr;
}

// 初始化方法，设置迷宫任务的基本参数
bool Maze::Init(const std::string& maze_name, const cocos2d::Vec2& start,
    const cocos2d::Vec2& end, float time_limit,
    const std::string& reward) 
{
    if (!Node::init()) 
    {  // 检查Node初始化是否成功
        return false;
    }

    // 配置迷宫的基本信息
    maze_name_ = maze_name;
    start_ = start;
    end_ = end;
    time_limit_ = time_limit;
    reward_ = reward;

    // 获取屏幕尺寸
    visible_size_ = cocos2d::Director::getInstance()->getVisibleSize();

    return true;
}

// 启动迷宫任务
void Maze::StartTask() 
{
    // 如果任务已经激活，则不再启动
    if (is_task_active_) return;

    is_task_active_ = true;
    elapsed_time_ = 0.0f;  // 重置已用时间
    task_status_ = false;

    // 获取玩家节点
    player_ = this->getParent()->getChildByName("hero");
    if (!player_)
    {
        cocos2d::log("Error: Cannot find player node.");
        return;
    }

    // 将玩家角色移动到迷宫的起点
    player_->setPosition(start_);

    // 创建计时器标签
    timer_label_ = cocos2d::Label::createWithTTF("Time: 0.0", "fonts/Marker Felt.ttf", 24);
    timer_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 50));
    timer_label_->setTextColor(cocos2d::Color4B::RED);
    this->getParent()->addChild(timer_label_, 20);

    // 创建限定时间标签
    time_limit_label_ = cocos2d::Label::createWithTTF("Limited Time: " + std::to_string(int(time_limit_)), "fonts/Marker Felt.ttf", 24);
    time_limit_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 100));  // 设置位置
    time_limit_label_->setTextColor(cocos2d::Color4B::RED);  // 设置文本颜色为红色
    this->addChild(time_limit_label_);  // 将限定时间标签添加到场景中

    // 启动任务逻辑更新
    this->schedule(CC_SCHEDULE_SELECTOR(Maze::UpdateTask), 0.1f);
}

// 任务更新逻辑：计时和检测任务完成情况
void Maze::UpdateTask(float delta) 
{
    // 如果任务未激活，则不执行
    if (!is_task_active_) return;

    elapsed_time_ += delta;  // 累加已用时间
    timer_label_->setString(cocos2d::StringUtils::format("Time: %.1f", elapsed_time_));

    // 如果已用时间超出限制，则任务失败
    if (elapsed_time_ > time_limit_) 
    {
        FailTask();
    }
    else 
    {
        CheckCompletion();  // 检查玩家是否到达终点
    }
}

// 检查玩家是否到达迷宫的终点
void Maze::CheckCompletion() 
{
    // 如果玩家节点不存在，则不进行检查
    if (!player_) return;

    // 判断玩家当前位置是否与终点相近
    if (player_->getPosition().distance(end_) < 15.0f) 
    {  // 允许一定误差
        CompleteTask();  // 玩家到达终点，任务完成
    }
}

// 任务完成处理
void Maze::CompleteTask() 
{
    is_task_active_ = false;  // 标记任务为未激活
    task_status_ = true;  // 设置任务状态为完成
    this->unschedule(CC_SCHEDULE_SELECTOR(Maze::UpdateTask));  // 停止任务计时

    ShowMessage("Mission Success!");  // 显示任务成功信息
    // 创建一个显示任务状态的标签
    auto label = cocos2d::Label::createWithTTF("Congratulations on your award!", "fonts/Marker Felt.ttf", 60);
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2 - 100);  // 设置标签位置
    label->setTextColor(cocos2d::Color4B::WHITE);  // 设置标签文字颜色为白色
    this->getParent()->addChild(label);

    // 延迟3秒后移除任务状态信息标签
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),  // 延迟3秒
        cocos2d::CallFunc::create([label]() { label->removeFromParent(); }),  // 移除标签
        nullptr));
    cocos2d::log("Maze task completed! Reward: %s", reward_.c_str());  // 输出奖励信息

    Cleanup();  // 清理UI元素

    // 完成任务后，触发任务1的完成事件（奖励）
    isTask1Completed = true;
}

// 任务失败处理
void Maze::FailTask()
{
    is_task_active_ = false;  // 标记任务为未激活
    task_status_ = false;  // 设置任务状态为失败

    this->unschedule(CC_SCHEDULE_SELECTOR(Maze::UpdateTask));  // 停止任务计时

    ShowMessage("Mission Failed!");  // 显示任务失败信息
    Cleanup();  // 清理UI元素
}

// 显示任务状态信息（成功/失败）
void Maze::ShowMessage(const std::string& message) 
{
    // 创建一个显示任务状态的标签
    auto label = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 60);
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2);  // 设置标签位置
    label->setTextColor(cocos2d::Color4B::WHITE);  // 设置标签文字颜色为白色
    this->getParent()->addChild(label);

    // 延迟3秒后移除任务状态信息标签
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),  // 延迟3秒
        cocos2d::CallFunc::create([label]() { label->removeFromParent(); }),  // 移除标签
        nullptr));

}

// 清理计时器和UI元素
void Maze::Cleanup()
{
    // 移除计时器标签
    if (timer_label_) 
    {
        timer_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // 延迟3秒
            cocos2d::CallFunc::create([this]() 
                {
                timer_label_->removeFromParent();  // 从场景中移除计时器标签
                timer_label_ = nullptr;            // 清空计时器标签指针
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


bool Maze::GetTaskStatus() const
{
    return task_status_;
};
