#include "OtherScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* OtherScene::createScene(const std::string& mapFile)
{
    OtherScene* ret = new OtherScene();
    if (ret && ret->init(mapFile)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainSceneScene.cpp\n");
}

bool OtherScene::init(const std::string& mapFile)
{
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    /////////////////////////////
    // ���һ���˳���ť    this->addChild(menu, 1);
    /////////////////////////////
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(OtherScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    /////////////////////
    //����С��ͼ
    ////////////////////
    std::string file = "Scene/othermap/" + mapFile;
    othermap = TMXTiledMap::create(file);

    if (!othermap) {
        CCLOG("Failed to load TMX map: %s", file.c_str());
        return false;
    }
    CCLOG("otherMap loaded successfully!");
    othermap->setScale(1.0f);

    othermap->setAnchorPoint(Vec2(0.5f, 0.5f));
    othermap->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    this->addChild(othermap);
    //////////////////////////
    // ��ͼ���½�����Ļ�ϵ�ʵ��λ��
    float othermapOriginX = othermap->getPositionX() - (othermap->getContentSize().width * othermap->getScale() * othermap->getAnchorPoint().x);
    float othermapOriginY = othermap->getPositionY() - (othermap->getContentSize().height * othermap->getScale() * othermap->getAnchorPoint().y);
    /////////////////////////

    ///////////////////////////
    // ���һ����ǩ 
    //////////////////////////
    auto label = Label::createWithTTF(mapFile, "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }
    //////////////////
    //���ؽ�ɫ
    //////////////////
    auto objectGroup = othermap->getObjectGroup("flag");
    if (objectGroup) {
        auto firstFlag = objectGroup->getObject("firstflag");
        if (!firstFlag.empty()) {
            float x = firstFlag["x"].asFloat();
            float y = firstFlag["y"].asFloat();
            CCLOG("Character spawn position: x = %.2f, y = %.2f", x, y);
            // ������ɫ�������ڳ���λ��
            auto hero = Character::create(Vec2(500, 500));
            if (hero) {
                hero->setName("hero"); // ���ý�ɫ����
                hero->setAnchorPoint(Vec2(0.5f, 0.5f));

                // ����ɫ���͵� firstflag ��λ��
                // �����firstflag����Ļ����
                float adjustedX = othermapOriginX + x * othermap->getScale(); // ��ͼ���½� + ������� x ƫ��
                float adjustedY = othermapOriginY + y * othermap->getScale(); // ��ͼ���½� + ������� y ƫ��

                hero->setPosition(Vec2(adjustedX, adjustedY));
                this->addChild(hero);  // ����ɫ��ӵ�������
            }
            else {
                CCLOG("Failed to load character");
            }
        }
        else {
            CCLOG("Failed to load object-firstflag");
        }
    }
    else {
        CCLOG("Failed to load objectGroup-flag");
    }

    // ��Ӽ����¼�������
    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(OtherScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(OtherScene::onKeyReleased, this);

    // ����������ӵ��¼��ַ���
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // ע�� update ����������ÿһ֡��������
    this->schedule([this](float dt) {
        this->update(dt);
        }, "update_key");

    return true;
}
void OtherScene::update(float dt)
{
    Node::update(dt);


    auto children = getChildren();
    for (auto child : children) {
        auto character = dynamic_cast<Character*>(child);
        if (character) {
            character->update(dt);
        }
    }
}

void OtherScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
    if (hero) {
        switch (keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_W:
            case cocos2d::EventKeyboard::KeyCode::KEY_W:
                hero->m_moveUp = true;
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_S:
            case cocos2d::EventKeyboard::KeyCode::KEY_S:
                hero->m_moveDown = true;
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_A:
            case cocos2d::EventKeyboard::KeyCode::KEY_A:
                hero->m_moveLeft = true;
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_D:
            case cocos2d::EventKeyboard::KeyCode::KEY_D:
                hero->m_moveRight = true;
                break;
            default:
                break;
        }
    }
}

void OtherScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
    if (hero) {
        switch (keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_W:
            case cocos2d::EventKeyboard::KeyCode::KEY_W:
                hero->m_moveUp = false;
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_S:
            case cocos2d::EventKeyboard::KeyCode::KEY_S:
                hero->m_moveDown = false;
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_A:
            case cocos2d::EventKeyboard::KeyCode::KEY_A:
                hero->m_moveLeft = false;
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_D:
            case cocos2d::EventKeyboard::KeyCode::KEY_D:
                hero->m_moveRight = false;
                break;
            default:
                break;
        }
    }
}

void OtherScene::returnToLastScene()
{
    // ʹ�� popScene ���ص�֮ǰ�ĳ���
    Director::getInstance()->popScene();
}

void OtherScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
