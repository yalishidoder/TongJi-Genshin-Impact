#ifndef __OTHER_SCENE_H__
#define __OTHER_SCENE_H__
#include "Scene/MainScene.h"
#include "cocos2d.h"
#include "Character/Hero/Hero.h"


class OtherScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene(const std::string& mapFile);

	virtual bool init(const std::string& mapFile);
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);

	void update(float dt);

	void returnToLastScene();

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);


	void showSelectionPopup();

	void hidePopup();
private:
	cocos2d::TMXTiledMap* othermap;
	std::string mapname;
	//��ͼ�л������Ϣ����
	std::vector<SceneSwitchPoint> sceneSwitchPoints;

	//λ�ô��͵����Ϣ����
	std::vector<PositionSwitchPoint> positionSwitchPoints;
	//��ǰ�Ƿ��е�ͼ�л��㵯��
	bool isDialogActive = false;
	//��ǰ�Ƿ���λ�ô��͵㵯��    
	bool isPopupVisible = false;
	//��¼ E ���Ƿ񱻰���
	bool isKeyPressedE = false;


};
#endif // __OtherScene_SCENE_H__