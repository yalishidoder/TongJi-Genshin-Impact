#ifndef __ENEMYHUNT_H__
#define __ENEMYHUNT_H__

#include "cocos2d.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Hero/Hero.h"

class EnemyHunt : public cocos2d::Node
{
public:
    // 任务状态
    enum TaskStatus
    {
        TASK_IN_PROGRESS,  // 任务进行中
        TASK_COMPLETED,    // 任务已完成
        TASK_FAILED        // 任务失败
    };

    // 构造函数
    EnemyHunt();
    // 析构函数
    virtual ~EnemyHunt();

    // 初始化函数
    bool init(const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const std::vector<Enemy*>& enemies,
        int target_kills);

    // 创建静态函数
    static EnemyHunt* create(const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const std::vector<Enemy*>& enemies,
        int target_kills);

    // 启动任务
    void StartTask();

    // 更新任务状态
    void Update();

    // 击败敌人
    void DefeatEnemy(Enemy* enemy);

    // 任务完成
    void CompleteTask();

    // 任务失败
    void FailTask();

    // 显示任务状态消息
    void ShowMessage(const std::string& message);

    // 获取当前任务状态
    TaskStatus GetTaskStatus() const;

    // 清理UI
    void CleanupUI();
    //int get_enemies_num(std::vector<Enemy*> enemies);

private:
    cocos2d::Size visible_size_;          // 可视区域大小
    cocos2d::TMXTiledMap* map_;           // 地图
    Hero* hero_;                          // 角色
    std::vector<Enemy*> enemies_;         // 敌人列表
    int target_kills_;                    // 目标击杀数
    int current_kills_;                   // 当前击杀数
    TaskStatus task_status_;              // 任务状态
    bool is_task_active_;                 // 任务是否激活
    cocos2d::Label* target_kills_label_;  // 目标击杀数标签
};

#endif // __ENEMYHUNT_H__
