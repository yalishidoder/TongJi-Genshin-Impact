/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹�������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#ifndef __HERO_H__
#define __HERO_H__

#include "cocos2d.h"
#include "Weapon/Weapon.h"
#include "Weapon/Switcher.h"
USING_NS_CC;

class Character : public cocos2d::Sprite {
public:
    // ���캯������������
    Character();
    virtual ~Character();

    // create �������ڴ�������ʼ����ɫʵ��
    static Character* create(const cocos2d::Vec2& initPosition);

    // ��ɫ��ʼ���������ɴ����ʼλ�õȲ���
    virtual bool init(const cocos2d::Vec2& initPosition);

    // ��ɫ�ƶ���ط���
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);
    void Character::update(float dt);

    // ��ɫ������ط���
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    cocos2d::Animation* Character::createWalkUpAnimation();
    cocos2d::Animation* Character::createWalkDownAnimation();
    cocos2d::Animation* Character::createWalkLeftAnimation();
    cocos2d::Animation* Character::createWalkRightAnimation();



    // ��ɫ������ط������������úͻ�ȡ����ֵ����������
    void setHealth(int health);
    int getHealth();
    void setAttackPower(int attackPower);
    int getAttackPower();
    bool getGender();
    void setGender(bool is_male);

    // ��ɫ״̬��أ������Ƿ�����ж�
    bool isAlive();

    // ��ײ�����أ��ɸ��ݾ�����ײ�߼���չ��
    bool checkCollision(Character* otherCharacter);
        
    cocos2d::Vec2 m_moveDirection;
    bool m_moveUp;
    bool m_moveDown;
    bool m_moveLeft;
    bool m_moveRight;

    // �����л����
    void addWeapon(Weapon* weapon);
    void removeWeapon(Weapon* weapon);
    void switchToNextWeapon();
    void switchToPreviousWeapon();
    Weapon* getCurrentWeapon();

private:
    bool m_isAlive;

    float m_speed;

    int m_sleepiness;
    int m_health;
    int m_attackPower;
    int m_heroism;
    bool m_ismale;

    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;

    WeaponSwitcher* m_weaponSwitcher;
};
#endif