#include "CharacterBase.h"

CharacterBase::CharacterBase()
    : m_health(100)
    , m_full_health(100)
    , m_level(1)
    , m_attackPower(10)
{
}

CharacterBase::~CharacterBase() 
{
}

void CharacterBase::moveTo(const cocos2d::Vec2& targetPosition) 
{
    auto moveAction = cocos2d::MoveTo::create(
        1.0f,  // �ƶ�ʱ�䣬�ɸ���ʵ���������
        targetPosition);
    runAction(moveAction);
}

void CharacterBase::moveBy(const cocos2d::Vec2& offset) 
{
    auto moveAction = cocos2d::MoveBy::create(
        0.01f,  // �ƶ�ʱ�䣬�ɵ���
        offset);
    runAction(moveAction);
}

// ��������ֵ
void CharacterBase::setHealth(int health) 
{
    m_health = health;
}

// ��ȡ����ֵ
int CharacterBase::getHealth()const 
{
    return m_health;
}

// �����������ֵ
void CharacterBase::setMaxHealth(int health)
{
    m_full_health = health;
}

// ��ȡ�������ֵ
int CharacterBase::getMaxHealth() const
{
    return m_full_health;
}

void CharacterBase::Recover()
{
    // ���ڹ�����ԣ�ͬʱ��Ҫ������ҹ������ܻ�Ѫ
    if (this->getPosition() == spawnPoint)
        m_health = m_full_health;
}

// ���ù�����
void CharacterBase::setAttackPower(int attackPower) 
{
    m_attackPower = attackPower;
}

// ��ȡ������
int CharacterBase::getAttackPower() const{
    return m_attackPower;
}

// ���õȼ�
void CharacterBase::setLevel(int level)
{
    m_level = level;
}

// ��ȡ��ǰ�ȼ�
int CharacterBase::getLevel()const
{
    return m_level;
}
