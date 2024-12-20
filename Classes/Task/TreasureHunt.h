/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     TreasureHunt.h
 * File Function: ʵ��ɳĮ�е�Ѱ��������Ķ��塣
 * Author:        Ҷ��ΰ
 * Update Date:   2024.12.19
 ****************************************************************/

#ifndef TREASURE_HUNT_H_
#define TREASURE_HUNT_H_

#include "cocos2d.h"
#include "Character/Hero/Hero.h"

class TreasureHunt : public cocos2d::Node 
{
public:
    // ����״̬ö��
    enum TaskStatus
    {
        TASK_IN_PROGRESS = 0,  // ���������
        TASK_COMPLETED = 1,    // �������
        TASK_FAILED = 2        // ����ʧ��
    };

    // ���캯��
    TreasureHunt();
    // ��������
    ~TreasureHunt();

    // ��ʼ����������������������Ϣ
    bool init(
        const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const cocos2d::Vec2& start_pos,
        const cocos2d::Vec2& end_pos,
        int target_score,
        float time_limit);

    // ��������������һ����ʼ���õ� TreasureHunt ����
    static TreasureHunt* create(
        const cocos2d::Size& visible_size,
        cocos2d::TMXTiledMap* map,
        Hero* hero,
        const cocos2d::Vec2& start_pos,
        const cocos2d::Vec2& end_pos,
        int target_score,
        float time_limit);

    // ����Ŀ��÷�
    void SetTargetScore(int target_score);

    // ����ʱ������
    void SetTimeLimit(float time_limit);

    // ��������
    void StartTask();

    // ��ȡ��ǰ����״̬
    TaskStatus GetTaskStatus() const;

    // ת�����굽��Ƭ����
    Vec2 tileCoordForPosition(cocos2d::Vec2 position);

private:
    // ��������״̬
    void Update(float delta);

    // ���¼�ʱ��UI
    void UpdateTimerLabel();

    // �ɼ���ʯ
    void CollectGem(const cocos2d::Vec2& position);

    // �������
    void CompleteTask();

    // ����ʧ��
    void FailTask();

    // ��ʾ����״̬��Ϣ
    void ShowMessage(const std::string& message);

    // ����UI
    void CleanupUI();

    // �жϽ�ɫ�Ƿ��ڡ�gem��ͼ�����
    bool IsHeroOnGemLayer() ;

    // ���ݳ�Ա
    //  
    // ��Ļ���������С
    cocos2d::Size visible_size_;     
    // ��ͼ����
    cocos2d::TMXTiledMap* map_;    
    // ��ɫ����
    Hero* hero_;   
    // ����ʼλ��
    cocos2d::Vec2 start_pos_;    
    // �������λ��
    cocos2d::Vec2 end_pos_; 
    // ����Ŀ��÷�
    int target_score_;      
    // ��ǰ�÷�
    int current_score_;    
    // ����ʱ������
    float time_limit_;     
    // �Ѿ���ȥ��ʱ��
    float elapsed_time_;  
    // ��ǰ����״̬
    TaskStatus task_status_;  
    // �����Ƿ񼤻�
    bool is_task_active_;     
    // ��ʱ����ǩ
    cocos2d::Label* timer_label_;   
    // Ŀ��÷ֱ�ǩ
    cocos2d::Label* target_score_label_; 

    // Ŀ��÷ֱ�ǩ
    cocos2d::Label* time_limit_label_;
};

#endif  // TREASURE_HUNT_H_
