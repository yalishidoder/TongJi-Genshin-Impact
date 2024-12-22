#ifndef __ENEMYHUNT_H__
#define __ENEMYHUNT_H__

#include "cocos2d.h"
#include "Character/Enemy/Enemy.h"
#include "Character/Hero/Hero.h"

class EnemyHunt : public cocos2d::Node
{
public:
    // ����״̬
    enum TaskStatus
    {
        TASK_IN_PROGRESS,  // ���������
        TASK_COMPLETED,    // ���������
        TASK_FAILED        // ����ʧ��
    };

    // ���캯��
    EnemyHunt();
    // ��������
    virtual ~EnemyHunt();

    // ��ʼ������
    bool init(const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const std::vector<Enemy*>& enemies,
        int target_kills);

    // ������̬����
    static EnemyHunt* create(const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const std::vector<Enemy*>& enemies,
        int target_kills);

    // ��������
    void StartTask();

    // ��������״̬
    void Update();

    // ���ܵ���
    void DefeatEnemy(Enemy* enemy);

    // �������
    void CompleteTask();

    // ����ʧ��
    void FailTask();

    // ��ʾ����״̬��Ϣ
    void ShowMessage(const std::string& message);

    // ��ȡ��ǰ����״̬
    TaskStatus GetTaskStatus() const;

    // ����UI
    void CleanupUI();
    //int get_enemies_num(std::vector<Enemy*> enemies);

private:
    cocos2d::Size visible_size_;          // ���������С
    cocos2d::TMXTiledMap* map_;           // ��ͼ
    Hero* hero_;                          // ��ɫ
    std::vector<Enemy*> enemies_;         // �����б�
    int target_kills_;                    // Ŀ���ɱ��
    int current_kills_;                   // ��ǰ��ɱ��
    TaskStatus task_status_;              // ����״̬
    bool is_task_active_;                 // �����Ƿ񼤻�
    cocos2d::Label* target_kills_label_;  // Ŀ���ɱ����ǩ
};

#endif // __ENEMYHUNT_H__
