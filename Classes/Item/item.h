#ifndef __ITEM_H__
#define __ITEM_H__

#include "cocos2d.h"

// ������Ʒ����ö��
enum class ItemType
{
    MATERIAL,   // ����
    ARMOR,      // ����
    CONSUMABLE, // ����Ʒ
    EMPTY       // ��
    // �������
};

class Item : public cocos2d::Ref
{
public:
    // ����Itemʵ��
    static Item* create(const std::string& name = "empty", const std::string& description = "empty", ItemType type = ItemType::EMPTY);

    // ��ʼ��������������Ʒ�����ơ�����������
    virtual bool init(const std::string& name, const std::string& description, ItemType type);

    // ���·���������ÿ֡������Ʒ״̬����ǰδʵ�־��幦�ܣ�
    virtual void update(float dt);

    // ��ȡ��Ʒ����
    std::string getName() const;

    // ��ȡ��Ʒ����
    std::string getDescription() const;

    // ��ȡ��Ʒ����
    ItemType getType() const;

protected:
    std::string m_name;        // ��Ʒ����
    std::string m_description; // ��Ʒ����
    ItemType m_type;           // ��Ʒ����
};

#endif // __ITEM_H__