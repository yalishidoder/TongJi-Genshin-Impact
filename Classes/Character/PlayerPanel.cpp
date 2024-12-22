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
}

// ���ؽ�ɫ�����ڸ����Ա�
void PlayerPanel::changeHeroGender()
{
    if (m_hero)
    {
        m_hero->setGender(!(m_hero->getGender()));
        // �����Ա��޸�ģ��
        if (!m_hero->getGender()) {
            m_hero->setTexture("Character/Hero/Animation/female/female_default.png");
        }
        else {
            m_hero->setTexture("Character/Hero/Animation/male/male_default.png");
        }
        m_hero->resetAnimationCache();
        updateInfo();
    }
    else
        CCLOG("m_hero is null");
}

// ��ʼ��ui���
void PlayerPanel::initUi()
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

        // ������屳��
        auto backgroundSprite = Sprite::create("Character/panel/Character_Box.png");
        if (backgroundSprite) {
            backgroundSprite->setPosition(cocos2d::Vec2(250, 250));
            backgroundSprite->setScale(1.25f);
            this->addChild(backgroundSprite);
        }

        // ��������ͷ��
        auto headshotSprite = cocos2d::Sprite::create("Character/Hero/test.png");
        if (headshotSprite)
        {
            headshotSprite->setName("headshotSprite");
            headshotSprite->setPosition(cocos2d::Vec2(400, 400));
            this->addChild(headshotSprite);
        }

        // ����label����
        auto namebackgroundSprite = Sprite::create("Character/panel/Nickname_Slot.png");
        if (namebackgroundSprite) {
            namebackgroundSprite->setPosition(cocos2d::Vec2(110, 500));
            namebackgroundSprite->setScale(2.0f);
            this->addChild(namebackgroundSprite);
        }

        // ��ʾ��ɫ����
        auto heronameLabel = cocos2d::Label::createWithTTF("", "fonts/MedievalSharp.ttf", 48);
        if (heronameLabel) {
            heronameLabel->setName("heronameLabel");
            heronameLabel->setPosition(cocos2d::Vec2(110, 500));
            this->addChild(heronameLabel);
        }

        // ����label����
        auto levelbackgroundSprite = Sprite::create("Character/panel/Nickname_Slot.png");
        if (levelbackgroundSprite) {
            levelbackgroundSprite->setPosition(cocos2d::Vec2(110, 400));
            levelbackgroundSprite->setScale(2.0f);
            this->addChild(levelbackgroundSprite);
        }

        // ��ʾ��ɫ�ȼ�
        auto levelLabelinPanel = cocos2d::Label::createWithTTF("", "fonts/MedievalSharp.ttf", 48);
        if (levelLabelinPanel) {
            levelLabelinPanel->setName("levelLabelinPanel");
            levelLabelinPanel->setPosition(cocos2d::Vec2(110, 400));
            this->addChild(levelLabelinPanel);
        }

        // ��ʾ��ɫ�Ա�
        auto genderLabel = cocos2d::Label::createWithTTF("", "fonts/MedievalSharp.ttf", 35);
        if (genderLabel) {
            genderLabel->setName("genderLabel");
            genderLabel->setPosition(cocos2d::Vec2(110, 300));
            this->addChild(genderLabel);
        }

        // ��ʾ��ɫ�������ֵ
        auto fullHealthLabel = cocos2d::Label::createWithTTF("", "fonts/MedievalSharp.ttf", 35);
        if (fullHealthLabel) {
            fullHealthLabel->setName("fullHealthLabel");
            fullHealthLabel->setPosition(cocos2d::Vec2(110, 200));
            this->addChild(fullHealthLabel);
        }

        // ��ʾ��������Ϣ
        auto attackLabel = cocos2d::Label::createWithTTF("", "fonts/MedievalSharp.ttf", 35);
        if (attackLabel) {
            attackLabel->setName("attackLabel");
            attackLabel->setPosition(cocos2d::Vec2(110, 0));
            this->addChild(attackLabel);
        }

        // �����޸İ�ť
        auto changeButton = cocos2d::ui::Button::create("Character/panel/Change_icon.png");
        if (changeButton) {
            changeButton->setPosition(cocos2d::Vec2(300, 300));
            changeButton->addClickEventListener([=](Ref* sender)
                {
                    this->changeHeroGender();
                });
            this->addChild(changeButton);
        }
    }
    else {
        CCLOG("m_hero or m_inventory is null.");
    }
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

        // ��ʾ����ͷ��
        auto headshotSprite = dynamic_cast<Sprite*>(this->getChildByName("headshotSprite"));
        if (headshotSprite)
        {
            // �����Ա��л�ͷ��
            if (!m_hero->getGender()) {
                headshotSprite->setTexture("Character/Hero/Animation/female/female_default.png");
            }
            else {
                headshotSprite->setTexture("Character/Hero/Animation/male/male_default.png");
            }
            headshotSprite->setScale(4.0f);
        }

        // ��ʾ��ɫ��
        auto heronameLabel = dynamic_cast<Label*>(this->getChildByName("heronameLabel"));
        if (heronameLabel) {
            heronameLabel->setString(m_hero->CharacterBase::getCharacterName());
        }

        // ��ʾ��ɫ�ȼ�
        auto levelLabelinPanel = dynamic_cast<Label*>(this->getChildByName("levelLabelinPanel"));
        if (levelLabelinPanel) {
            levelLabelinPanel->setString(StringUtils::format("Lv %d", m_hero->CharacterBase::getLevel()));
        }

        // ��ʾ��ɫ�Ա�
        auto genderLabel = dynamic_cast<Label*>(this->getChildByName("genderLabel"));
        if (genderLabel) {
            std::string m_gender = (m_hero->getGender()) ? "Man" : "Woman";
            genderLabel->setString("Gender: " + m_gender);
        }

        // ��ʾ��ɫ�������ֵ
        auto fullHealthLabel = dynamic_cast<Label*>(this->getChildByName("fullHealthLabel"));
        if (fullHealthLabel) {
            fullHealthLabel->setString(StringUtils::format("MaxHealth: %d", m_hero->CharacterBase::getMaxHealth()));
        }

        // ��ʾ��������Ϣ
        auto attackLabel = dynamic_cast<Label*>(this->getChildByName("attackLabel"));
        if (attackLabel) {
            attackLabel->setString(StringUtils::format("AttackPower: %d", m_hero->CharacterBase::getAttackPower()));
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