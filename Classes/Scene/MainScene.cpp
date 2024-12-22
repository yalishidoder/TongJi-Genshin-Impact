#include "Scene/MainScene.h"
#include "Scene/OtherScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "Scene/MapManager.h"


USING_NS_CC;

// 定义全局变量，用于判断任务是否完成
//任务1是森林迷宫任务
bool isTask1Completed = false;
//任务2是沙漠寻宝任务
bool isTask2Completed = false;
//任务3是城镇杀敌任务
bool isTask3Completed = false;

Scene* MainScene::createScene()
{
    return MainScene::create();
}



// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
#if 1
    /////////////////////////////
    // 添加一个退出按钮    this->addChild(menu, 1);
    /////////////////////////////
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(MainScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'Scene/MainScene/CloseNormal.png' and 'Scene/MainScene/CloseSelected.png'");
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
#endif
    ////////////////////////////
    // 加载主地图    this->addChild(map,0);
    ////////////////////////////
    std::string file = "Scene/MainScene/mainmap.tmx";
    map = TMXTiledMap::create(file);

    if (!map) {
        CCLOG("Failed to load TMX map: %s", file.c_str());
        return false;
    }
    CCLOG("Map loaded successfully!");
    map->setScale(1.0f);
    map->setName("map");
    map->setAnchorPoint(Vec2(0.5f, 0.5f));
    map->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    this->addChild(map);
    //////////////////////////
    // 地图左下角在屏幕上的实际位置
    float mapOriginX = map->getPositionX() - (map->getContentSize().width * map->getScale() * map->getAnchorPoint().x);
    float mapOriginY = map->getPositionY() - (map->getContentSize().height * map->getScale() * map->getAnchorPoint().y);
    /////////////////////////

    ///////////////////////////
    // 添加一个标签"MainScene" map->addChild(label, 1);
    //////////////////////////
    auto label = Label::createWithTTF("MainScene", "fonts/Marker Felt.ttf", 24);
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

    EnemyManager::getInstance()->setSceneID("MainScene");

    //////////////////////////
    //加载角色 add 0
    //////////////////////////
    auto objectGroup_ch = map->getObjectGroup("chusheng");
    if (objectGroup_ch) {
        auto chushengObject = objectGroup_ch->getObject("initspot");
        if (!chushengObject.empty()) {
            float x = chushengObject["x"].asFloat();
            float y = chushengObject["y"].asFloat();
            CCLOG("Hero spawn position: x = %.2f, y = %.2f", x, y);
            // 创建角色并放置在出生位置

            auto hero = Hero::create(Vec2(500, 500));;
            if (hero) {
                hero->loadProfile("hero.txt");
                hero->setName("hero"); // 设置角色名称
                hero->setCharacterName("CaiXuKun");
                hero->setAnchorPoint(Vec2(0.5f, 0.15f));
                // 设置元素力
                hero->setElement(CharacterElement::ICE);

                // 计算出生点的屏幕坐标
                float adjustedX = mapOriginX + x * map->getScale(); // 地图左下角 + 出生点的 x 偏移
                float adjustedY = mapOriginY + y * map->getScale(); // 地图左下角 + 出生点的 y 偏移

                // 设置人物位置
                hero->setPosition(Vec2(adjustedX, adjustedY));
                hero->setSpawnPoint(Vec2(adjustedX, adjustedY)); 
                this->addChild(hero);  // 将角色添加到场景中 
                
                // 创建玩家面板
                m_playerPanel = PlayerPanel::create();
                if (m_playerPanel) {
                    m_playerPanel->setName("m_playerPanel");
                    m_playerPanel->setPosition(cocos2d::Vec2(250, 100));
                    m_playerPanel->setVisible(false);
                    m_playerPanel->setHero(hero);
                    m_playerPanel->initUi();
                    this->addChild(m_playerPanel, 11);
                }
            }

            // 创建血条背景
            auto healthBg = Sprite::create("Character/Hero/health_bg.png");
            if (healthBg) {
                healthBg->setName("healthBg"); // 设置名字
                healthBg->setPosition(Vec2(500, 50));
                healthBg->setOpacity(128);
                this->addChild(healthBg);
            }
            
            // 创建血条填充
            auto healthFill = Sprite::create("Character/Hero/health_fillg.png");
            if (healthFill) {
                healthFill->setName("healthFill"); // 设置名字
                healthFill->setAnchorPoint(Vec2(0, 0.5));
                healthFill->setPosition(Vec2(225,50));
                healthFill->setOpacity(128);
                this->addChild(healthFill);
            }

            // 创建等级Label
            auto levelLabel = Label::createWithTTF("Lv 1", "fonts/Marker Felt.ttf", 24);
            if (levelLabel) {
                levelLabel->setName("levelLabel"); // 设置名字
                levelLabel->setPosition(Vec2(500, 100));
                this->addChild(levelLabel);
            }
           
            //创建敌人
            auto demon = Enemy::create(Vec2(250, 300));

            if (demon) {
                demon->setName("demon"); // 设置角色名称
                demon->setAnchorPoint(Vec2(0.5f, 0.5f));
                demon->setPlayer(hero);  //设置玩家
                demon->setInitData(10); //根据敌人等级初始化数据 (别太大，会溢出)
                demon->setElement(CharacterElement::WATER);   // 初始化属性
                demon->setAttackMethods(Ranged_Enemy);         // 设置为远程
                // 计算出生点的屏幕坐标
                float adjustedX = mapOriginX + 250.0f; // 地图左下角 + 出生点的 x 偏移
                float adjustedY = mapOriginY + 300.0f; // 地图左下角 + 出生点的 y 偏移

                // 设置人物位置
                demon->setPosition(Vec2(adjustedX, adjustedY));
                demon->setSpawnPoint(Vec2(adjustedX, adjustedY));
                this->addChild(demon);  // 将角色添加到场景中

            }
            auto demon2 = Enemy::create(Vec2(250, 300));

            if (demon2) {
                demon2->setName("demon2"); // 设置角色名称
                demon2->setAnchorPoint(Vec2(0.5f, 0.5f));
                demon2->setPlayer(hero);  //设置玩家
                demon2->setInitData(10); //根据敌人等级初始化数据 (别太大，会溢出)
                demon2->setElement(CharacterElement::FIRE);   // 初始化属性
                demon2->setAttackMethods(Melee_Enemy);
                // 计算出生点的屏幕坐标
                float adjustedX = mapOriginX + 550.0f; // 地图左下角 + 出生点的 x 偏移
                float adjustedY = mapOriginY + 500.0f; // 地图左下角 + 出生点的 y 偏移

                // 设置人物位置
                demon2->setPosition(Vec2(adjustedX, adjustedY));
                demon2->setSpawnPoint(Vec2(adjustedX, adjustedY));
                this->addChild(demon2);  // 将角色添加到场景中

            }
        }
    }
    else {
        CCLOG("Failed to load chchchch");
    }
    // 读取主场景中的地图切换点位置
    auto objectGroup = map->getObjectGroup("SceneSwitchPoints");
    if (objectGroup) {
        for (auto& obj : objectGroup->getObjects()) {

            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX = mapOriginX + switchPos.x * map->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY = mapOriginY + switchPos.y * map->getScale(); // 地图左下角 + 出生点的 y 偏移
            switchPos.x = adjustedX;
            switchPos.y = adjustedY;
            //输出传送点位置
            CCLOG("Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);

            std::string targetMap = obj.asValueMap()["targetMap"].asString();
            // 输出传送点目的地调试信息
            CCLOG("targetMap: %s", targetMap.c_str());


            // 读取 no_position 属性并解析为 Vec2
            std::string noPositionStr = obj.asValueMap()["no_position"].asString();
            CCLOG("noPositionStr: %s", noPositionStr.c_str());
            Vec2 noPosition;
            std::istringstream noPosStream(noPositionStr);
            noPosStream >> noPosition.x >> noPosition.y; // 使用空格解析出两个坐标值
            CCLOG("before No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX_no = mapOriginX + noPosition.x * map->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY_no = mapOriginY + noPosition.y * map->getScale(); // 地图左下角 + 出生点的 y 偏移
            noPosition.x = adjustedX_no;
            noPosition.y = adjustedY_no;
            // 输出拒绝传送后的位置调试信息
            CCLOG("actual No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);
            // 存储到 sceneSwitchPoints
            sceneSwitchPoints.push_back({ switchPos, targetMap, noPosition });
        }
    }
    //** 读取场景中的位置传送点位置
    auto objectGroup_PositionSwitchPoints = map->getObjectGroup("PositionSwitchPoints");
    if (objectGroup_PositionSwitchPoints) {
        for (auto& obj : objectGroup_PositionSwitchPoints->getObjects()) {
            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX = mapOriginX + switchPos.x * map->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY = mapOriginY + switchPos.y * map->getScale(); // 地图左下角 + 出生点的 y 偏移
            switchPos.x = adjustedX;
            switchPos.y = adjustedY;
            //输出传送点位置
            CCLOG("PositionSwitchPoints: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            // 存储到 sceneSwitchPoints
            positionSwitchPoints.push_back({ switchPos });
        }
    }

    // 从 MapManager 加载存档的传送点状态
    auto savedSwitchPoints = MapManager::getInstance()->getMainSceneSwitchPoints();
    for (size_t i = 0; i < savedSwitchPoints.size(); ++i) {
        if (i < positionSwitchPoints.size()) {
            positionSwitchPoints[i].isActive = savedSwitchPoints[i].isActive;
        }
    }
    /////////////////////////////
    // 添加小地图
    /////////////////////////////
     miniMap = MiniMap::create(map, file, visibleSize, this);
    this->addChild(miniMap, 1);
    /////////////////////////////
    // 添加迷雾区域
    /////////////////////////////
    initFog();
    fogSandLayer = map->getLayer("WallLayer_desert");
    fogTownLayer = map->getLayer("WallLayer_town");


    // 添加键盘事件监听器
    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);

    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // 添加鼠标事件监听器
    auto mouseListener = cocos2d::EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(MainScene::onMouseDown, this);
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
    mouseListener->onMouseMove = CC_CALLBACK_1(Hero::onMouseMove, hero);
    
    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    
    // 注册 update 函数，让它每一帧都被调用
    this->schedule([this](float dt) {
        this->update(dt);
        }, "update_key");

    return true;
}



void MainScene::update(float dt)
{
    if ((isPopupVisible || isDialogActive || isPanelVisible)) {
        auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
        hero->m_moveUp = false;
        hero->m_moveDown = false;
        hero->m_moveLeft = false;
        hero->m_moveRight = false;
        return;
    }
    Node::update(dt);

    // 更新敌人管理器
    EnemyManager::getInstance()->update(dt);

    // 处理任务完成情况
    if (isTask1Completed == true)
    {
        completeTask1();
    }
    if (isTask2Completed == true)
    {
        completeTask2();
    }


    auto children = getChildren();
    for (auto child : children) {
        auto character = dynamic_cast<Hero*>(child);
        auto enemy = dynamic_cast<Enemy*>(child);
        if (character) {
            character->update(dt);
            if (character) {
                //更新角色相关的ui
                // 
                // 更新血条
                float healthRatio = character->CharacterBase::getHealth() / float(character->CharacterBase::getMaxHealth());
                auto healthFill = dynamic_cast<Sprite*>(this->getChildByName("healthFill"));
                if (healthFill)
                    healthFill->setScaleX(healthRatio);

                // 更新等级Label
                auto levelLabel = dynamic_cast<Label*>(this->getChildByName("levelLabel"));
                if (levelLabel)
                    levelLabel->setString(StringUtils::format("Lv %d", character->CharacterBase::getLevel()));
                // 输出当前角色等级
                //CCLOG("hero level : %d", character->CharacterBase::getLevel());
            }
        }
        if (enemy) {
            enemy->update(dt);
        }
    }

    //////////////////////
    //人物碰到地图切换点
    //////////////////////
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (hero) {
        for (auto& switchPoint : sceneSwitchPoints) {

            // 跳过未激活的传送点
            if (!switchPoint.isActive) continue;

            // 判断是否有弹窗正在显示&&判断地图传送点和人物是否碰撞
            if (!isDialogActive && hero->getBoundingBox().intersectsRect(Rect(switchPoint.position.x - 2.5, switchPoint.position.y - 2.5, 10, 10))) {
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

                // Yes 按钮的回调
                yesButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    EnemyManager::getInstance()->clearScene("MainScene");
                    // 播放点击音效
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/click_sl.mp3");
                    hero->saveProfile("hero.txt");//存储角色信息
                    CCLOG("User selected YES. Teleporting to %s.", switchPoint.targetMap.c_str());
                    CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPoint.position.x, switchPoint.position.y);
                    dialog->removeFromParent();
                    isDialogActive = false; // 恢复标志位
                    Director::getInstance()->pushScene(OtherScene::createScene(switchPoint.targetMap));
                    });

                // No 按钮的回调
                noButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    // 播放点击音效
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/click_sl.mp3");
                    CCLOG("User selected NO. Dialog removed.");
                    if (dialog) {
                        dialog->removeFromParent();
                        // 恢复标志位
                        isDialogActive = false;
                    }

                    // 将角色移出传送点范围
                    //float offset = 50.0f;
                    hero->setPosition(switchPoint.no_position.x, switchPoint.no_position.y);
                    CCLOG("Hero moved out of teleport area to position (%.2f, %.2f)", hero->getPositionX(), hero->getPositionY());

                    });
            }
        }
    }
    ////////////////////
    //人物碰到位置传送点
    ////////////////////
    auto player = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (player) {
        // 设置触发范围半径
        float triggerRadius = 50.0f;
        Vec2 currentPosition = player->getPosition();  // 获取角色当前位置
        bool isInRange = false;
        // 检测角色是否进入任何一个spot的范围
        for (auto& spot : positionSwitchPoints) {
            if (currentPosition.distance(spot.position) < triggerRadius) {
                isInRange = true;

                // 如果传送点未解锁，解锁该传送点
                if (!spot.isActive) {
                    spot.isActive = true;  // 解锁
                    // 更新 MapManager 中的传送点状态
                    MapManager::getInstance()->saveMainSceneSwitchPoints(positionSwitchPoints);
                }

                break;
            }
        }
        // 如果角色进入范围并按下了 E 键
        if (isInRange && isKeyPressedE) {
            if (!isPopupVisible) {
                // 如果弹窗未显示，则显示弹窗
                showSelectionPopup();
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");
               
                isPopupVisible = true;  // 设置弹窗为可见
            }
        }
        else if (isPopupVisible && !isKeyPressedE) {
            // 如果弹窗已显示且玩家没有按下E键，则继续监听E键关闭弹窗
            if (isKeyPressedE) {
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");
                hidePopup();  // 隐藏弹窗
                isPopupVisible = false;  // 设置弹窗为不可见
            }
        }
    }
    // 玩家面板的显示
    auto protagonist = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (protagonist) {
        if (isKeyPressedP) {
            operatemyPanel();
        }
    }
}

void MainScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (hero&& !(isPopupVisible || isDialogActive)) {
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
            case cocos2d::EventKeyboard::KeyCode::KEY_1:
                hero->ChangeToBayonet();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_2:
                hero->ChangeToBullet();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_Z:
            case cocos2d::EventKeyboard::KeyCode::KEY_Z:
                hero->SkillZ();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_X:
            case cocos2d::EventKeyboard::KeyCode::KEY_X:
                hero->SkillX();
                break;
            case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_C:
            case cocos2d::EventKeyboard::KeyCode::KEY_C:
                hero->SkillC();
                break;
            default:
                break;
        }
    }
    switch (keyCode) {
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_E:
        case cocos2d::EventKeyboard::KeyCode::KEY_E:
            isKeyPressedE = true;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_P:
        case cocos2d::EventKeyboard::KeyCode::KEY_P:
            isKeyPressedP = true;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_B:
        case cocos2d::EventKeyboard::KeyCode::KEY_B:
            isKeyPressedB = true;
            break;
    }
}

void MainScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
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
    switch (keyCode) {
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_E:
        case cocos2d::EventKeyboard::KeyCode::KEY_E:
            isKeyPressedE = false;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_P:
        case cocos2d::EventKeyboard::KeyCode::KEY_P:
            isKeyPressedP = false;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_B:
        case cocos2d::EventKeyboard::KeyCode::KEY_B:
            isKeyPressedB = false;
    }
}

void MainScene::onMouseDown(cocos2d::EventMouse* event)
{
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (isPopupVisible || isDialogActive)
        return;
    if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT) {
        auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
        if (hero && !(hero->m_isBulletChosen)&&hero->m_isBulletGet)
            hero->ChangeToBullet();
        if (hero && hero->m_isBulletChosen) {
            hero->attackWithBullet(TranslatePos(mouseEvent->getLocation()));
        }
    }
    else if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
        if (hero && !(hero->m_isBayonetChosen))
            hero->ChangeToBayonet();
        if (hero && hero->m_isBayonetChosen) {
            hero->attackWithBayonet();
        }
    }
}

cocos2d::Vec2 MainScene::TranslatePos(cocos2d::Vec2 origin) {
    cocos2d::Vec2 res;
    //auto map = dynamic_cast<TMXTiledMap*>(this->getChildByName("map"));
    float mapOriginX = map->getPositionX() - (map->getContentSize().width * map->getScale() * map->getAnchorPoint().x);
    float mapOriginY = map->getPositionY() - (map->getContentSize().height * map->getScale() * map->getAnchorPoint().y);
    res.x =  - mapOriginX + origin.x ; // 地图左下角 x 偏移修正
    res.y = -origin.y + 768;           // 地图左下角 y 偏移修正
    return res;
}


void MainScene::showSelectionPopup()
{
    auto player = dynamic_cast<Hero*>(this->getChildByName("hero"));
    auto popupLayer = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150)); // 半透明黑色背景
    this->addChild(popupLayer, 10);  // 将图层添加到场景，并设置显示优先级
    // 遍历所有spot，生成按钮
    for (const auto& spot : positionSwitchPoints) {
        // 只有isActive为true时，才显示按钮
        if (spot.isActive) {
            // 创建按钮，使用图片作为按钮的背景
            auto button = cocos2d::MenuItemImage::create(
                "Transfer_switch/Transfer_normal.png",  // 普通状态的按钮图片
                "Transfer_switch/Transfer_selected.png",  // 按下状态的按钮图片
                [this, spot, popupLayer, player](cocos2d::Ref* sender) {  // 捕获 player
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");
                    player->setPosition(spot.position);  // 传送角色
                    popupLayer->removeFromParent();  // 隐藏弹窗
                    isPopupVisible = false;  // 设置弹窗为不可见
                }
            );

            // 设置按钮位置
            button->setPosition(spot.position);  // 将按钮位置设置为传送点的位置

            // 创建菜单并将按钮添加到菜单
            auto menu = cocos2d::Menu::create(button, nullptr);
            menu->setPosition(cocos2d::Vec2::ZERO);  // 将菜单的原点设置为(0, 0)
            popupLayer->addChild(menu);  // 将菜单添加到popupLayer
        }
    }

    // 显示弹窗
    isPopupVisible = true;

}

bool MainScene::checkCollision(cocos2d::Vec2 position)
{

    // 获取瓦片层（假设墙壁层的名字为 "WallLayer"）
    TMXLayer* wallLayer = map->getLayer("WallLayer");

    // 获取目标位置的瓦片ID
    Vec2 tileCoord = tileCoordForPosition(position);
    int tileGID = wallLayer->getTileGIDAt(tileCoord);


    // 如果瓦片ID大于0，表示该位置有墙壁
    if (tileGID != 0 )
    {
        return true;  // 碰到墙壁
    }

    // 检查 fog_sand 层
    if (fogSandLayer && !isTask1Completed)
    {
        tileGID = fogSandLayer->getTileGIDAt(tileCoord);
        if (tileGID != 0) 
        {
            return true; // 碰到 fog_sand
        }
    }

    // 检查 fog_town 层
    if (fogTownLayer && !isTask2Completed) 
    {
        tileGID = fogTownLayer->getTileGIDAt(tileCoord);
        if (tileGID != 0) 
        {
            return true; // 碰到 fog_town
        }
    }
    return false;  // 没有碰到墙壁

}
cocos2d::Vec2 MainScene::tileCoordForPosition(Vec2 position)
{
    float x = position.x;
    float y = position.y;

    // 假设每个瓦片的大小是 16x16
    int tileWidth = 16;  // 每个瓦片的宽度
    int tileHeight = 16; // 每个瓦片的高度

    // 计算瓦片坐标
    int tileX = (int)(x / tileWidth);
    int tileY = (int)((map->getMapSize().height * tileHeight - y) / tileHeight);

    // 输出瓦片坐标
    CCLOG("Converting position (%.2f, %.2f) to tile coordinates: (%.2f, %.2f)", x, y, tileX, tileY);

    return Vec2(tileX, tileY);
}
void MainScene::hidePopup()
{
    // 隐藏弹窗
    this->removeChildByName("popupLayer");
    isPopupVisible = false;

}

void MainScene::showmyPanel()
{
    auto PanelLayer = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150)); // 半透明黑色背景
    PanelLayer->setName("PanelLayer");
    this->addChild(PanelLayer, 10);
}

void MainScene::hidemyPanel()
{
    this->removeChildByName("PanelLayer");
    m_playerPanel->setVisible(!m_playerPanel->isVisible());
    isPanelVisible = (m_playerPanel->isVisible());
}

void MainScene::operatemyPanel()
{
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (hero) {
        if (m_playerPanel) {
            m_playerPanel->setHero(hero);
            //m_playerPanel->initUi();
            m_playerPanel->updateInfo();
            m_playerPanel->setVisible(!m_playerPanel->isVisible());
            isPanelVisible = (m_playerPanel->isVisible());
            CCLOG("Panel visibility set to: %d", m_playerPanel->isVisible());

            showmyPanel();

            // 创建关闭按钮
            auto closeButton_ = cocos2d::ui::Button::create("Character/panel/Close_Icon.png");
            if (closeButton_) {
                closeButton_->setPosition(Vec2(550, 550));
                closeButton_->addClickEventListener([=](Ref* sender)
                    {
                        this->hidemyPanel();
                    });
                m_playerPanel->addChild(closeButton_);
            }
        }
        else
            CCLOG("m_playerPanel is null!");
    }
}


// 添加两个迷雾层，分别覆盖右半部分的上下两层，并且使用自定义的 PNG 图片作为阴影
void MainScene::initFog()
{
    // 获取地图的右半部分的尺寸和位置
    Size mapSize = map->getContentSize();
    float mapOriginX = map->getPositionX() - (map->getContentSize().width * map->getScale() * map->getAnchorPoint().x);
    float mapOriginY = map->getPositionY() - (map->getContentSize().height * map->getScale() * map->getAnchorPoint().y);

    // 上层迷雾区域（使用PNG图片作为阴影）
    fogUpperLayer = Layer::create();
    fogUpperLayer->setContentSize(Size(mapSize.width / 2, mapSize.height / 2)); // 使图片稍微变窄
    fogUpperLayer->setPosition(mapOriginX + mapSize.width / 2, mapOriginY + mapSize.height / 2);
    this->addChild(fogUpperLayer, 2);  // 添加到场景，设置zOrder

    // 下层迷雾区域（同样使用PNG图片作为阴影）
    fogLowerLayer = Layer::create();
    fogLowerLayer->setContentSize(Size(mapSize.width / 2, mapSize.height / 2));
    fogLowerLayer->setPosition(mapOriginX + mapSize.width / 2, mapOriginY);
    this->addChild(fogLowerLayer, 2);  // 添加到场景，设置zOrder

    // 创建精灵节点来显示PNG图片
    auto upperFogSprite = Sprite::create("Scene/MainScene/fogUpper.png");  // 上层迷雾的图片
    fogUpperLayer->addChild(upperFogSprite);
    upperFogSprite->setAnchorPoint(Vec2(0.1f, 0.2f)); // 设置锚点为中心，增大x向左移动，增大y向下移动
    upperFogSprite->setOpacity(255); // 设置透明度，50%透明度
    upperFogSprite->setContentSize(fogUpperLayer->getContentSize()); // 设置图片大小与迷雾层相同
    upperFogSprite->setScale(1.5f); // 让迷雾图片变大，调整比例根据需要


    auto lowerFogSprite = Sprite::create("Scene/MainScene/fogLower.png");  // 下层迷雾的图片
    fogLowerLayer->addChild(lowerFogSprite);
    lowerFogSprite->setAnchorPoint(Vec2(0.05f, 0.25f)); // 设置锚点为中心，增大x向右移动，增大y向下移动
    lowerFogSprite->setOpacity(255); // 设置透明度，50%透明度
    lowerFogSprite->setContentSize(fogLowerLayer->getContentSize()); // 设置图片大小与迷雾层相同
    lowerFogSprite->setScale(1.8f); // 让迷雾图片变大，调整比例根据需要

    // 设置迷雾初始为可见
    fogUpperLayer->setVisible(true);
    fogLowerLayer->setVisible(true);
}


// 完成任务1，隐藏 fog_sand 图层，隐藏上层迷雾
void MainScene::completeTask1()
{
    if (fogSandLayer)
    {
        fogSandLayer->setVisible(false); // 隐藏 fog_sand 图层
        fogUpperLayer->setVisible(false);// 隐藏上层迷雾
    }
}

// 完成任务2，隐藏 fog_town 图层，隐藏下层迷雾
void MainScene::completeTask2()
{
    if (fogTownLayer)
    {
        fogTownLayer->setVisible(false); // 隐藏 fog_town 图层
        fogLowerLayer->setVisible(false);// 隐藏下层迷雾
    }
}


void MainScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (hero)
        hero->saveProfile("hero.txt");
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
