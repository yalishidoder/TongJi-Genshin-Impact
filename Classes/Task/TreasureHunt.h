/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     TreasureHunt.h
 * File Function: 实现沙漠中的寻宝任务类的定义。
 * Author:        叶贤伟
 * Update Date:   2024.12.19
 ****************************************************************/

#ifndef TREASURE_HUNT_H_
#define TREASURE_HUNT_H_

#include "cocos2d.h"
#include "Character/Hero/Hero.h"

class TreasureHunt : public cocos2d::Node 
{
public:
    // 任务状态枚举
    enum TaskStatus
    {
        TASK_IN_PROGRESS = 0,  // 任务进行中
        TASK_COMPLETED = 1,    // 任务完成
        TASK_FAILED = 2        // 任务失败
    };

    // 构造函数
    TreasureHunt();
    // 析构函数
    ~TreasureHunt();

    // 初始化函数，设置任务的相关信息
    bool init(
        const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const cocos2d::Vec2& start_pos,
        const cocos2d::Vec2& end_pos,
        int target_score,
        float time_limit);

    // 创建函数，返回一个初始化好的 TreasureHunt 对象
    static TreasureHunt* create(
        const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const cocos2d::Vec2& start_pos,
        const cocos2d::Vec2& end_pos,
        int target_score,
        float time_limit);

    // 设置目标得分
    void SetTargetScore(int target_score);

    // 设置时间限制
    void SetTimeLimit(float time_limit);

    // 启动任务
    void StartTask();

    // 获取当前任务状态
    TaskStatus GetTaskStatus() const;

    // 转化坐标到瓦片坐标
    Vec2 tileCoordForPosition(cocos2d::Vec2 position);

private:
    // 更新任务状态
    void Update(float delta);

    // 更新计时器UI
    void UpdateTimerLabel();

    // 采集矿石
    void CollectGem(const cocos2d::Vec2& position);

    // 完成任务
    void CompleteTask();

    // 任务失败
    void FailTask();

    // 显示任务状态消息
    void ShowMessage(const std::string& message);

    // 清理UI
    void CleanupUI();

    // 判断角色是否在“gem”图块层上
    bool IsHeroOnGemLayer() ;

    // 数据成员
    //  
    // 屏幕可视区域大小
    cocos2d::Size visible_size_;     
    // 地图对象
    cocos2d::TMXTiledMap* map_;    
    // 角色对象
    Hero* hero_;   
    // 任务开始位置
    cocos2d::Vec2 start_pos_;    
    // 任务结束位置
    cocos2d::Vec2 end_pos_; 
    // 任务目标得分
    int target_score_;      
    // 当前得分
    int current_score_;    
    // 任务时间限制
    float time_limit_;     
    // 已经过去的时间
    float elapsed_time_;  
    // 当前任务状态
    TaskStatus task_status_;  
    // 任务是否激活
    bool is_task_active_;     
    // 计时器标签
    cocos2d::Label* timer_label_;   
    // 目标得分标签
    cocos2d::Label* target_score_label_; 

    // 目标得分标签
    cocos2d::Label* time_limit_label_;
};

#endif  // TREASURE_HUNT_H_
