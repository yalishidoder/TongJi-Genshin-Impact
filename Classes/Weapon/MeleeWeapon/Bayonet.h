#ifndef __BAYONET_H__
#define __BAYONET_H__
#include "cocos2d.h"

class Bayonet : public cocos2d::Sprite {
public:
    Bayonet();
    ~Bayonet();

    // ��̬��������
    static Bayonet* create(const std::string& textureFileName = "bayonet.png");

    // ��ʼ������
    bool init(const std::string& textureFileName);

    // ���º���
    void update(float delta);

    // ��������
    void attack(float angle = 0);

    // ���ʹ�������˵���ײ
    void checkAndHandleCollision();
    bool checkCollision(cocos2d::Node* target);
    void onCollisionWithEnemy(cocos2d::Node* enemy);

    // ����Bayonet����ת�Ƕ�
    void updateRotation();
    void setMousePosition(const cocos2d::Vec2& position);
#if 0
    cocos2d::Animation* createSwingAnimation();
    void addAnimation(const std::string& animationName, const cocos2d::Animation& animation);
    void playAnimation(const std::string& animationName);
#endif
    // �̵��Ĺ�����
    int attackPower;
protected:
    // �̵��Ĺ�����Χ
    float attackRange;
    
    // �̵��Ĺ�����ȴʱ��
    float attackCooldown;
    // ʣ��Ĺ�����ȴʱ��
    float remainingCooldown;
    // �ӿ�����
    cocos2d::AnimationCache* m_animationCache;
    float m_rotationAngle;
    cocos2d::Vec2 m_mousePosition;

    bool isIdle;
    bool nextDir;
};

#endif