#include "EnemyManager.h"

// ��ʼ������ʵ��
EnemyManager* EnemyManager::s_instance = nullptr;

EnemyManager* EnemyManager::getInstance() {
    if (!s_instance) {
        s_instance = new (std::nothrow) EnemyManager();
    }
    return s_instance;
}

void EnemyManager::destroyInstance() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

EnemyManager::EnemyManager() {
    // ��ʼ���߼�
}

EnemyManager::~EnemyManager() {
    // �����߼�
}

void EnemyManager::registerEnemyDeath(Enemy* enemy) {
    // ��¼����������Ϣ
    EnemySpawnInfo info;
    info.position = enemy->getSpawnPoint();
    info.level = enemy->getLevel();
    info.element = enemy->getElement();
    info.attackMethod = enemy->getAttackMethod();
    info.player = enemy->getPlayer();
    info.respawnTime = 5.0f; // �������5�������
    info.elapsedTime = 0.0f;

    spawnInfoList.push_back(info);
}

void EnemyManager::update(float delta) {
    // ����������Ҫˢ�µĵ���
    for (auto it = spawnInfoList.begin(); it != spawnInfoList.end(); ) {
        it->elapsedTime += delta;

        // ���ʱ�䵽�ˣ����ɵ���
        if (it->elapsedTime >= it->respawnTime) {
            spawnEnemy(*it);
            it = spawnInfoList.erase(it); // �Ƴ��Ѿ�ˢ�µĵ�����Ϣ
        }
        else {
            ++it;
        }
    }
}

void EnemyManager::spawnEnemy(const EnemySpawnInfo& info) {
    // ��������ʼ������
    Enemy* enemy = Enemy::create(info.position);
    enemy->setInitData(info.level);
    enemy->setElement(info.element);
    enemy->setAttackMethods(info.attackMethod);
    enemy->setPlayer(info.player);

    // ��ȡ��ǰ��������ӵ���
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene) {
        currentScene->addChild(enemy);
    }
}