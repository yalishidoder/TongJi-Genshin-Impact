#include "EnemyManager.h"

// 初始化单例实例
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
    // 初始化逻辑
}

EnemyManager::~EnemyManager() {
    // 清理逻辑
}

void EnemyManager::registerEnemyDeath(Enemy* enemy) {
    // 记录敌人死亡信息
    EnemySpawnInfo info;
    info.position = enemy->getSpawnPoint();
    info.level = enemy->getLevel();
    info.element = enemy->getElement();
    info.attackMethod = enemy->getAttackMethod();
    info.player = enemy->getPlayer();
    info.respawnTime = 5.0f; // 假设敌人5秒后重生
    info.elapsedTime = 0.0f;

    spawnInfoList.push_back(info);
}

void EnemyManager::update(float delta) {
    // 遍历所有需要刷新的敌人
    for (auto it = spawnInfoList.begin(); it != spawnInfoList.end(); ) {
        it->elapsedTime += delta;

        // 如果时间到了，生成敌人
        if (it->elapsedTime >= it->respawnTime) {
            spawnEnemy(*it);
            it = spawnInfoList.erase(it); // 移除已经刷新的敌人信息
        }
        else {
            ++it;
        }
    }
}

void EnemyManager::spawnEnemy(const EnemySpawnInfo& info) {
    // 创建并初始化敌人
    Enemy* enemy = Enemy::create(info.position);
    enemy->setInitData(info.level);
    enemy->setElement(info.element);
    enemy->setAttackMethods(info.attackMethod);
    enemy->setPlayer(info.player);

    // 获取当前场景并添加敌人
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene) {
        currentScene->addChild(enemy);
    }
}