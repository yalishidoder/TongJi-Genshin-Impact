#ifndef __OTHER_SCENE_H__
#define __OTHER_SCENE_H__
#include "Scene/MainScene.h"
#include "cocos2d.h"
#include "Character/Hero/Hero.h"
#include "Character/Enemy/Enemy.h"

class OtherScene : public cocos2d::Scene
{
public:
	OtherScene();
	~OtherScene();
	static cocos2d::Scene* createScene(const std::string& mapFile);
	//音频函数
	void playBackgroundMusic();
	void pauseBackgroundMusic();
	void resumeBackgroundMusic();
	void stopBackgroundMusic();

	void pauseMusicCallback(Ref* pSender);  // 暂停音乐的回调函数
	virtual bool init(const std::string& mapFile);
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void update(float dt);

	void returnToLastScene();

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onMouseDown(cocos2d::EventMouse* event);
	cocos2d::Vec2 TranslatePos(cocos2d::Vec2 origin);

	//检查位置是否发生碰撞
	bool checkCollision(cocos2d::Vec2 position);
	//窗口到瓦片地图坐标转化
	cocos2d::Vec2 tileCoordForPosition(cocos2d::Vec2 position);
	void showSelectionPopup_positionSwitchPoints();
	void showSelectionPopup_taskStartPosition();
	void hidePopup();
	int get_enemies_num();

	// 对玩家面板的操作
	void operatemyPanel();
	void showmyPanel();
	void hidemyPanel();
private:
	cocos2d::TMXTiledMap* othermap;
	cocos2d::Vec2 initspot_smallmap;

	std::string mapname;
	//地图切换点的信息储存
	std::vector<SceneSwitchPoint> sceneSwitchPoints;

	//位置传送点的信息储存
	std::vector<PositionSwitchPoint> positionSwitchPoints;
	//任务触发点
	cocos2d::Vec2 taskStartPosition;
	//任务结束点
	cocos2d::Vec2 taskEndPosition;
	//当前是否有地图切换点弹窗
	bool isDialogActive = false;
	//当前是否有位置传送点弹窗    
	bool isPopupVisible = false;
	//当前是否有任务弹窗 
	bool isTaskVisible = false;
	//当前是否显示玩家角色面板
	bool isPanelVisible = false;

	//记录 E 键是否被按下
	bool isKeyPressedE = false;
	//记录 P 键是否被按下（玩家面板）
	bool isKeyPressedP = false;
	//记录 R 键是否被按下（触发任务）
	bool isKeyPressedR = false;
	bool tasking = false;
	//记录 B 键是否被按下（背包）
	bool isKeyPressedB = false;
	//记录 U 键是否被按下（人物位置重置）
	bool isKeyPressedU = false;
	PlayerPanel* m_playerPanel = nullptr;
	std::vector<Enemy*> enemies;  // 存储town敌人的容器
	int enmies_num;
	int musicID;  // 用于控制背景音乐播放的 ID
	MenuItemImage* pauseButton;  // 暂停/恢复按钮


};
#endif // __OtherScene_SCENE_H__