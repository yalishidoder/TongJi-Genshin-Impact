#include "OtherScene.h"
#include "Scene/MainScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "MiniMap.h"
#include "Scene/MapManager.h"
#include "Task/Maze.h"
#include "Task/TreasureHunt.h"
#include "Task/EnemyHunt.h"

USING_NS_CC;

extern bool isTask1Completed;
extern bool isTask2Completed ;

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
    // 添加一个退出按钮    this->addChild(menu, 1);
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
    //加载地图
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
    // 地图左下角在屏幕上的实际位置
    float othermapOriginX = othermap->getPositionX() - (othermap->getContentSize().width * othermap->getScale() * othermap->getAnchorPoint().x);
    float othermapOriginY = othermap->getPositionY() - (othermap->getContentSize().height * othermap->getScale() * othermap->getAnchorPoint().y);
    /////////////////////////

    ///////////////////////////
    // 添加一个标签 
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
    //加载角色
    //////////////////
    auto objectGroup_flag = othermap->getObjectGroup("flag");
    if (objectGroup_flag) {
        auto firstFlag = objectGroup_flag->getObject("firstflag");
        if (!firstFlag.empty()) {
            float x = firstFlag["x"].asFloat();
            float y = firstFlag["y"].asFloat();
            CCLOG("Character spawn position: x = %.2f, y = %.2f", x, y);
            // 创建角色并放置在出生位置
            auto hero = Hero::create(Vec2(500, 500));
            if (hero) {
                hero->loadProfile("hero.txt");
                hero->setName("hero"); // 设置角色名称
                hero->setCharacterName("CaiXuKun");
                hero->setAnchorPoint(Vec2(0.5f, 0.15f));
                // 设置元素力
                hero->setElement(CharacterElement::ICE);

                // 将角色传送到 firstflag 的位置
                // 计算出firstflag的屏幕坐标
                float adjustedX = othermapOriginX + x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
                float adjustedY = othermapOriginY + y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移

                hero->setPosition(Vec2(adjustedX, adjustedY));
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
                healthFill->setPosition(Vec2(500, 50));
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
            std::vector<Vec2>initposition;//创建的敌人位置
            if (mapname == "forest.tmx") {
                initposition = {

                };
            }
            if (mapname == "town.tmx") {
                initposition = {
                    {300,178},
                    {370,358},
                    {380,546},
                    {815,358},
                    {814,610},
                    {814,166}
                };
            }
            for (int i = 0; i < initposition.size(); i++) {
                auto demon = Enemy::create(initposition[i]);
                if (demon) {
                    demon->setName("demon"); // 设置角色名称
                    demon->setAnchorPoint(Vec2(0.5f, 0.5f));
                    demon->setPlayer(hero);  //设置玩家
                    demon->setPatrolRange(150.0f, 300.0f);   //设置巡逻范围
                    demon->setRadius(100.0f);
                    demon->setInitData(10); //根据敌人等级初始化数据 (别太大，会溢出)
                    demon->setElement(CharacterElement::WATER);   // 初始化属性
                    // 计算出生点的屏幕坐标
                    float adjustedX = othermapOriginX + initposition[i].x; // 地图左下角 + 出生点的 x 偏移
                    float adjustedY = othermapOriginX + initposition[i].y; // 地图左下角 + 出生点的 y 偏移

                    // 设置人物位置
                    demon->setPosition(Vec2(adjustedX, adjustedY));

                    this->addChild(demon);  // 将角色添加到场景中
                    // 将敌人对象添加到 enemies 向量中
                    enemies.push_back(demon);
                }
            }
        }
        else {
            CCLOG("Failed to load object-firstflag");
        }
    }
    else {
        CCLOG("Failed to load objectGroup-flag");
    }

    // 读取场景中的地图传送点位置
    auto objectGroup_SceneSwitchPoints = othermap->getObjectGroup("SceneSwitchPoints");
    if (objectGroup_SceneSwitchPoints) {
        for (auto& obj : objectGroup_SceneSwitchPoints->getObjects()) {
            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            CCLOG("before Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX_sw = othermapOriginX + switchPos.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY_sw = othermapOriginY + switchPos.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
            switchPos.x = adjustedX_sw;
            switchPos.y = adjustedY_sw;
            //输出传送点位置
            CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            
            // 读取目标地图名称
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
            float adjustedX_no = othermapOriginX + noPosition.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY_no = othermapOriginY + noPosition.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
            noPosition.x = adjustedX_no;
            noPosition.y = adjustedY_no;
            // 输出拒绝传送后的位置调试信息
            CCLOG("actual No Position: x = %.2f, y = %.2f", noPosition.x, noPosition.y);

            // 存储到 sceneSwitchPoints
            sceneSwitchPoints.push_back({ switchPos, targetMap, noPosition });
        }
    }
    //** 读取场景中的位置传送点位置
    auto objectGroup_PositionSwitchPoints = othermap->getObjectGroup("PositionSwitchPoints");
    if (objectGroup_PositionSwitchPoints) {
        for (auto& obj : objectGroup_PositionSwitchPoints->getObjects()) {
            Vec2 switchPos(obj.asValueMap()["x"].asFloat(), obj.asValueMap()["y"].asFloat());
            //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
            float adjustedX = othermapOriginX + switchPos.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
            float adjustedY = othermapOriginY + switchPos.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
            switchPos.x = adjustedX;
            switchPos.y = adjustedY;
            //输出传送点位置
            CCLOG("PositionSwitchPoints: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
            // 存储到 sceneSwitchPoints
            positionSwitchPoints.push_back({ switchPos });
        }
    }
    // 从 MapManager 加载存档的传送点状态
    if (mapname == "forest.tmx") {
        auto savedSwitchPoints = MapManager::getInstance()->getforestSwitchPoints();
        for (size_t i = 0; i < savedSwitchPoints.size(); ++i) {
            if (i < positionSwitchPoints.size()) {
                positionSwitchPoints[i].isActive = savedSwitchPoints[i].isActive;
            }
        }
        positionSwitchPoints[0].isActive = true;
    }
    else if (mapname == "desert.tmx") {
        auto savedSwitchPoints = MapManager::getInstance()->getdesertSwitchPoints();
        for (size_t i = 0; i < savedSwitchPoints.size(); ++i) {
            if (i < positionSwitchPoints.size()) {
                positionSwitchPoints[i].isActive = savedSwitchPoints[i].isActive;
            }
        }
    }
    else if (mapname == "town.tmx") {
        auto savedSwitchPoints = MapManager::getInstance()->gettownSwitchPoints();
        for (size_t i = 0; i < savedSwitchPoints.size(); ++i) {
            if (i < positionSwitchPoints.size()) {
                positionSwitchPoints[i].isActive = savedSwitchPoints[i].isActive;
            }
        }

    }
    //任务点读取
    auto objectGroup_taskStartPosition = othermap->getObjectGroup("task_start");
    if (objectGroup_taskStartPosition) {
        ValueMap taskStartData = objectGroup_taskStartPosition->getObject("task_start");
        Vec2 switchPos(taskStartData["x"].asFloat(), taskStartData["y"].asFloat());
        //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
        float adjustedX = othermapOriginX + switchPos.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
        float adjustedY = othermapOriginY + switchPos.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
        switchPos.x = adjustedX;
        switchPos.y = adjustedY;
        //输出任务领取点位置
        CCLOG("PositionSwitchPoints: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
        // 存储到 taskStartPosition
        taskStartPosition = switchPos;
    }
    //任务结束点读取
    auto objectGroup_taskEndPosition = othermap->getObjectGroup("task_end");
    if (objectGroup_taskEndPosition) {
        ValueMap taskStartData = objectGroup_taskEndPosition->getObject("task_end");
        Vec2 switchPos(taskStartData["x"].asFloat(), taskStartData["y"].asFloat());
        //由于地图的位置原点的改变，需要将从tmx读出来的坐标进行调整
        float adjustedX = othermapOriginX + switchPos.x * othermap->getScale(); // 地图左下角 + 出生点的 x 偏移
        float adjustedY = othermapOriginY + switchPos.y * othermap->getScale(); // 地图左下角 + 出生点的 y 偏移
        switchPos.x = adjustedX;
        switchPos.y = adjustedY;
        //输出任务结束点位置
        CCLOG("taskEndPosition: x = %.2f, y = %.2f", switchPos.x, switchPos.y);
        // 存储到 taskEndPosition
        taskEndPosition = switchPos;
    }


 //////////////////////////////
 // 添加小地图
 /////////////////////////////
    auto miniMap = MiniMap::create(othermap, file, visibleSize, this);
    this->addChild(miniMap, 1);

    // 添加键盘事件监听器
    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(OtherScene::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(OtherScene::onKeyReleased, this);

    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    // 添加鼠标事件监听器
    auto mouseListener = cocos2d::EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(OtherScene::onMouseDown, this);
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

void OtherScene::update(float dt)
{
#if 1
    if ((isPopupVisible || isDialogActive || isPanelVisible|| isTaskVisible)) {
        auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
        hero->m_moveUp = false;
        hero->m_moveDown = false;
        hero->m_moveLeft = false;
        hero->m_moveRight = false;
        return;
    }
#endif
    Node::update(dt);


    auto children = getChildren();
    for (auto child : children) {
        auto character = dynamic_cast<Hero*>(child);
        auto enemy = dynamic_cast<Enemy*>(child);
        if (character) {
            character->update(dt);
            if (character) {
                //更新角色相关的ui
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
            //判断地图传送点和人物是否碰撞
         // 跳过未激活的传送点
            if (!switchPoint.isActive) continue;

            // 判断是否有弹窗正在显示
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

                // Yes 按钮的回调
                yesButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    CCLOG("User selected YES. Teleporting to %s.", switchPoint.targetMap.c_str());
                    CCLOG("actual Switch Position: x = %.2f, y = %.2f", switchPoint.position.x, switchPoint.position.y);
                    // 播放点击音效
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/click_sl.mp3");

                    hero->saveProfile("hero.txt");
                    dialog->removeFromParent();
                    isDialogActive = false; // 恢复标志位
                    if (switchPoint.targetMap == "mainmap.tmx") {
                        Director::getInstance()->pushScene(MainScene::createScene());
                    }
                    });

                // No 按钮的回调
                noButton->addClickEventListener([=, &switchPoint](Ref* sender) {
                    CCLOG("User selected NO. Dialog removed.");
                    // 播放点击音效
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/click_sl.mp3");

                    if (dialog) {
                        dialog->removeFromParent();
                        // 恢复标志位
                        isDialogActive = false;
                    }

                    // 将角色移出传送点范围
                    //float offset = 100.0f;
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
                    if (mapname == "forest.tmx") {
                        MapManager::getInstance()->saveforestSwitchPoints(positionSwitchPoints);
                    }
                    else if (mapname == "desert.tmx") {
                        MapManager::getInstance()->savedesertSwitchPoints(positionSwitchPoints);
                    }
                    else if (mapname == "town.tmx") {
                        MapManager::getInstance()->savetownSwitchPoints(positionSwitchPoints);
                    }
                }

                break;
            }
        }
        // 如果角色进入范围并按下了 E 键
        if (isInRange && isKeyPressedE) {
            if (!isPopupVisible) {
                // 如果弹窗未显示，则显示弹窗
                showSelectionPopup_positionSwitchPoints();
                isPopupVisible = true;  // 设置弹窗为可见
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");

            }
        }
        else if (isPopupVisible && !isKeyPressedE) {
            // 如果弹窗已显示且玩家没有按下E键，则继续监听E键关闭弹窗
            if (isKeyPressedE) {
                hidePopup();  // 隐藏弹窗
                isPopupVisible = false;  // 设置弹窗为不可见
            }
        }


    }
    ////////////////////
    //人物碰到任务触发点
    ////////////////////
    auto player_task = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (player_task) {
        // 设置触发范围半径
        float triggerRadius = 50.0f;
        Vec2 currentPosition = player_task->getPosition();  // 获取角色当前位置
        bool isInRange = false;
        // 检测角色是否进入任务点的范围
        if (currentPosition.distance(taskStartPosition) < triggerRadius) {
            isInRange = true;
        }
        // 如果角色进入范围并按下了 R 键
        if (isInRange && isKeyPressedR&&!tasking) {
            if (!isTaskVisible) {
                // 播放点击音效
                if (mapname == "forest.tmx") {
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/npc_forest.mp3");
                }
                if (mapname == "desert.tmx") {
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/npc_desert.mp3");
                }
                if (mapname == "town.tmx") {
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/npc_forest.mp3");
                }
                // 如果弹窗未显示，则显示弹窗
                showSelectionPopup_taskStartPosition();
                isTaskVisible = true;  // 设置弹窗为可见
            }
        }
    }
    ////////////////////
    //人物碰到任务结束点
    ////////////////////
    if (mapname == "forest.tmx" || mapname == "desert.tmx") {
        auto player_taskend = dynamic_cast<Hero*>(this->getChildByName("hero"));
        if (player_taskend) {
            // 设置触发范围半径
            float triggerRadius = 50.0f;
            Vec2 currentPosition = player_taskend->getPosition();  // 获取角色当前位置
            bool isInRange = false;
            // 检测角色是否进入任务结束点的范围
            if (currentPosition.distance(taskEndPosition) < triggerRadius) {
                isInRange = true;
            }
            if (isInRange && tasking) {
                tasking = false;// 任务结束
                ///////////////////
                //添加发放奖励代码
                ///////////////////
                if (mapname == "forest.tmx") {
                    positionSwitchPoints[2].isActive = true;
                }
            }
        }
    }
}

void OtherScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
    if (hero && !(isPopupVisible || isDialogActive)) {
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
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_R:
        case cocos2d::EventKeyboard::KeyCode::KEY_R:
            isKeyPressedR = true;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_B:
        case cocos2d::EventKeyboard::KeyCode::KEY_B:
            isKeyPressedB = true;
            break;
    }
}

void OtherScene::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
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
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_R:
        case cocos2d::EventKeyboard::KeyCode::KEY_R:
            isKeyPressedR =false;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_CAPITAL_B:
        case cocos2d::EventKeyboard::KeyCode::KEY_B:
            isKeyPressedB = false;
    }
}
void OtherScene::onMouseDown(cocos2d::EventMouse* event)
{
    auto mouseEvent = dynamic_cast<EventMouse*>(event);
    if (isPopupVisible || isDialogActive)
        return;
    if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT) {
        auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
        if (hero && hero->m_isBulletChosen) {
            hero->attackWithBullet(TranslatePos(mouseEvent->getLocation()));
        }
    }
    else if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        auto hero = dynamic_cast<Hero*>(this->getChildByName("hero"));
        if (hero && hero->m_isBayonetChosen) {
            hero->attackWithBayonet();
        }
    }
}

cocos2d::Vec2 OtherScene::TranslatePos(cocos2d::Vec2 origin)
{
    cocos2d::Vec2 res;
    //auto map = dynamic_cast<TMXTiledMap*>(this->getChildByName("map"));
    float mapOriginX = othermap->getPositionX() - (othermap->getContentSize().width * othermap->getScale() * othermap->getAnchorPoint().x);
    float mapOriginY = othermap->getPositionY() - (othermap->getContentSize().height * othermap->getScale() * othermap->getAnchorPoint().y);
    res.x = -mapOriginX + origin.x; // 地图左下角 x 偏移修正
    res.y = -origin.y + 768;           // 地图左下角 y 偏移修正
    return res;
}

bool OtherScene::checkCollision(cocos2d::Vec2 position)
{

    // 获取瓦片层（假设墙壁层的名字为 "WallLayer"）
    TMXLayer* wallLayer = othermap->getLayer("WallLayer");

    // 获取目标位置的瓦片ID
    Vec2 tileCoord = tileCoordForPosition(position);
    int tileGID = wallLayer->getTileGIDAt(tileCoord);

    // 如果瓦片ID大于0，表示该位置有墙壁
    if (tileGID != 0) {
        return true;  // 碰到墙壁
    }
    return false;  // 没有碰到墙壁
}
cocos2d::Vec2 OtherScene::tileCoordForPosition(Vec2 position)
{
    float x = position.x;
    float y = position.y;

    // 假设每个瓦片的大小是 16x16
    int tileWidth = 16;  // 每个瓦片的宽度
    int tileHeight = 16; // 每个瓦片的高度

    // 计算瓦片坐标
    int tileX = (int)(x / tileWidth);
    int tileY = (int)((othermap->getMapSize().height * tileHeight - y) / tileHeight);

    // 输出瓦片坐标
    CCLOG("Converting position (%.2f, %.2f) to tile coordinates: (%.2f, %.2f)", x, y, tileX, tileY);

    return Vec2(tileX, tileY);
}

void OtherScene::showSelectionPopup_positionSwitchPoints()
{

    auto player = dynamic_cast<Hero*>(this->getChildByName("hero"));
    auto popupLayer = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150)); // 半透明黑色背景
    this->addChild(popupLayer, 10);  // 将图层添加到场景，并设置显示优先级
    // 遍历所有spot，生成按钮
    for (const auto& spot : positionSwitchPoints) {
        // 只有isActive为true时，才显示按钮
        if (spot.isActive) {
            // 创建按钮，使用图片作为按钮的背景
            auto button=cocos2d::MenuItemImage::create();
            if (mapname == "forest.tmx") {
                auto button_forest = cocos2d::MenuItemImage::create(
                    "Transfer_switch/Transfer_normal_forest.png",  // 普通状态的按钮图片
                    "Transfer_switch/Transfer_selected_forest.png",  // 按下状态的按钮图片
                    [this, spot, popupLayer, player](cocos2d::Ref* sender) {  // 捕获 player
                        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");

                        player->setPosition(spot.position);  // 传送角色
                        popupLayer->removeFromParent();  // 隐藏弹窗
                        isPopupVisible = false;  // 设置弹窗为不可见
                    }
                );
                button = button_forest;
            }
            else if (mapname == "desert.tmx") {
                auto button_desert = cocos2d::MenuItemImage::create(
                    "Transfer_switch/Transfer_normal_desert.png",  // 普通状态的按钮图片
                    "Transfer_switch/Transfer_select_desert.png",  // 按下状态的按钮图片
                    [this, spot, popupLayer, player](cocos2d::Ref* sender) {  // 捕获 player
                        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");

                        player->setPosition(spot.position);  // 传送角色
                        popupLayer->removeFromParent();  // 隐藏弹窗
                        isPopupVisible = false;  // 设置弹窗为不可见
                    }
                );
                button = button_desert;
            }
            else if (mapname == "town.tmx") {
                auto button_town = cocos2d::MenuItemImage::create(
                    "Transfer_switch/Transfer_normal_town.png",  // 普通状态的按钮图片
                    "Transfer_switch/Transfer_selected_town.png",  // 按下状态的按钮图片
                    [this, spot, popupLayer, player](cocos2d::Ref* sender) {  // 捕获 player
                        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/transfer.mp3");

                        player->setPosition(spot.position);  // 传送角色
                        popupLayer->removeFromParent();  // 隐藏弹窗
                        isPopupVisible = false;  // 设置弹窗为不可见
                    }
                );
                button = button_town;
            }
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

void OtherScene::showSelectionPopup_taskStartPosition()
{
    auto player = dynamic_cast<Hero*>(this->getChildByName("hero"));
    auto dialog = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 150)); // 半透明黑色背景
    this->addChild(dialog, 10);  // 将图层添加到场景，并设置显示优先级
    Label* label = nullptr;
    // 根据 mapname 设置不同的标签内容
    if (mapname == "forest.tmx") {
        label = Label::createWithSystemFont("Do you want to start the task:\nComplete the maze within the time limit", "fonts/Marker Felt.ttf", 40);
    }
    else if (mapname == "desert.tmx") {
        label = Label::createWithSystemFont("Do you want to start the task:\nCollect enough gems within a limited time", "fonts/Marker Felt.ttf", 40);
    }
    else if (mapname == "town.tmx") {
        label = Label::createWithSystemFont("The town has been overrun by the bad guys T.T \nWarrior! would you like to help me save the town", "fonts/Marker Felt.ttf", 40); // 默认标签
    }
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
    yesButton->addClickEventListener([=](Ref* sender) {
        // 播放点击音效
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/click_sl.mp3");
        CCLOG("User selected YES. Starting the task...");
        tasking = true;
        dialog->removeFromParent();  // 移除对话框
        isTaskVisible = false;  // 恢复标志位
        //**表示是否在任务状态
        tasking == true;
        /////////////////////////////
        // 这里开始任务
        /////////////////////////////
        if (mapname == "forest.tmx")
        {
            // 创建迷宫任务节点
            // 在20秒内完成迷宫任务，即可获得奖励
            auto maze = Maze::Create("Forest Maze", taskStartPosition, taskEndPosition, 20.0f, "Unlock a new map! Get ranged weapons!");
            this->addChild(maze,10);

            // 启动任务
            maze->StartTask();
            //任务的奖励在任务类中实现
        }
        if (mapname == "desert.tmx")
        {
            // 创建迷宫任务节点
            auto treasure_hunt = TreasureHunt::create(Director::getInstance()->getVisibleSize(), othermap, player, taskStartPosition, taskEndPosition, 30,30.0f);
            this->addChild(treasure_hunt, 10);

            // 启动任务
            treasure_hunt->StartTask();

            //任务的奖励在任务类中实现
        }
        if (mapname == "town.tmx")
        {
            int em = get_enemies_num();
            CCLOG("######get_enemies_num: %d", em); // 输出当前敌人数
            auto enemy_hunt = EnemyHunt::create(Director::getInstance()->getVisibleSize(),othermap, player, enemies, em);
            this->addChild(enemy_hunt, 10);

            enemy_hunt->StartTask();
            CCLOG("Start the Task...");
            //任务的奖励在任务类中实现
        }
        CCLOG("Start the Task...");
        });

    // No 按钮的回调
    noButton->addClickEventListener([=](Ref* sender) {
        // 播放点击音效
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/click_sl.mp3");
        CCLOG("User selected NO. Dialog removed.");
        dialog->removeFromParent();  // 移除对话框
        isTaskVisible = false;  // 恢复标志位
        });
}

void OtherScene::hidePopup()
{
    // 隐藏弹窗
    this->removeChildByName("popupLayer");
    isPopupVisible = false;
}

int OtherScene::get_enemies_num()
{
    int num = 0;
    auto children = getChildren();
    for (auto child : children) {
        auto enemy = dynamic_cast<Enemy*>(child);
        if (enemy) {
            if (enemy->isAlive() && enemy)
            {
                num++;
            }
        }
    }
    return num;
}

void OtherScene::returnToLastScene()
{
    // 使用 popScene 返回到之前的场景
    Director::getInstance()->popScene();
}

void OtherScene::menuCloseCallback(Ref* pSender)
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
