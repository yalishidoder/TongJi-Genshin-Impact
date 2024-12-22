#ifndef __CHARACTER_BASE_H__
#define __CHARACTER_BASE_H__

#include "cocos2d.h"
#include <string>

// ��ɫ������
enum class CharacterElement
{
    FIRE,   //��
    WATER,  //ˮ
    ICE,    //��
    ROCK    //��
};

const float FreezeTime = 1.0f;

class CharacterBase : public cocos2d::Sprite
{
public:
    // ���캯������������
    CharacterBase();
    virtual ~CharacterBase();

    // ��ʼ������
    virtual bool init(const cocos2d::Vec2& initPosition) = 0;

    // ��ȡ����������
    virtual void setCharacterName(const std::string& Name);
    virtual std::string getCharacterName();

    void setSpawnPoint(const cocos2d::Vec2& position);
    cocos2d::Vec2 getSpawnPoint()const;

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

    // ���ý�ɫԪ��
    virtual void setElement(CharacterElement Elementpower);

    // �ص�������ָ�����ֵ 
    virtual void Recover(float delta);

    // ���ÿ���ʱ�������״̬
    virtual void applyControl(float duration);
    virtual void setControlled(bool controlled);
    virtual void onControlEnd(float dt);
protected:
    // ��ɫ�ض�����
    std::string m_name;

    // Ԫ�ط�Ӧ����
    std::string ERType;

    // ����ֵ
    int m_health;
    int m_full_health;

    // ������
    int m_attackPower;

    // �ȼ�
    int m_level;
    int MaxLevel;

    bool isControlled;
    // Ԫ��
    CharacterElement element;

    // ������
    // ���н�ɫ���س�����ָ���������ֵ
    cocos2d::Vec2 spawnPoint;

    // �˺���ǩ
    cocos2d::Label* damageLabel;

    // Ԫ�ط�Ӧ��ǩ
    cocos2d::Label* ERLabel;

    // Ԫ�ط�Ӧ��ǩ
    cocos2d::Label* recoverLabel;

    cocos2d::Sprite* freezeSprite;
};

#endif // __CHARACTER_BASE_H__
