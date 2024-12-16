#ifndef __CHARACTER_BASE_H__
#define __CHARACTER_BASE_H__

#include "cocos2d.h"

// ��ɫ������
enum class CharacterElement
{
    FIRE,   //��
    WATER,  //ˮ
    ICE,    //��
    ROCK    //��
};

class CharacterBase : public cocos2d::Sprite 
{
public:
    // ���캯������������
    CharacterBase();
    virtual ~CharacterBase();

    // ��ʼ������
    virtual bool init(const cocos2d::Vec2& initPosition) = 0;

    // ��ɫ���·���
    virtual void update(float delta) = 0;

    // �ƶ�����
    virtual void moveTo(const cocos2d::Vec2& targetPosition);
    virtual void moveBy(const cocos2d::Vec2& offset);

    // ��������
    virtual void attack() = 0;

    // ��ȡ����������ֵ
    virtual void setHealth(int health);
    virtual void setMaxHealth(int health);
    virtual int getHealth() const;
    virtual int getMaxHealth()const;

    // ��ȡ�����ù�����
    virtual void setAttackPower(int attackPower);
    virtual int getAttackPower() const;

    // ��ȡ�����õȼ�
    virtual int getLevel()const;
    virtual void setLevel(int level);

    // �ص�������ָ�����ֵ 
    virtual void Recover();
protected:
    // ����ֵ
    int m_health;
    int m_full_health;

    // ������
    int m_attackPower;

    // �ȼ�
    int m_level;
    int MaxLevel;

    CharacterElement element;

    // ������
    // ���н�ɫ���س�����ָ���������ֵ
    cocos2d::Vec2 spawnPoint;
};

#endif // __CHARACTER_BASE_H__
