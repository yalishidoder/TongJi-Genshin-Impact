/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Maze.h
 * File Function: 定义迷宫任务类，实现任务启动及完成检测。
 * Author:        叶贤伟
 * Last Updated:  2024-12-19
 ****************************************************************/

#ifndef MAZE_H_
#define MAZE_H_

#include <string>
#include "cocos2d.h"
#include "Character/Hero/Hero.h"


// Maze类，表示一个迷宫任务，继承自cocos2d::Node
class Maze : public cocos2d::Node 
{

public:
    // 任务状态枚举
    enum TaskStatus
    {
        TASK_IN_PROGRESS = 0,  // 任务进行中
        TASK_COMPLETED = 1,    // 任务完成
        TASK_FAILED = 2        // 任务失败
    };

    // 创建Maze对象，使用工厂方法模式
    static Maze* Create(const std::string& maze_name, const cocos2d::Vec2& start,
        const cocos2d::Vec2& end, float time_limit,
        const std::string& reward, Hero* hero);

    // 初始化方法，配置迷宫任务的基本参数
    bool Init(const std::string& maze_name, const cocos2d::Vec2& start,
        const cocos2d::Vec2& end, float time_limit,
        const std::string& reward, Hero* hero);

    // 启动迷宫任务
    void StartTask();

    // 获取任务状态（是否成功/失败）
    bool GetTaskStatus() const;

private:
    // 私有构造函数和析构函数
    Maze();

    virtual ~Maze();

    // 更新任务逻辑（计时和检测玩家位置）
    void UpdateTask(float delta);

    // 检测玩家是否到达终点
    void CheckCompletion();

    // 任务完成时的处理
    void CompleteTask();

    // 任务失败时的处理
    void FailTask();

    // 显示任务状态信息（成功/失败）
    void ShowMessage(const std::string& message);

    // 清理UI和计时器
    void Cleanup();

    // 迷宫任务的名称
    std::string maze_name_;

    // 起点和终点位置
    cocos2d::Vec2 start_;
    cocos2d::Vec2 end_;

    // 时间限制（秒）
    float time_limit_;

    // 当前已用时间
    float elapsed_time_;

    // 奖励描述
    std::string reward_;

    // 任务状态
    bool task_status_=false;

    // 是否任务正在运行
    bool is_task_active_;

    // 显示计时的标签
    cocos2d::Label* timer_label_;

    // 目标得分标签
    cocos2d::Label* time_limit_label_;

    // 玩家角色节点
    cocos2d::Node* player_;
    Hero* hero_;             // 角色

    // 可见区域大小，用于显示UI
    cocos2d::Size visible_size_;
};

#endif  // MAZE_H_