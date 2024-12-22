/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Maze.h
 * File Function: �����Թ������࣬ʵ��������������ɼ�⡣
 * Author:        Ҷ��ΰ
 * Last Updated:  2024-12-19
 ****************************************************************/

#ifndef MAZE_H_
#define MAZE_H_

#include <string>
#include "cocos2d.h"
#include "Character/Hero/Hero.h"


// Maze�࣬��ʾһ���Թ����񣬼̳���cocos2d::Node
class Maze : public cocos2d::Node 
{

public:
    // ����״̬ö��
    enum TaskStatus
    {
        TASK_IN_PROGRESS = 0,  // ���������
        TASK_COMPLETED = 1,    // �������
        TASK_FAILED = 2        // ����ʧ��
    };

    // ����Maze����ʹ�ù�������ģʽ
    static Maze* Create(const std::string& maze_name, const cocos2d::Vec2& start,
        const cocos2d::Vec2& end, float time_limit,
        const std::string& reward, Hero* hero);

    // ��ʼ�������������Թ�����Ļ�������
    bool Init(const std::string& maze_name, const cocos2d::Vec2& start,
        const cocos2d::Vec2& end, float time_limit,
        const std::string& reward, Hero* hero);

    // �����Թ�����
    void StartTask();

    // ��ȡ����״̬���Ƿ�ɹ�/ʧ�ܣ�
    bool GetTaskStatus() const;

private:
    // ˽�й��캯������������
    Maze();

    virtual ~Maze();

    // ���������߼�����ʱ�ͼ�����λ�ã�
    void UpdateTask(float delta);

    // �������Ƿ񵽴��յ�
    void CheckCompletion();

    // �������ʱ�Ĵ���
    void CompleteTask();

    // ����ʧ��ʱ�Ĵ���
    void FailTask();

    // ��ʾ����״̬��Ϣ���ɹ�/ʧ�ܣ�
    void ShowMessage(const std::string& message);

    // ����UI�ͼ�ʱ��
    void Cleanup();

    // �Թ����������
    std::string maze_name_;

    // �����յ�λ��
    cocos2d::Vec2 start_;
    cocos2d::Vec2 end_;

    // ʱ�����ƣ��룩
    float time_limit_;

    // ��ǰ����ʱ��
    float elapsed_time_;

    // ��������
    std::string reward_;

    // ����״̬
    bool task_status_=false;

    // �Ƿ�������������
    bool is_task_active_;

    // ��ʾ��ʱ�ı�ǩ
    cocos2d::Label* timer_label_;

    // Ŀ��÷ֱ�ǩ
    cocos2d::Label* time_limit_label_;

    // ��ҽ�ɫ�ڵ�
    cocos2d::Node* player_;
    Hero* hero_;             // ��ɫ

    // �ɼ������С��������ʾUI
    cocos2d::Size visible_size_;
};

#endif  // MAZE_H_