/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Enemy.cpp
 * File Function: 敌人类实现
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/
#include "EnemyManager.h"
#include "Character/CharacterBase.h"
#include "../Hero/Hero.h"
#include "Weapon/RangedWeapon/Ammunition/Bullet.h"
#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"
#include <cmath>
#include "AudioEngine.h"

Enemy::Enemy()
    : m_isAlive(true)
    , m_moveSpeed(50.0f)  // 初始移动速度
    , isAttacking(false)  //是否发起进攻
    , m_animationCache(cocos2d::AnimationCache::getInstance())
    , m_currentAnimate(nullptr)
    , currentState(EnemyState::PATROL)   //初始状态为巡逻
    , dirX(1) ,dirY(1)     //初始巡逻方向设为正方向
    , rangedX(200.0f)
    , rangedY(300.0f)
    , attackRange(50.0f)
    , attackCooldown(1.5f)
    , remainingCooldown(0.0f)
    , m_attackPower(5)
    , radius(200.0f)
{

}


Enemy::~Enemy() {
}

bool Enemy::init(const cocos2d::Vec2& initPosition) {
    if (!Sprite::init()) {
        return false;
    }

    // 设置敌人初始位置
    setPosition(initPosition);
    setScale(0.8f);
    spawnPoint = initPosition;

    // 可以在这里加载敌人的初始外观纹理等
    // 例如：setTexture("enemy_default.png");
    damageLabel = cocos2d::Label::createWithTTF("", "fonts/arial.ttf", 24);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    if (damageLabel)
    {
        labelPos.x -= 20;
        labelPos.y += 30;
        damageLabel->setPosition(labelPos);
        this->addChild(damageLabel);
        damageLabel->setVisible(false);
    }

    ERLabel = cocos2d::Label::createWithTTF("", "fonts/arial.ttf", 20);
    if (ERLabel)
    {
        labelPos.x += 110;
        ERLabel->setPosition(labelPos);
        this->addChild(ERLabel);
        ERLabel->setVisible(false);
    }

    freezeSprite= cocos2d::Sprite::create("Character/ER/Crystal_Icon.png");
    if (freezeSprite)
    {
        freezeSprite->setPosition(this->getPosition());
        this->addChild(freezeSprite);
        freezeSprite->setVisible(isControlled);
        freezeSprite->setOpacity(150);
    }

    // 血条背景
    healthBg = cocos2d::Sprite::create("Character/Enemy/health_bg.png");
    if (healthBg) {
        labelPos = this->getAnchorPoint();
        labelPos.x += 30;
        labelPos.y += 60;
        healthBg->setPosition(labelPos);
        healthBg->setVisible(true);
        healthBg->setOpacity(120);
        this->addChild(healthBg);
    }

    // 血条填充
    healthFill = cocos2d::Sprite::create("Character/Enemy/health_Fill.png");
    if (healthFill)
    {
        healthFill->setAnchorPoint(Vec2(0, 0.5));
        healthFill->setPosition(Vec2(labelPos.x / 2 - 11, labelPos.y));
        healthFill->setVisible(true);
        healthBg->setOpacity(120);
        this->addChild(healthFill);
    }

    // 手枪
    enemyPistol = cocos2d::Sprite::create("Weapon/pistol.png");
    if (enemyPistol) {
        enemyPistol->setAnchorPoint(Vec2(-0.1f, 0.5f));
        enemyPistol->setName("enemyPistol");
        Vec2 handpos = getPosition();
        handpos.y += 10;
        enemyPistol->setPosition(handpos);
        enemyPistol->setVisible(0);
        this->addChild(enemyPistol);
    }

    setTexture("Character/Enemy/Animation/hitman/WALK_DOWN_0.png");
    m_animationCache = cocos2d::AnimationCache::getInstance();
    m_animationCache->addAnimation(createWalkUpAnimation(), "walk_up_enemy");
    m_animationCache->addAnimation(createWalkDownAnimation(), "walk_down_enemy");
    m_animationCache->addAnimation(createWalkLeftAnimation(), "walk_left_enemy");
    m_animationCache->addAnimation(createWalkRightAnimation(), "walk_right_enemy");
    m_animationCache->addAnimation(createDeathAnimation(), "death_enemy");
    m_animationCache->addAnimation(createAttackDownAnimation(), "attack_down_enemy");
    m_animationCache->addAnimation(createAttackUpAnimation(), "attack_up_enemy");
    m_animationCache->addAnimation(createAttackLeftAnimation(), "attack_left_enemy");
    m_animationCache->addAnimation(createAttackRightAnimation(), "attack_right_enemy");
    return true;
}

Enemy* Enemy::create(const cocos2d::Vec2& initPosition)  {
    auto enemy = new (std::nothrow) Enemy();
    if (enemy && enemy->init(initPosition)) {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

Hero* Enemy::getPlayer()
{
    return player;
}

bool Enemy::isAlive() const
{
    return m_isAlive;
}

void Enemy::setPatrolRange(float X, float Y)
{
    this->rangedX = X;
    this->rangedY = Y;
}

void Enemy::setInitData(int level)
{
    this->setLevel(level);
    this->setMaxHealth(std::pow(1.2,(level + 1))*100);
    this->setHealth(this->getMaxHealth());
    this->setAttackPower(std::pow(1.2,level + 1)*50);
}

void Enemy::setDeath() {
    if (!m_isAlive) return;

    m_isAlive = false;
    isControlled = true;

    //currentState = EnemyState::STAY;
    cocos2d::experimental::AudioEngine::play2d("Audio/death.mp3", false, 0.5f);
#if 1
    // 播放死亡动画
    playAnimation("death_enemy");

    // 使用延迟动作来延迟移除敌人
    auto delay = cocos2d::DelayTime::create(1.0f);
    auto remove = cocos2d::CallFunc::create([=]() {
        removeFromParentAndCleanup(true);
        });
    auto sequence = cocos2d::Sequence::create(delay, remove, nullptr);
    runAction(sequence);

    // 注册敌人死亡事件
    EnemyManager::getInstance()->registerEnemyDeath(this);
    
#endif
}

void Enemy::Recover(float delta)
{
    cocos2d::Vec2 distanceToSpawnPoint = spawnPoint - this->getPosition();
    float distance = distanceToSpawnPoint.length();

    if (distance < 10.0f)
        this->setHealth(getHealth() + delta * amount_of_healing);
}

void Enemy::update(float delta) 
{
    updateHealthFill();

    if (isControlled)
        return;

    //判定范围内存在玩家再进行操作
    updateHero();

    if (isHeroExist(delta))
    {
        //aiLogic();
        //moveLogic(delta);
        //attackLogic();
    }

    if (remainingCooldown > 0) {
        remainingCooldown -= delta;
    }

    if (isRanged)
        updatePistol();

    Recover(delta);
}

// 简单的元素反应逻辑
// 同元素无伤 
// 火冰融化 水火蒸发 倍率依照原神的规则
// 特殊的：岩元素触碰到其余元素会生成元素盾，这里可以简化成岩元素始终减伤20%
// 水冰相遇会冻结 但只在冰攻击附带水元素的角色的情况下
// 在冻结的情况下，受到岩元素攻击会碎冰

void Enemy::takeDamage(float damage)
{
    if (!m_isAlive)
        return;
    Hero* player = this->getPlayer();

    switch (player->getElement())//元素附加伤害
    {
    case(CharacterElement::FIRE):
        CCLOG("BURN!");
        //火效果
        switch (this->getElement())
        {
            //融化
        case(CharacterElement::ICE):
            damage *= 2.0f;
            ERType = "Melt!";
            break;
            //蒸发
        case(CharacterElement::WATER):
            damage *= 1.5f;
            ERType = "Evaporate!";
            break;
            //抵抗
        case(CharacterElement::ROCK):
            damage *= 0.8f;
            ERType = "Resist!";
            break;
        default:
            break;
        }
        break;
    case(CharacterElement::ICE):
        CCLOG("FREEZE!");
        //冰效果
        switch (this->getElement())
        {
            //融化
        case(CharacterElement::FIRE):
            damage *= 1.5f;
            ERType = "Melt!";
            break;
            //冻结
        case(CharacterElement::WATER):
            ERType = "Freeze!";
            Freeze();
            break;
        case(CharacterElement::ROCK):
            damage *= 0.8f;
            ERType = "Resist!";
            break;
        default:
            break;
        }
        break;
    case(CharacterElement::WATER):
        CCLOG("BE WATER MY FRIEND");
        //水效果
        switch (this->getElement())
        {
            //蒸发
        case(CharacterElement::FIRE):
            damage *= 2.0f;
            ERType = "Evaporate!";
            break;
            //冻结
        case(CharacterElement::ICE):
            ERType = "Freeze!";
            Freeze();
            break;
        case(CharacterElement::ROCK):
            damage *= 0.8f;
            ERType = "Resist!";
            break;
        default:
            break;
        }
        break;
    case(CharacterElement::ROCK):
        CCLOG("YOU SHALL NOT PASS!");
        //岩效果
        // 碎冰
        if (isControlled && freezeSprite->isVisible())
            damage *= 3;
        break;
    default:
        break;
    }

    if (this->getElement() == player->getElement())
        damage = 0;

    // 减少敌人的生命值
    float health = this->getHealth() - damage;

    const int player_lv = player->getLevel();
    const int enemy_lv = this->getLevel();
    const int lv_gap = (enemy_lv - player_lv) < 5 ? std::pow(1.2, enemy_lv - player_lv + 1) : enemy_lv - player_lv;    // 等级差弥补

    // 更新伤害数字显示
    updateDamageLabel(damage);
    // 更新元素反应显示
    updateERLabel();
    player->addExp(log10(damage + 10) * lv_gap / 10 + 10);  // 伤害经验
    cocos2d::experimental::AudioEngine::play2d("Audio/enemy_hit.mp3", false, 0.5f);
    // 检查敌人是否死亡    
    if (health < 0) {
        
        player->addExp(std::pow(1.2, lv_gap) * 5 + 50);  // 击杀等级经验
        this->setDeath();
    }
    else
        this->setHealth(health);


}

// 更新伤害数字显示
void Enemy::updateDamageLabel(int damage)
{

    damageLabel->setString('-'+std::to_string(damage));
    damageLabel->setVisible(true);
    // 初始颜色
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    damageLabel->setTextColor(initialColor);

    // 闪烁颜色
    cocos2d::Color4B blinkColor = cocos2d::Color4B::BLACK;
    float blinkInterval = 0.05f;
    int blinkTimes = 1;

    // 闪烁动作
    cocos2d::ActionInterval* blinkAction = cocos2d::Repeat::create(cocos2d::Sequence::create(
        cocos2d::TintTo::create(blinkInterval, blinkColor.r, blinkColor.g, blinkColor.b),
        cocos2d::TintTo::create(blinkInterval, initialColor.r, initialColor.g, initialColor.b),
        nullptr
    ), blinkTimes);

    // 目标颜色
    cocos2d::Color3B targetColor = cocos2d::Color3B::RED;

    // 颜色渐变动作
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // 最终隐藏动作
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            damageLabel->setVisible(false);
            }),
        nullptr
    );

    // 运行动作
    damageLabel->runAction(hideAction);
}

// 更新元素反应显示
void Enemy::updateERLabel()
{
    ERLabel->setString(ERType);
    ERLabel->setVisible(true);
    // 初始颜色
    cocos2d::Color4B initialColor = cocos2d::Color4B::WHITE;
    ERLabel->setTextColor(initialColor);

    // 闪烁颜色
    cocos2d::Color4B blinkColor = cocos2d::Color4B::BLACK;
    float blinkInterval = 0.05f;
    int blinkTimes = 1;

    // 闪烁动作
    cocos2d::ActionInterval* blinkAction = cocos2d::Repeat::create(cocos2d::Sequence::create(
        cocos2d::TintTo::create(blinkInterval, blinkColor.r, blinkColor.g, blinkColor.b),
        cocos2d::TintTo::create(blinkInterval, initialColor.r, initialColor.g, initialColor.b),
        nullptr
    ), blinkTimes);

    // 目标颜色
    cocos2d::Color3B targetColor = cocos2d::Color3B::WHITE;

    if(ERType=="Melt!")
        targetColor = cocos2d::Color3B::RED;
    else if(ERType == "Freeze!")
        targetColor = cocos2d::Color3B::BLUE;
    else if(ERType=="Resist!")
        targetColor = cocos2d::Color3B::GRAY;

    // 颜色渐变动作
    cocos2d::ActionInterval* colorAction = cocos2d::TintTo::create(0.25f, targetColor.r, targetColor.g, targetColor.b);

    // 最终隐藏动作
    cocos2d::ActionInterval* hideAction = cocos2d::Sequence::create(
        blinkAction,
        colorAction,
        cocos2d::CallFunc::create([=]() {
            ERLabel->setVisible(false);
            }),
        nullptr
    );

    // 运行动作
    ERLabel->runAction(hideAction);
}

// 设置冰冻效果
void Enemy::Freeze()
{
    applyControl(FreezeTime);
    cocos2d::Vec2 labelPos = this->getAnchorPoint();
    labelPos.x += 30;
    labelPos.y += 30;
    freezeSprite->setPosition(labelPos);
    freezeSprite->setVisible(isControlled);

    // 使用scheduleOnce安排一个延迟任务
    this->scheduleOnce(schedule_selector(Enemy::hideFreezeSprite), FreezeTime);

    /*this->scheduleOnce([this](float dt) {
        freezeSprite->setVisible(false);
        }, FreezeTime);*/
}

// 添加回调函数
void Enemy::hideFreezeSprite(float dt)
{
    freezeSprite->setVisible(false);
}

// 更新血条
void Enemy::updateHealthFill()
{
    float healthRatio = this->getHealth() / float(this->getMaxHealth());
    if (healthFill)
    {
        healthFill->setScaleX(healthRatio);
    }
}

// 获取敌人元素
CharacterElement Enemy::getElement()
{
    return element;
}

void Enemy::setAttackMethods(bool method)
{
    isRanged = method;
}

bool Enemy::getAttackMethod()const 
{
    return isRanged;
}

void Enemy::attack() {
    // 敌人的攻击逻辑，可根据需要扩展，例如发射子弹、近战攻击等
    // 这里仅作示例，可添加具体的攻击实现
    CCLOG("Enemy attacks with power %d", m_attackPower);
}

// 拳头攻击的逻辑
void Enemy::attackWithPunch()
{
    if (remainingCooldown > 0) {
        return;
    }

    isAttacking = true;

    // 播放动画
    // 根据当前方向选择动画名称
    std::string animationName;
    switch (currentDirection) {
        case Direction::UP:
            animationName = "attack_up_enemy";
            break;
        case Direction::DOWN:
            animationName = "attack_down_enemy";
            break;
        case Direction::LEFT:
            animationName = "attack_left_enemy";
            break;
        case Direction::RIGHT:
            animationName = "attack_right_enemy";
            break;
        default:
            animationName = "walk_up_enemy";
            break;
    }

    // 播放攻击动画，并设置完成回调
    playAnimation(animationName);
    checkAndHandleCollision();

    isAttacking = false;

    // 重置冷却时间
    remainingCooldown = attackCooldown;
    CCLOG("Enemy attacks with power %d", m_attackPower);
}

// 更新手枪的位置
void Enemy::updatePistol()
{
    if (enemyPistol) {
        enemyPistol->setAnchorPoint(Vec2(-0.1f, 0.5f));
        Vec2 handpos = getPosition();
        handpos.x -= 250;
        handpos.y -= 275;
        enemyPistol->setPosition(handpos);
        enemyPistol->setVisible(0);
    }

    if (player)
    {
        if (player->getPosition() != cocos2d::Vec2::ZERO) {
            // 计算手枪指向玩家位置的角度
            float angle = atan2(player->getPositionY() - this->getPositionY(), player->getPositionX() - this->getPositionX());
            // 将角度转换为度数
            angle = CC_RADIANS_TO_DEGREES(-angle);
            // 设置手枪的旋转角度
            if (currentState == EnemyState::CHASE)
                enemyPistol->setRotation(-angle);

        }
    }
}

// 手枪攻击的逻辑
void Enemy::attackWithPistol(const Vec2& position)
{
    if (remainingCooldown > 0) {
        return;
    }

    // 创建子弹
    auto bullet = Bullet::create(cocos2d::Vec2::ZERO, cocos2d::Vec2::ZERO, this->getLevel());
    if (bullet) {
        bullet->setOwner(this);

        // 加载子弹图形资源
        switch (this->getElement()) {
            case(CharacterElement::FIRE):
                bullet->setTexture("Weapon/Ammunition/bullet_fire.png");
                break;
            case(CharacterElement::ICE):
                bullet->setTexture("Weapon/Ammunition/bullet_ice.png");
                break;
            case(CharacterElement::WATER):
                bullet->setTexture("Weapon/Ammunition/bullet_water.png");
                break;
            case(CharacterElement::ROCK):
                bullet->setTexture("Weapon/Ammunition/bullet_rock.png");
                break;
            default:
                bullet->setTexture("Weapon/Ammunition/bullet_default.png");
                break;
        }

        // 设置锚点
        bullet->setAnchorPoint(Vec2(0.5f, 0.5f));

        // 设置子弹的位置为敌人的位置
        bullet->setPosition(getPosition());
        // 计算子弹的移动方向
        Vec2 direction = position - getPosition();
        float distanceToClick = direction.length();

        // 设置最小移动距离
        const float minDistance = 10.0f;
        if (distanceToClick < minDistance) {
            direction = Vec2(minDistance, 0);
            distanceToClick = minDistance;
        }

        // 在攻击范围内
        if (distanceToClick <= 1000.0f) {
            float angle = std::atan2(direction.y, direction.x) * (180.0f / M_PI);
            bullet->setRotation(-angle);
            // 计算速度
            float speed = 100.0f * (log10(bullet->getLevel()) + 3);
            float time = distanceToClick / speed;

            // 移动子弹到点击位置
            auto moveAction = MoveTo::create(time, position);
            auto removeAction = RemoveSelf::create();
            bullet->runAction(Sequence::create(moveAction, removeAction, nullptr));
            // 添加子弹到主场景
            this->getParent()->addChild(bullet);
            CCLOG("Angle: %f", angle);
        }
        CCLOG("Enemy position: %f, %f", getPosition().x, getPosition().y);
        CCLOG("Player position: %f, %f", position.x, position.y);

        // 重置冷却时间
        remainingCooldown = attackCooldown;
        CCLOG("Enemy attacks with power %d", m_attackPower);
    }
}

void Enemy::checkAndHandleCollision()
{
    if (player&&player->isAlive()) {
        if (this->checkCollision(player))
        {
            // 处理碰撞逻辑
            onCollisionWithHero(player);
        }
    }
}

// 处理与玩家间的碰撞
bool Enemy::checkHeroCollision(cocos2d::Node* target)
{
    float distance = this->getPosition().distance(target->getPosition());
    float radius1 = this->getContentSize().width / 2;
    float radius2 = target->getContentSize().width / 2;
    return distance < (radius1 + radius2 - 2);
}

void Enemy::onCollisionWithHero(cocos2d::Node* player)
{
    static_cast<Hero*>(player)->takeDamage(m_attackPower, this);
}

bool Enemy::checkCollision(cocos2d::Sprite* target) {
    auto thisBoundingBox = getBoundingBox();
    auto targetBoundingBox = target->getBoundingBox();
    return thisBoundingBox.intersectsRect(targetBoundingBox);
}

void Enemy::setPlayer(Hero* player)
{
    this->player = player;
}

void Enemy::setRadius(float radius = 100.0f)
{
    this->radius = radius;
}

void Enemy::updateHero()
{
    // 获取当前场景
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();

    if (currentScene)
    {
        // 更新场景中的玩家
        auto heros = currentScene->getChildren();
        for (auto hero : heros)
        {
            auto temp = dynamic_cast<Hero*>(hero);
            if (temp && temp->isAlive())
                setPlayer(temp);
        }
    }
}

//判定范围内是否存在玩家
bool Enemy::isHeroExist(float dt)
{
    if (player)
    {
        cocos2d::Vec2 playerPos = player->getPosition();
        cocos2d::Vec2 enemyPos = this->getPosition();
        cocos2d::Vec2 directionToPlayer = playerPos - enemyPos;
        float distanceToPlayer = directionToPlayer.length();
       
        switch (currentState)
        {
            case EnemyState::PATROL:
                patrolLogic(dt, distanceToPlayer);
                break;
            case EnemyState::CHASE:
                chaseLogic(dt, playerPos, enemyPos, directionToPlayer, distanceToPlayer);
                break;
            case EnemyState::STAY:
                stayLogic(distanceToPlayer);
                break;
            case EnemyState::RETURN:
                returnToSpawnLogic(dt, enemyPos, distanceToPlayer, playerPos);
                break;
        }
        return true;
    }
    return false;
}

void Enemy::patrolLogic(float& dt, float& distanceToPlayer)
{
    patrol(dt);

    if (distanceToPlayer <= radius)
    {
        currentState = EnemyState::CHASE;
    }
}

void Enemy::chaseLogic(float& dt, cocos2d::Vec2& playerPos, cocos2d::Vec2& enemyPos, cocos2d::Vec2& directionToPlayer, float& distanceToPlayer)
{
    if (distanceToPlayer > radius)
    {
        currentState = EnemyState::RETURN;
    }
    else
    {
        if(!isRanged)
        {
            if (distanceToPlayer > 10)
            {
                handleChasingMovement(dt, directionToPlayer);
            }
            else
            {
                currentState = EnemyState::STAY;
            }
        }
        else {
            // 远程攻击函数
            if (distanceToPlayer > 150.0f)
            {
                handleChasingMovement(dt, directionToPlayer);
            }
            else
                attackWithPistol(playerPos);
        }
    }
}

void Enemy::stayLogic(float& distanceToPlayer)
{
    if (distanceToPlayer > 10 && m_isAlive)
    {
        currentState = EnemyState::PATROL;
    }
    else 
    {
       attackWithPunch();
        // 处理近战攻击逻辑
    }
}

void Enemy::returnToSpawnLogic(float& dt, cocos2d::Vec2& enemyPos, float& distanceToPlayer, cocos2d::Vec2& playerPos)
{
    handleReturnToSpawnMovement(dt, enemyPos);

    if (distanceToPlayer <= radius)
    {
        currentState = EnemyState::CHASE;
    }
}

void Enemy::handleChasingMovement(float& dt, cocos2d::Vec2& directionToPlayer)
{
    directionToPlayer.normalize();
    cocos2d::Vec2 movement = directionToPlayer * m_moveSpeed * dt;

    // 根据移动方向调整状态
    if (std::abs(directionToPlayer.x) > std::abs(directionToPlayer.y)) {
        if (directionToPlayer.x > 0) {
            currentDirection = Direction::RIGHT;
        }
        else if (directionToPlayer.x < 0) {
            currentDirection = Direction::LEFT;
        }
    }
    else {
        if (directionToPlayer.y > 0) {
            currentDirection = Direction::UP;
        }
        else if (directionToPlayer.y < 0) {
            currentDirection = Direction::DOWN;
        }
    }

    // 播放动画
   // 根据当前方向选择动画名称
    std::string animationName;
    switch (currentDirection) {
        case Direction::UP:
            animationName = "walk_up_enemy";
            break;
        case Direction::DOWN:
            animationName = "walk_up_enemy";
            break;
        case Direction::LEFT:
            animationName = "walk_left_enemy";
            break;
        case Direction::RIGHT:
            animationName = "walk_right_enemy";
            break;
        default:
            animationName = "walk_up_enemy";
            break;
    }

    // 播放攻击动画，并设置完成回调
    playAnimation(animationName);

    CCLOG("directionToPlayer: (%.2f, %.2f)", directionToPlayer.x, directionToPlayer.y);

    CCLOG("Enemy position before move: (%.2f, %.2f)", this->getPositionX(), this->getPositionY());
    cocos2d::Vec2 targetPosition = this->getPosition() + movement;

    bool collision = checkCollisionWithScene(targetPosition);

    if (!collision)
    {
        this->setPosition(this->getPosition() + movement);
        CCLOG("Enemy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
    }
    else
    {
        currentState = EnemyState::RETURN;
        CCLOG("Enemy touched the wall");
    }

    float angle = std::atan2(directionToPlayer.y, directionToPlayer.x) * (180.0f / M_PI);
    //this->setRotation(-angle);
}

void Enemy::handleReturnToSpawnMovement(float& dt, cocos2d::Vec2& enemyPos)
{
    cocos2d::Vec2 directionToSpawn = spawnPoint - enemyPos;
    float distanceToSpawn = directionToSpawn.length();

    if (distanceToSpawn > 1.0f)
    {
        directionToSpawn.normalize();
        cocos2d::Vec2 movement = directionToSpawn * m_moveSpeed * dt;
        this->setPosition(this->getPosition() + movement);

        float angle = std::atan2(directionToSpawn.y, directionToSpawn.x) * (180.0f / M_PI);
        //this->setRotation(angle);
    }
    else
    {
        currentState = EnemyState::PATROL;
        setRotation(0.0f);
        dirX = 1;
    }
}

bool Enemy::checkCollisionWithScene(cocos2d::Vec2& targetPosition)
{
    cocos2d::Scene* currentScene = cocos2d::Director::getInstance()->getRunningScene();
    if (currentScene)
    {
        if (dynamic_cast<MainScene*>(currentScene))
        {
            MainScene* scene = dynamic_cast<MainScene*>(currentScene);
            return scene->checkCollision(targetPosition);
        }
        else if (dynamic_cast<OtherScene*>(currentScene))
        {
            OtherScene* scene = dynamic_cast<OtherScene*>(currentScene);
            return scene->checkCollision(targetPosition);
        }
    }
    return false;
}

void Enemy::patrol(float delta)
{
    // 巡逻逻辑，例如沿着路径移动或随机移动
    // 示例：简单地左右移动
    //后续可以添加向上下巡逻的功能

    cocos2d::Vec2 moveDirection = cocos2d::Vec2(dirX * m_moveSpeed * delta, 0);
    cocos2d::Vec2 targetPosition = getPosition() + moveDirection;
    playAnimation("walk_right_enemy");

    // 获取当前场景进行碰撞检测
    bool collision = checkCollisionWithScene(targetPosition);

    CCLOG("Collision check: Target Position = (%.2f, %.2f), Collision = %s",
        targetPosition.x, targetPosition.y, collision ? "True" : "False");

    //大于右边界,则规定向左移动
    if (this->getPositionX() >= spawnPoint.x + rangedX)
    {
        dirX = -1;
        playAnimation("walk_left_enemy");
    }
    //小于左边界，则规定向右移动
    else if (this->getPositionX() <= spawnPoint.x - rangedX)
    {
        dirX = 1;
        playAnimation("walk_right_enemy");
    }

    // 如果没有碰撞，执行移动
    if (!collision)
    {
        moveBy(moveDirection);
        CCLOG("Enemy moved to new position: (%.2f, %.2f)", targetPosition.x, targetPosition.y);
    }
    else // 有碰撞，反向移动
    {
        // 向右移动碰到墙壁，反向向左
        if (dirX == 1)
        {
            dirX = -1;
            playAnimation("walk_left_enemy");
        }
        // 向左移动碰到墙壁，反向向右
        else if (dirX == -1)
        {
            dirX = 1;
            playAnimation("walk_right_enemy");
        }
    }
}

void Enemy::moveLogic(float dt) {
    // 简单的随机移动逻辑，可根据需要扩展为追逐玩家或其他逻辑
    cocos2d::Vec2 moveDirection;
    float randomX = CCRANDOM_MINUS1_1();
    float randomY = CCRANDOM_MINUS1_1();
    moveDirection.x = randomX * m_moveSpeed * dt;
    moveDirection.y = randomY * m_moveSpeed * dt;
    
    
    if (moveDirection != cocos2d::Vec2::ZERO) {
        moveBy(moveDirection);
    }
}

void Enemy::attackLogic() {
    // 简单的攻击逻辑，可根据需要扩展为更复杂的攻击判断和触发条件
    if (CCRANDOM_0_1() < 0.01) {  // 1% 的概率攻击，可调整
        attack();
    }
}


void Enemy::aiLogic() {
    // 这里可以实现更复杂的 AI 逻辑，例如巡逻、追逐玩家、躲避玩家等
    // 暂时为空，可根据游戏需求添加

}

// 创建向上行走动画
cocos2d::Animation* Enemy::createWalkUpAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_UP.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_UP.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_UP_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// 创建向下行走动画
cocos2d::Animation* Enemy::createWalkDownAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_DOWN.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_DOWN.png");
    
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_DOWN_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// 创建向左行走动画
cocos2d::Animation* Enemy::createWalkLeftAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_LEFT.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_LEFT.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_LEFT_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    return animation;
}

// 创建向右行走动画
cocos2d::Animation* Enemy::createWalkRightAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_RIGHT.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/WALK_RIGHT.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 2; i <= 8; ++i) {
        std::string frameName = StringUtils::format("Character/Enemy/Animation/WALK_RIGHT_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// 创建死亡动画
cocos2d::Animation* Enemy::createDeathAnimation() {
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/ENEMY_DEATH.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/ENEMY_DEATH.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 6; ++i) {
        std::string frameName = StringUtils::format("DEATH-%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

cocos2d::Animation* Enemy::createAttackDownAnimation()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_DOWN_ENEMY.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_DOWN_ENEMY.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 11; ++i) {
        std::string frameName = StringUtils::format("ATTACK_DOWN_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

cocos2d::Animation* Enemy::createAttackUpAnimation()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_UP_ENEMY.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_UP_ENEMY.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 11; ++i) {
        std::string frameName = StringUtils::format("ATTACK_UP_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

cocos2d::Animation* Enemy::createAttackLeftAnimation()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_LEFT_ENEMY.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_LEFT_ENEMY.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 11; ++i) {
        std::string frameName = StringUtils::format("ATTACK_LEFT_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

cocos2d::Animation* Enemy::createAttackRightAnimation()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_RIGHT_ENEMY.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Character/Enemy/Animation/punch/ATTACK_RIGHT_ENEMY.png");
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames;

    for (int i = 0; i <= 11; ++i) {
        std::string frameName = StringUtils::format("ATTACK_RIGHT_%d.png", i);
        cocos2d::SpriteFrame* frame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            animFrames.pushBack(frame);
        }
        else {
            log("Failed to load frame: %s", frameName.c_str());
        }
    }
    // 创建动画，设置帧间隔为 frameDelay 秒
    cocos2d::Animation* animation = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.1f);
    //animation->setLoops(-1);
    return animation;
}

// 播放指定名称的动画
void Enemy::playAnimation(const std::string& animationName) {
    auto animation = m_animationCache->getAnimation(animationName);
    if (animation) {
        m_currentAnimate = cocos2d::Animate::create(animation);
        runAction(m_currentAnimate);

    }
}

// 停止当前动画
void Enemy::stopAnimation() {
    //stopAllActionsByTag(100);  // 这里用100作为动画相关action的标签，可自定义
    stopAllActions();
    CC_SAFE_RELEASE_NULL(m_currentAnimate);
}

// 添加动画到缓存
void Enemy::addAnimation(const std::string& animationName, const cocos2d::Animation& animation) {
    // 创建一个新的 Animation 对象，将传入的 animation 复制到新对象中
    auto newAnimation = Animation::create();
    newAnimation->setDelayPerUnit(animation.getDelayPerUnit());
    newAnimation->setLoops(animation.getLoops());
    newAnimation->setRestoreOriginalFrame(animation.getRestoreOriginalFrame());
    for (const auto& frame : animation.getFrames()) {
        // 从 AnimationFrame 中提取 SpriteFrame 并添加
        SpriteFrame* spriteFrame = frame->getSpriteFrame();
        if (spriteFrame) {
            newAnimation->addSpriteFrame(spriteFrame);
        }
    }
    m_animationCache->addAnimation(newAnimation, animationName);
}