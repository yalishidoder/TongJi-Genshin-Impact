#include "Item.h"

// ����Itemʵ��
Item* Item::create(const std::string& name, const std::string& description, ItemType type)
{
    auto item = new (std::nothrow) Item();
    if (item && item->init(name, description, type))
    {
        item->autorelease();
        return item;
    }
    CC_SAFE_DELETE(item);
    return nullptr;
}

// ��ʼ��������������Ʒ�����ơ�����������
bool Item::init(const std::string& name, const std::string& description, ItemType type)
{
    m_name = name;
    m_description = description;
    m_type = type;
    return true;
}

// ��ȡ��Ʒ����
std::string Item::getName() const
{
    return m_name;
}

// ��ȡ��Ʒ����
std::string Item::getDescription() const
{
    return m_description;
}

// ��ȡ��Ʒ����
ItemType Item::getType() const
{
    return m_type;
}

// ���·���������ÿ֡������Ʒ״̬����ǰδʵ�־��幦�ܣ�
void Item::update(float dt)
{
    // ʵ����Ʒ�ĸ����߼�
}