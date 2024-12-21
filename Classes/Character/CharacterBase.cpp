#include "CharacterBase.h"

CharacterBase::CharacterBase()
    : m_health(100)
    , m_full_health(100)
    , m_level(1)
    , m_attackPower(10)
    , isControlled(false)
{
}

CharacterBase::~CharacterBase() 
{
}

// 设置角色的特定名字
void CharacterBase::setCharacterName(const std::string& Name)
{
    m_name = Name;
}

// 获取角色的特定名字
std::string CharacterBase::getCharacterName()
{
    return m_name;
}

void CharacterBase::moveTo(const cocos2d::Vec2& targetPosition) 
{
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // 移动时间，可根据实际情况调整
        targetPosition);
    runAction(moveAction);
}

void CharacterBase::moveBy(const cocos2d::Vec2& offset) 
{
    auto moveAction = cocos2d::MoveBy::create(
        0.01f,  // 移动时间，可调整
        offset);
    runAction(moveAction);
}

// 设置生命值
void CharacterBase::setHealth(int health) 
{
    m_health = health;
}

// 获取生命值
int CharacterBase::getHealth()const 
{
    return m_health;
}

// 设置最大生命值
void CharacterBase::setMaxHealth(int health)
{
    m_full_health = health;
}

// 获取最大生命值
int CharacterBase::getMaxHealth() const
{
    return m_full_health;
}

void CharacterBase::Recover()
{
    // 对于怪物而言，同时需要不被玩家攻击才能回血
    if (this->getPosition() == spawnPoint)
        m_health = m_full_health;
}

// 设置攻击力
void CharacterBase::setAttackPower(int attackPower) 
{
    m_attackPower = attackPower;
}

// 获取攻击力
int CharacterBase::getAttackPower() const{
    return m_attackPower;
}

// 设置等级
void CharacterBase::setLevel(int level)
{
    m_level = level;
}

// 获取当前等级
int CharacterBase::getLevel()const
{
    return m_level;
}

void CharacterBase::setElement(CharacterElement Elementpower)
{
    element = Elementpower;
}

void CharacterBase::applyControl(float duration)
{
    setControlled(true);

    // 使用scheduleOnce在duration秒后解除控制
    this->scheduleOnce(schedule_selector(CharacterBase::onControlEnd), duration);

    /*this->scheduleOnce([this](float dt)
        {
            setControlled(false);
        }, duration);*/
}

void CharacterBase::setControlled(bool controlled)
{
    isControlled = controlled;
}

void CharacterBase::onControlEnd(float dt)
{
    setControlled(false);
}

