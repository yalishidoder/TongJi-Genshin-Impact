#include "EnemyHunt.h"
#include "Character/Hero/Hero.h"
#include "Character/Enemy/Enemy.h"
#include "SimpleAudioEngine.h"


extern bool isTask3Completed ;
// ���캯��
EnemyHunt::EnemyHunt()
    : visible_size_(cocos2d::Size::ZERO),  // ���������С��ʼ��Ϊ��
    map_(nullptr),                      // ��ͼ��ʼ��Ϊ��
    hero_(nullptr),                     // ��ɫ��ʼ��Ϊ��
    target_kills_(0),                   // Ŀ���ɱ����ʼ��Ϊ0
    current_kills_(0),                  // ��ǰ��ɱ����ʼ��Ϊ0
    task_status_(TASK_IN_PROGRESS),     // ��ʼ����״̬Ϊ������
    is_task_active_(false),             // �����ʼ��Ϊδ����
    target_kills_label_(nullptr) {      // Ŀ���ɱ����ǩ��ʼ��Ϊ��
}

// ��������
EnemyHunt::~EnemyHunt()
{
    CleanupUI();  // ����UI��Դ
}

// ��ʼ������
bool EnemyHunt::init(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const std::vector<Enemy*>& enemies,
    int target_kills)
{
    if (!Node::init())
    {
        return false;  // ��������ʼ��ʧ�ܣ��򷵻�false
    }

    // ��ʼ����Ա����
    visible_size_ = visible_size;  // ���ÿ��������С
    map_ = map;                    // ���õ�ͼ
    hero_ = hero;                  // ���ý�ɫ
    enemies_ = enemies;            // ���õ����б�
    target_kills_ = target_kills;  // ����Ŀ���ɱ��

    return true;  // ��ʼ���ɹ�������true
}

// ��������������һ����ʼ���õ� EnemyHunt ����
EnemyHunt* EnemyHunt::create(
    const cocos2d::Size& visible_size,
    cocos2d::TMXTiledMap* map,
    Hero* hero,
    const std::vector<Enemy*>& enemies,
    int target_kills)
{
    EnemyHunt* ret = new EnemyHunt();  // ����һ���µ� EnemyHunt ����
    if (ret && ret->init(visible_size, map, hero, enemies, target_kills))
    {
        ret->autorelease();  // �Զ��ͷ��ڴ�
        return ret;  // ���س�ʼ���ɹ��Ķ���
    }
    else
    {
        delete ret;  // �����ʼ��ʧ�ܣ�ɾ������
        return nullptr;  // ���ؿ�ָ��
    }
}

// ��������
void EnemyHunt::StartTask()
{
    current_kills_ = 0;         // ��ʼ����ǰ��ɱ��Ϊ0
    is_task_active_ = true;     // ��������Ϊ����״̬
    task_status_ = TASK_IN_PROGRESS;  // ��������״̬Ϊ������

    // ����Ŀ���ɱ����ǩ
    target_kills_label_ = cocos2d::Label::createWithTTF("Target Kills: " + std::to_string(target_kills_), "fonts/Marker Felt.ttf", 24);
    target_kills_label_->setPosition(cocos2d::Vec2(visible_size_.width - 100, visible_size_.height - 50));  // ����λ��
    target_kills_label_->setTextColor(cocos2d::Color4B::RED);  // �����ı���ɫΪ��ɫ
    this->addChild(target_kills_label_);  // ��Ŀ���ɱ����ǩ��ӵ�������

    // ע�ᶨʱ���ص�����
    cocos2d::Director::getInstance()->getScheduler()->schedule(
        [this](float delta) { Update(); },  // ÿ0.1�����һ��Update����
        this,
        0.1f,
        false,
        "enemy");  // ��ʱ��IDΪ"enemy"
}

// ��������״̬
void EnemyHunt::Update()
{
    if (!is_task_active_) return;  // �������δ���ֱ�ӷ���
    // ģ����ܵ���
    for (auto enemy : enemies_)
    {
        if (!enemy->isAlive() && enemy->task_alive == 1)
        {
            enemy->task_alive = 0;
            DefeatEnemy(enemy);  // ÿ�ε��� DefeatEnemy ����ʱ����������һ����ɱ������ʾ��Ϣ
        }
    }
    // �ж��Ƿ�ﵽĿ���ɱ��
    if (current_kills_ >= target_kills_)
    {
        CompleteTask();  // �����ɱ���ﵽĿ�꣬�������
    }
}

// ���ܵ���
void EnemyHunt::DefeatEnemy(Enemy* enemy)
{
    if (!is_task_active_ || !enemy) return;  // �������δ��������Ϊ�գ�ֱ�ӷ���

    current_kills_++;  // ���ӻ�ɱ��

    // ��ʾ��ɱ�ɹ�����Ϣ
    auto label = cocos2d::Label::createWithTTF("Enemy Defeated!", "fonts/Marker Felt.ttf", 48);  // ������Ϣ��ǩ
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2 + 100);  // ���ñ�ǩλ�þ���
    label->setTextColor(cocos2d::Color4B::WHITE);  // �����ı���ɫΪ��ɫ
    this->addChild(label);  // ����ǩ��ӵ�������

    // ��ǩ��ʾ3����Ƴ�
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(1.0f),  // �ӳ�3��
        cocos2d::RemoveSelf::create(), nullptr));  // �Ƴ���ǩ
}

// �������
void EnemyHunt::CompleteTask()
{
    is_task_active_ = false;  // ��������Ϊδ����״̬
    task_status_ = TASK_COMPLETED;  // ��������״̬Ϊ���
    //���������Ч
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/task_finish.mp3");

    ShowMessage("Mission Success!");  // ��ʾ����ɹ���Ϣ
    CleanupUI();  // ����UI

    // ����3��ɣ�֮�������д����
    isTask3Completed = true;
    int target_level = hero_->getLevel() + 10;
    if (target_level >= 99) {
        hero_->setLevel(99);
    }
    else {
        hero_->setLevel(target_level);
    }
}

// ����ʧ��
void EnemyHunt::FailTask()
{
    is_task_active_ = false;  // ��������Ϊδ����״̬
    task_status_ = TASK_FAILED;  // ��������״̬Ϊʧ��
    ShowMessage("Mission Failed!");  // ��ʾ����ʧ����Ϣ
    CleanupUI();  // ����UI
}

// ��ʾ����״̬��Ϣ
void EnemyHunt::ShowMessage(const std::string& message)
{
    auto label = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 60);  // ������Ϣ��ǩ
    label->setPosition(visible_size_.width / 2, visible_size_.height / 2);  // ���ñ�ǩλ�þ���
    label->setTextColor(cocos2d::Color4B::BLACK);  // �����ı���ɫΪ��ɫ
    this->addChild(label);  // ����ǩ��ӵ�������

    // ��ǩ��ʾ3����Ƴ�
    label->runAction(cocos2d::Sequence::create(
        cocos2d::DelayTime::create(3.0f),
        cocos2d::RemoveSelf::create(), nullptr));  // �Ƴ���ǩ
}

EnemyHunt::TaskStatus EnemyHunt::GetTaskStatus() const
{
    return task_status_;  // ���ص�ǰ����״̬
}

// ����UI
void EnemyHunt::CleanupUI()
{
    // �Ƴ�Ŀ���ɱ����ǩ
    if (target_kills_label_)
    {
        target_kills_label_->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),
            cocos2d::CallFunc::create([this]()
                {
                    target_kills_label_->removeFromParent();
                    target_kills_label_ = nullptr;
                }),
            nullptr));
    }
}
