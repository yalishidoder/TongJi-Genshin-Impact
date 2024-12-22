#ifndef __ENEMY_MANAGER_H__
#define __ENEMY_MANAGER_H__

#include "cocos2d.h"
#include "Enemy.h"
#include <vector>

class EnemyManager {
public:
    // ��ȡ����ʵ��
    static EnemyManager* getInstance();

    // ���ٵ���ʵ��
    static void destroyInstance();

    // ע����������¼�
    void registerEnemyDeath(Enemy* enemy);

    // ���¹������߼�
    void update(float delta);

private:
    // ˽�й��캯������ֹ�ⲿʵ����
    EnemyManager();
    ~EnemyManager();

    // ����ʵ��
    static EnemyManager* s_instance;

    // ����ˢ����Ϣ�ṹ��
    struct EnemySpawnInfo {
        cocos2d::Vec2 position;
        int level;
        float respawnTime;
        float elapsedTime;
        CharacterElement element;
        bool attackMethod;
        Hero* player;
    };

    // �洢����ˢ����Ϣ���б�
    std::vector<EnemySpawnInfo> spawnInfoList;

    // ���ɵ���
    void spawnEnemy(const EnemySpawnInfo& info);
};

#endif // __ENEMY_MANAGER_H__