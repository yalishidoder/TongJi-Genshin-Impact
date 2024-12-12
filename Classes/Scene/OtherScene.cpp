#include "OtherScene.h"
#include "Scene/MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"

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
    mapname = mapFile;
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
    auto objectGroup_flag = othermap->getObjectGroup("flag");
    if (objectGroup_flag) {
        auto firstFlag = objectGroup_flag->getObject("firstflag");
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
    // ��ȡ�����еĵ�ͼ���͵�λ��
    auto objectGroup_SceneSwitchPoints = othermap->getObjectGroup("SceneSwitchPoints");
    if (objectGroup_SceneSwitchPoints) {
        for (auto& obj : objectGroup_SceneSwitchPoints->getObjects()) {
            
            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            CCLOG("before Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            //���ڵ�ͼ��λ��ԭ��ĸı䣬��Ҫ����tmx��������������е���
            float adjustedX_sw = othermapOriginX + switchPos.x * othermap->getScale(); // ��ͼ���½� + ������� x ƫ��
            float adjustedY_sw = othermapOriginY + switchPos.y * othermap->getScale(); // ��ͼ���½� + ������� y ƫ��
            switchPos.x = adjustedX_sw;
            switchPos.y = adjustedY_sw;
            //������͵�λ��
            CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            
            // ��ȡĿ���ͼ����
            std::string targetMap = obj.asValueMap()["targetMap"].asString();
            // ������͵�Ŀ�ĵص�����Ϣ
            CCLOG("targetMap: %s", targetMap.c_str());

            // ��ȡ no_position ���Բ�����Ϊ Vec2
            std::string noPositionStr = obj.asValueMap()["no_position"].asString();
            CCLOG("noPositionStr: %s", noPositionStr.c_str());
            Vec2 noPosition;
            std::istringstream noPosStream(noPositionStr);
            noPosStream >> noPosition.x >> noPosition.y; // ʹ�ÿո��������������ֵ
            CCLOG("before No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);
            //���ڵ�ͼ��λ��ԭ��ĸı䣬��Ҫ����tmx��������������е���
            float adjustedX_no = othermapOriginX + noPosition.x * othermap->getScale(); // ��ͼ���½� + ������� x ƫ��
            float adjustedY_no = othermapOriginY + noPosition.y * othermap->getScale(); // ��ͼ���½� + ������� y ƫ��
            noPosition.x = adjustedX_no;
            noPosition.y = adjustedY_no;
            // ����ܾ����ͺ��λ�õ�����Ϣ
            CCLOG("actual No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);

            // �洢�� sceneSwitchPoints
            sceneSwitchPoints.push_back({ switchPos, targetMap, noPosition });
        }
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
    //����ÿһ�����͵���Ϣ
    auto hero = dynamic_cast<Character*>(this->getChildByName("hero"));
    if (hero) {
        for (auto& switchPoint : sceneSwitchPoints) {
            //�жϵ�ͼ���͵�������Ƿ���ײ
         // ����δ����Ĵ��͵�
            if (!switchPoint.isActive) continue;

            // �ж��Ƿ��е���������ʾ
            if (!isDialogActive && hero->getBoundingBox().intersectsRect(Rect(switchPoint.position.x - 10, switchPoint.position.y - 10, 20, 20))) {
                isDialogActive = true;

                auto dialog = LayerColor::create(Color4B(0, 0, 0, 128));
                this->addChild(dialog, 10);

                auto label = Label::createWithSystemFont("Do you want to go to the " + switchPoint.targetMap, "Arial", 40);
                label->setPosition(Director::getInstance()->getVisibleSize() / 2 + Size(0, 50));
                dialog->addChild(label);

                auto yesButton = cocos2d::ui::Button::create();
                yesButton->setTitleText("Yes");
                yesButton->setTitleFontSize(30);
                yesButton->setPosition(Director::getInstance()->getVisibleSize() / 2 + Size(-50, -20));
                dialog->addChild(yesButton);

                auto noButton = cocos2d::ui::Button::create();
                noButton->setTitleText("No");
                noButton->setTitleFontSize(30);
                noButton->setPosition(Director::getInstance()->getVisibleSize() / 2 + Size(50, -20));
                dialog->addChild(noButton);

                // Yes ��ť�Ļص�
                yesButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    CCLOG("User selected YES. Teleporting to %s.", switchPoint.targetMap.c_str());
                    CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPoint.position.x, switchPoint.position.y);

                    dialog->removeFromParent();
                    isDialogActive = false; // �ָ���־λ
                    if (switchPoint.targetMap == "mainmap.tmx") {
                        Director::getInstance()->pushScene(MainScene::createScene());
                    }
                    });

                // No ��ť�Ļص�
                noButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    CCLOG("User selected NO. Dialog removed.");
                    if (dialog) {
                        dialog->removeFromParent();
                        // �ָ���־λ
                        isDialogActive = false;
                    }

                    // ����ɫ�Ƴ����͵㷶Χ
                    //float offset = 100.0f;
                    hero->setPosition(switchPoint.no_position.x , switchPoint.no_position.y);
                    CCLOG("Hero moved out of teleport area to position (%.2f, %.2f)", hero->getPositionX(), hero->getPositionY());

                    });
            }
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
