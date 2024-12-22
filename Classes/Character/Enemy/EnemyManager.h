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

    // ���õ�ǰ����ID
    void setSceneID(const std::string& sceneID);
    // ����ָ�������ĵ���ˢ����Ϣ
    void clearScene(const std::string& sceneID);

    // ע����������¼�
    void registerEnemyDeath(Enemy* enemy);

    // ���¹������߼�
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

    // ��ǰ����ID
    std::string currentSceneID;

    // ����ID������ˢ����Ϣ�б��ӳ��
    std::map<std::string, std::vector<EnemySpawnInfo>> spawnInfoMap;

    void spawnEnemy(const EnemySpawnInfo& info);
};

#endif // __ENEMY_MANAGER_H__