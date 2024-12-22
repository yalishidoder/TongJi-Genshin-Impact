#ifndef __ENEMY_MANAGER_H__
#define __ENEMY_MANAGER_H__

#include "cocos2d.h"
#include "Enemy.h"
#include <vector>
#include <map>
#include <string>

class EnemyManager {
public:
    static EnemyManager* getInstance();
    static void destroyInstance();

    // 设置当前场景ID
    void setSceneID(const std::string& sceneID);
    // 清理指定场景的敌人刷新信息
    void clearScene(const std::string& sceneID);

    // 注册敌人死亡事件
    void registerEnemyDeath(Enemy* enemy);

    // 更新管理器逻辑
    void update(float delta);

private:
    EnemyManager();
    ~EnemyManager();

    static EnemyManager* s_instance;

    struct EnemySpawnInfo {
        cocos2d::Vec2 position;
        int level;
        float respawnTime;
        float elapsedTime;
        CharacterElement element;
        bool attackMethod;
        Hero* player;
    };

    // 当前场景ID
    std::string currentSceneID;

    // 场景ID到敌人刷新信息列表的映射
    std::map<std::string, std::vector<EnemySpawnInfo>> spawnInfoMap;

    void spawnEnemy(const EnemySpawnInfo& info);
};

#endif // __ENEMY_MANAGER_H__