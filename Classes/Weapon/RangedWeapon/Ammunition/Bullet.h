#ifndef __BULLET_H__
#define __BULLET_H__

#include "cocos2d.h"
#include "Character/Enemy/Enemy.h"
class Bullet : public cocos2d::Sprite
{
public:
    Bullet();
    ~Bullet();

    // �����ӵ��Ĺ�������
    static Bullet* create(const cocos2d::Vec2& position = cocos2d::Vec2::ZERO, const cocos2d::Vec2& velocity = cocos2d::Vec2::ZERO, const int level = 1);

    // ��ʼ���ӵ�
    bool init(const cocos2d::Vec2& position, const cocos2d::Vec2& velocity, const int level);

    // �����ӵ�״̬
    void update(float delta);

    // �����ӵ��ٶ�
    void setVelocity(const cocos2d::Vec2& velocity);

    // ��ȡ�ӵ��ٶ�
    cocos2d::Vec2 getVelocity() const;

    // �����ӵ���������
    void setLifeTime(float lifeTime);

    // ��ȡ�ӵ���������
    float getLifeTime() const;

    // �����ӵ��ȼ�
    void setLevel(int level);

    // ��ȡ�ӵ��ȼ�
    int getLevel() const;

    // �ӵ���ײ���
    void checkAndHandleCollision();
    bool checkCollision(cocos2d::Node* target);
    void onCollisionWithEnemy(cocos2d::Node* enemy);
    

protected:
    // �ӵ��ٶ�
    cocos2d::Vec2 _velocity;

    // �ӵ���������
    float _lifeTime;

    // �ӵ�ʣ������
    float _remainingLife;

    // �ӵ��ȼ�
    int _level;

    
};

#endif // __BULLET_H__
