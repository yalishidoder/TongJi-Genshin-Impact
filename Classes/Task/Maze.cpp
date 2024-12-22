/****************************************************************
 * Project Name:  Genshin Impact
 * File Name:     Maze.cpp
 * File Function: �����Թ��������ʵ�֡�Ҫ��ʵ���Թ�����Ĵ��������������¡���ɡ�ʧ�ܡ�UI��ʾ�ȹ��ܡ�
 * Description:   �Թ���������������޶�ʱ���ڣ�ͨ���ƶ���ɫ�����յ㡣
 * Author:        Ҷ��ΰ
 * Last Updated:  2024-12-19
 ****************************************************************/


#include "Maze.h"

extern bool isTask1Completed;


 // ���캯��
Maze::Maze()
    : time_limit_(30.0f),         // ����Ĭ��ʱ������Ϊ30��
    elapsed_time_(0.0f),         // ��ʼ������ʱ��Ϊ0
    is_task_active_(false),      // �����ʼ״̬Ϊδ����
    timer_label_(nullptr),       // ��ʱ����ǩָ���ʼ��Ϊ��
    player_(nullptr),            // ��ҽڵ�ָ���ʼ��Ϊ��
    task_status_(false) {}

// ��������
 Maze::~Maze()
{
    Cleanup();  // �����ʱ����UIԪ��
}

// ������������������ʼ��Maze����
Maze* Maze::Create(const std::string& maze_name, const cocos2d::Vec2& start,
    const cocos2d::Vec2& end, float time_limit,
    const std::string& reward) 
{
    Maze* maze = new (std::nothrow) Maze();
    if (maze && maze->Init(maze_name, start, end, time_limit, reward)) 
    {
        maze->autorelease();  // �Զ��ͷ�
        return maze;
    }
    delete maze;  // ����ʧ��ʱɾ������
    return nullptr;
}

// ��ʼ�������������Թ�����Ļ�������
bool Maze::Init(const std::string& maze_name, const cocos2d::Vec2& start,
    const cocos2d::Vec2& end, float time_limit,
    const std::string& reward) 
{
    if (!Node::init()) 
    {  // ���Node��ʼ���Ƿ�ɹ�
        return false;
    }

    // �����Թ��Ļ�����Ϣ
    maze_name_ = maze_name;
    start_ = start;
    end_ = end;
    time_limit_ = time_limit;
    reward_ = reward;

    // ��ȡ��Ļ�ߴ�
    visible_size_ = cocos2d::Director::getInstance()->getVisibleSize();

    return true;
}

// �����Թ�����
void Maze::StartTask() 
{
    // ��������Ѿ������������
    if (is_task_active_) return;

    is_task_active_ = true;
    elapsed_time_ = 0.0f;  // ��������ʱ��
    task_status_ = false;

    // ��ȡ��ҽڵ�
    player_ = this->getParent()->getChildByName("hero");
    if (!player_)
    {
        cocos2d::log("Error: Cannot find player node.");
        return;
    }

    // ����ҽ�ɫ�ƶ����Թ������
    player_->setPosition(start_);

    // ������ʱ����ǩ
    timer_label_ = cocos2d::Label::createWithTTF("Time: 0.0", "fonts/Marker Felt.ttf", 24);
    timer_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 50));
    timer_label_->setTextColor(cocos2d::Color4B::RED);
    this->getParent()->addChild(timer_label_, 20);

    // �����޶�ʱ���ǩ
    time_limit_label_ = cocos2d::Label::createWithTTF("Limited Time: " + std::to_string(int(time_limit_)), "fonts/Marker Felt.ttf", 24);
    time_limit_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 100));  // ����λ��
    time_limit_label_->setTextColor(cocos2d::Color4B::RED);  // �����ı���ɫΪ��ɫ
    this->addChild(time_limit_label_);  // ���޶�ʱ���ǩ��ӵ�������

    // ���������߼�����
    this->schedule(CC_SCHEDULE_SELECTOR(Maze::UpdateTask), 0.1f);
}

// ��������߼�����ʱ�ͼ������������
void Maze::UpdateTask(float delta) 
{
    // �������δ�����ִ��
    if (!is_task_active_) return;

    elapsed_time_ += delta;  // �ۼ�����ʱ��
    timer_label_->setString(cocos2d::StringUtils::format("Time: %.1f", elapsed_time_));

    // �������ʱ�䳬�����ƣ�������ʧ��
    if (elapsed_time_ > time_limit_) 
    {
        FailTask();
    }
    else 
    {
        CheckCompletion();  // �������Ƿ񵽴��յ�
    }
}

// �������Ƿ񵽴��Թ����յ�
void Maze::CheckCompletion() 
{
    // �����ҽڵ㲻���ڣ��򲻽��м��
    if (!player_) return;

    // �ж���ҵ�ǰλ���Ƿ����յ����
    if (player_->getPosition().distance(end_) < 15.0f) 
    {  // ����һ�����
        CompleteTask();  // ��ҵ����յ㣬�������
    }
}

// ������ɴ���
void Maze::CompleteTask() 
{
    is_task_active_ = false;  // �������Ϊδ����
    task_status_ = true;  // ��������״̬Ϊ���
    this->unschedule(CC_SCHEDULE_SELECTOR(Maze::UpdateTask));  // ֹͣ�����ʱ

    ShowMessage("Mission Success!");  // ��ʾ����ɹ���Ϣ
    // ����һ����ʾ����״̬�ı�ǩ
    auto label = cocos2d::Label::createWithTTF("Congratulations on your award!", "fonts/Marker Felt.ttf", 60);
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2 - 100);  // ���ñ�ǩλ��
    label->setTextColor(cocos2d::Color4B::WHITE);  // ���ñ�ǩ������ɫΪ��ɫ
    this->getParent()->addChild(label);

    // �ӳ�3����Ƴ�����״̬��Ϣ��ǩ
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
        cocos2d::CallFunc::create([label]() { label->removeFromParent(); }),  // �Ƴ���ǩ
        nullptr));
    cocos2d::log("Maze task completed! Reward: %s", reward_.c_str());  // ���������Ϣ

    Cleanup();  // ����UIԪ��

    // �������󣬴�������1������¼���������
    isTask1Completed = true;
}

// ����ʧ�ܴ���
void Maze::FailTask()
{
    is_task_active_ = false;  // �������Ϊδ����
    task_status_ = false;  // ��������״̬Ϊʧ��

    this->unschedule(CC_SCHEDULE_SELECTOR(Maze::UpdateTask));  // ֹͣ�����ʱ

    ShowMessage("Mission Failed!");  // ��ʾ����ʧ����Ϣ
    Cleanup();  // ����UIԪ��
}

// ��ʾ����״̬��Ϣ���ɹ�/ʧ�ܣ�
void Maze::ShowMessage(const std::string& message) 
{
    // ����һ����ʾ����״̬�ı�ǩ
    auto label = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 60);
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2);  // ���ñ�ǩλ��
    label->setTextColor(cocos2d::Color4B::WHITE);  // ���ñ�ǩ������ɫΪ��ɫ
    this->getParent()->addChild(label);

    // �ӳ�3����Ƴ�����״̬��Ϣ��ǩ
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
        cocos2d::CallFunc::create([label]() { label->removeFromParent(); }),  // �Ƴ���ǩ
        nullptr));

}

// �����ʱ����UIԪ��
void Maze::Cleanup()
{
    // �Ƴ���ʱ����ǩ
    if (timer_label_) 
    {
        timer_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
            cocos2d::CallFunc::create([this]() 
                {
                timer_label_->removeFromParent();  // �ӳ������Ƴ���ʱ����ǩ
                timer_label_ = nullptr;            // ��ռ�ʱ����ǩָ��
                }),
            nullptr));
    }

    // �Ƴ�ʱ�����Ʊ�ǩ
    if (time_limit_label_)
    {
        time_limit_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),  // �ӳ�3��
            cocos2d::CallFunc::create([this]()
                {
                    time_limit_label_->removeFromParent();  // �ӳ������Ƴ�Ŀ��÷ֱ�ǩ
                    time_limit_label_ = nullptr;  // ���Ŀ��÷ֱ�ǩָ��
                }),
            nullptr));
    }
}


bool Maze::GetTaskStatus() const
{
    return task_status_;
};
