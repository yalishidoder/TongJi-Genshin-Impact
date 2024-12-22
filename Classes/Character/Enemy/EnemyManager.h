#ifndef __ENEMY_MANAGER_H__
#define __ENEMY_MANAGER_H__

#include "cocos2d.h"
#include "Enemy.h"
#include <vector>

class EnemyManager {
public:
    // 获取单例实例
    static EnemyManager* getInstance();

    // 销毁单例实例
    static void destroyInstance();

    // 注册敌人死亡事件
    void registerEnemyDeath(Enemy* enemy);

    // 更新管理器逻辑
    void update(float delta);

private:
    // 私有构造函数，防止外部实例化
    EnemyManager();
    ~EnemyManager();

    // 单例实例
    static EnemyManager* s_instance;

    // 敌人刷新信息结构体
    struct EnemySpawnInfo {
        cocos2d::Vec2 position;
        int level;
        float respawnTime;
        float elapsedTime;
        CharacterElement element;
        bool attackMethod;
        Hero* player;
    };

    // 存储敌人刷新信息的列表
    std::vector<EnemySpawnInfo> spawnInfoList;

    // 生成敌人
    void spawnEnemy(const EnemySpawnInfo& info);
};

#endif // __ENEMY_MANAGER_H__