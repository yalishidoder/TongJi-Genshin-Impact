#include "PlayerPanel.h"
#include "Hero/Hero.h"
#include "Inventory/Inventory.h"

// ����PlayerPanelʵ���ľ�̬����
PlayerPanel* PlayerPanel::create() 
{
    auto panel = new (std::nothrow) PlayerPanel();
    if (panel && panel->init()) {
        panel->autorelease();
        return panel;
    }
    CC_SAFE_DELETE(panel);
    return nullptr;
}

// ��ʼ�����������ù�����ͼ�Ͳ���
bool PlayerPanel::init()
{
    if (!Node::init()) {
        return false;
    }

    // ������ֱ�����Ĺ�����ͼ
    auto scrollView = cocos2d::ui::ScrollView::create();
    if(scrollView)
    {
        scrollView->setDirection(cocos2d::ui::ScrollView::Direction::VERTICAL);
        scrollView->setPosition(cocos2d::Vec2(0, 0));
        scrollView->setContentSize(cocos2d::Size(200, 300));
        this->addChild(scrollView);
    }
    else {
        CCLOG("Failed to create scrollView");
        return false;
    }

    // ���������Ʒ�Ĳ���
    auto layout = cocos2d::ui::Layout::create();
    if(layout)
    {
        layout->setContentSize(cocos2d::Size(200, 0));
        scrollView->getInnerContainer()->addChild(layout);
    }
    else {
        CCLOG("Failed to create layout");
        return false;
    }

    m_inventoryLayout = layout;
    m_scrollView = scrollView;

    return true;
}

// ����Ӣ�۽�ɫ������ȡ�䱳��
void PlayerPanel::setHero(Hero* hero)
{
    m_hero = hero;
    if (m_hero) {
        m_inventory = hero->getInventory();
    }
    else {
        CCLOG("m_hero is null.");
    }

    updateInfo();
}

// ����UI��Ϣ����ʾ������Ʒ�ͽ�ɫ״̬
void PlayerPanel::updateInfo()
{
    if (m_hero && m_inventory) {
        CCLOG("m_hero and m_inventory are valid");
        m_inventoryLayout->removeAllChildren();

        // ���������е�������Ʒ�������ڵ㲢��ӵ�������
        auto items = m_inventory->getAllItem();

        CCLOG("Number of items: %d", items.size());
        for (auto item : items) {
            if (item != nullptr) {
                auto itemNode = createItemNode(item);
                m_inventoryLayout->addChild(itemNode);
            }
            else {
                CCLOG("Found null item in inventory.");
            }
        }

        // ���ò��ֵĴ�С
        float height = items.size() * 50;

        if (!items.size())
            height = 1000;

        m_inventoryLayout->setContentSize(cocos2d::Size(200, height));

        // ȷ��ScrollView�����������С��ȷ
        m_scrollView->setInnerContainerSize(cocos2d::Size(200, height));

        // ��������ͷ��
        auto headshotSprite = cocos2d::Sprite::create("Character/Hero/test.png");
        
        if (headshotSprite)
        {
            // �����Ա��л�ͷ��
            if (!m_hero->getGender()) {
                headshotSprite->setTexture("Character/Hero/Animation/female/female_default.png");
            }
            else {
                headshotSprite->setTexture("Character/Hero/Animation/male/male_default.png");
            }

            headshotSprite->setPosition(cocos2d::Vec2(300, 400));
            this->addChild(headshotSprite);

        }

        // ��ʾ��ɫ����
        auto nameLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
        if (nameLabel) {
            nameLabel->setString(m_hero->getCharacterName());
            nameLabel->setPosition(cocos2d::Vec2(100, 600));
            this->addChild(nameLabel);
        }

        // ��ʾ��ɫ�ȼ�
        auto levelLabelinPanel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
        if (levelLabelinPanel) {
            levelLabelinPanel->setString("LV :" + std::to_string(m_hero->getLevel()));
            levelLabelinPanel->setPosition(cocos2d::Vec2(100, 500));
            this->addChild(levelLabelinPanel);
        }

        // ��ʾ��ɫ�Ա�
        auto genderLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
        if (genderLabel) {
            std::string m_gender = (m_hero->getGender()) ? "Woman" : "Man";
            genderLabel->setString("Gender: " + m_gender);
            genderLabel->setPosition(cocos2d::Vec2(100, 400));
            this->addChild(genderLabel);
        }

        // ��ʾ��ɫ�������ֵ
        auto fullHealthLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
        if (fullHealthLabel) {
            fullHealthLabel->setString("MaxHealth: " + std::to_string(m_hero->getMaxHealth()));
            fullHealthLabel->setPosition(cocos2d::Vec2(100, 300));
            this->addChild(fullHealthLabel);
        }

        // ��ʾ��װ����������Ϣ
        auto equippedWeapon = m_hero->getEquippedWeapon();
        if (equippedWeapon) {
            auto weaponLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
            if (weaponLabel) {
                weaponLabel->setString("equippedWeapon: ");// + equippedWeapon->getName());
                weaponLabel->setPosition(cocos2d::Vec2(100, 200));
                this->addChild(weaponLabel);
            }
        }

        // ��ʾ��������Ϣ
        auto attackLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
        if (attackLabel) {
            attackLabel->setString("AttackPower: " + std::to_string(m_hero->getAttackPower()));
            attackLabel->setPosition(cocos2d::Vec2(100, 100));
            this->addChild(attackLabel);
        }
    }
    else {
        CCLOG("m_hero or m_inventory is null.");
    }
}

// ������ʾ��Ʒ�Ľڵ㣬����ͼ�ꡢ���ƺ�ʹ�ð�ť
cocos2d::Node* PlayerPanel::createItemNode(Item* item) 
{
    auto node = cocos2d::Node::create();
    if (node)
        node->setContentSize(cocos2d::Size(200, 50));
    else {
        //CCLOG("Failed to create node for item");
        return nullptr;
    }

    // ������Ʒͼ��
    auto iconSprite = cocos2d::Sprite::create("Character/Hero/test.png");
    if(iconSprite)
    {
        iconSprite->setPosition(cocos2d::Vec2(20, 25));
        node->addChild(iconSprite);
    }
    /*else
        CCLOG("Failed to create iconSprite for item");*/

    // ������Ʒ���Ʊ�ǩ
    auto nameLabel = cocos2d::Label::createWithTTF("", "fonts/Marker Felt.ttf", 48);
    if(nameLabel)
    {
        //nameLabel->setString(item->getName());
        nameLabel->setString("Nothing");
        nameLabel->setPosition(cocos2d::Vec2(60, 25));
        node->addChild(nameLabel);
    }
    else 
        CCLOG("Failed to create nameLabel for item");

    // ����ʹ����Ʒ�İ�ť
    auto button = cocos2d::ui::Button::create();
    if(button)
    {
        button->setPosition(cocos2d::Vec2(160, 25));
        button->addClickEventListener([=](cocos2d::Ref* sender) {
            useItem(item);
            });
        node->addChild(button);
    }
    /*else 
        CCLOG("Failed to create button for item");*/

    return node;
}

// ����ʹ����Ʒ���߼�
void PlayerPanel::useItem(Item* item) {
    if (item->getType() == ItemType::CONSUMABLE) {
        //ҩˮ��ʹ���߼�
       /* auto potion = dynamic_cast<Potion*>(item);
        if (potion) {
            m_hero->setHealth(m_hero->getHealth() + potion->getRestoreAmount());
            m_inventory->removeItem(potion);
            updateInfo();
        }*/
    }
    else if (item->getType() == ItemType::ARMOR) {
        m_inventory->equipItem(item);
        updateInfo();
    }
}