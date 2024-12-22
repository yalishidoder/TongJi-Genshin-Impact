#include "EnemyManager.h"

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

EnemyManager::EnemyManager() : currentSceneID("") {
    // ��ʼ���߼�
}

EnemyManager::~EnemyManager() {
    // �����߼�
}

void EnemyManager::setSceneID(const std::string& sceneID) {
    currentSceneID = sceneID;
}

void EnemyManager::clearScene(const std::string& sceneID) {
    spawnInfoMap.erase(sceneID);
}

void EnemyManager::registerEnemyDeath(Enemy* enemy) {
    if (currentSceneID.empty()) {
        // ���û�����ó���ID������¼����������Ϣ
        return;
    }

    EnemySpawnInfo info;
    info.position = enemy->getSpawnPoint();
    info.level = enemy->getLevel();
    info.element = enemy->getElement();
    info.attackMethod = enemy->getAttackMethod();
    info.player = enemy->getPlayer();
    info.respawnTime = 5.0f; // �������5�������
    info.elapsedTime = 0.0f;

    spawnInfoMap[currentSceneID].push_back(info);
}

void EnemyManager::update(float delta) {
    if (currentSceneID.empty()) {
        // ���û�����ó���ID�������и���
        return;
    }

    auto& spawnList = spawnInfoMap[currentSceneID];
    for (auto it = spawnList.begin(); it != spawnList.end();) {
        it->elapsedTime += delta;

        if (it->elapsedTime >= it->respawnTime) {
            spawnEnemy(*it);
            it = spawnList.erase(it); // �Ƴ��Ѿ�ˢ�µĵ�����Ϣ
        }
        else {
            ++it;
        }
    }
}

void EnemyManager::spawnEnemy(const EnemySpawnInfo& info) {
    Enemy* enemy = Enemy::create(info.position);
    enemy->setInitData(info.level);
    enemy->setElement(info.element);
    enemy->setAttackMethods(info.attackMethod);
    enemy->setPlayer(info.player);

    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene) {
        currentScene->addChild(enemy);
    }
}