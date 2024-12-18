#include "Inventory.h"

// ����Inventoryʵ��
Inventory* Inventory::create()
{
    auto inventory = new (std::nothrow) Inventory();
    if (inventory && inventory->init()) {
        inventory->autorelease();
        return inventory;
    }
    CC_SAFE_DELETE(inventory);
    return nullptr;
}

// ��ʼ�������������Ʒ�б�
bool Inventory::init()
{
    m_items.clear();
    return true;
}

// �򱳰��������Ʒ
void Inventory::addItem(Item* item)
{
    m_items.push_back(item);
    // ע�⣺��ӵ���Ʒָ����Ҫȷ�����������ڹ�����ȷ
}

// �ӱ������Ƴ���Ʒ
void Inventory::removeItem(Item* item)
{
    auto iter = std::find(m_items.begin(), m_items.end(), item);
    if (iter != m_items.end()) {
        m_items.erase(iter);
    }
}

// �������ƻ�ȡ��Ʒ�����ص�һ��ƥ�����Ʒ
Item* Inventory::getItem(const std::string& name)
{
    for (auto item : m_items) {
        if (item->getName() == name) {
            return item;
        }
    }
    return nullptr;
}

// ��ȡ���е���Ʒ
const std::vector<Item*>& Inventory::getAllItem() const
{
    return m_items;
}

// װ����Ʒ��������Ʒ���ͽ��в�ͬ����
void Inventory::equipItem(Item* item)
{
    // �����ڿ��ǰ�װ������������ȡ����
    // ������Ʒ��װ���߼��ͷ��ڱ�����
}