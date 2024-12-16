/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Hero.cpp
 * File Function: ���˹�������
 * Author:        kfx
 * Update Date:   2024.12.10
 ****************************************************************/

#ifndef __HERO_H__
#define __HERO_H__

#include <vector>
#include "cocos2d.h"
#include "Character/CharacterBase.h"
#include "Weapon/Weapon.h"
#include "Weapon/Switcher.h"
USING_NS_CC;

class Hero : public CharacterBase {
public:
    // ���캯������������
    Hero();
    virtual ~Hero();

    // create �������ڴ�������ʼ����ɫʵ��
    static Hero* create(const cocos2d::Vec2& initPosition);

    // ��ɫ��ʼ���������ɴ����ʼλ�õȲ���
    virtual bool init(const cocos2d::Vec2& initPosition)override;

    // ��ɫ�ƶ���ط���
    void moveTo(const cocos2d::Vec2& targetPosition);
    void moveBy(const cocos2d::Vec2& offset);
    virtual void Hero::update(float dt)override;

    // ��������
    virtual void attack()override;
    void attackWithBullet(const Vec2& position);
    void attackWithBayonet();

    // ��ɫ������ط���
    void playAnimation(const std::string& animationName);
    void stopAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    cocos2d::Animation* Hero::createWalkUpAnimation();
    cocos2d::Animation* Hero::createWalkDownAnimation();
    cocos2d::Animation* Hero::createWalkLeftAnimation();
    cocos2d::Animation* Hero::createWalkRightAnimation();

    // ��ɫ������ط������������úͻ�ȡ����ֵ����������
    bool getGender();
    void setGender(bool is_male);
    CharacterElement getElement();

    // ��ɫ�ȼ��뾭�����
    void addExp(int exp);
    void LevelUp();
    int calculateExpToLevelUp()const;

    // ��ɫ״̬��أ������Ƿ�����ж�
    bool isAlive()const;

    // ��ײ�����أ��ɸ��ݾ�����ײ�߼���չ��
    bool checkCollision(Hero* otherHero);
        
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

    int MaxLevel;
    int m_sleepiness;
    int m_heroism;
    int m_exp;
    int m_expToLevelUp;
    bool m_ismale;

    //�������
    float m_skillCoolDownTime;     //��ȴʱ��
    bool m_isSkillOnCoolDown;      //�Ƿ���ȴ���
    float m_skillDuration;         //����ʱ��

    cocos2d::AnimationCache* m_animationCache;
    cocos2d::Animate* m_currentAnimate;

    WeaponSwitcher* m_weaponSwitcher;
};
#endif