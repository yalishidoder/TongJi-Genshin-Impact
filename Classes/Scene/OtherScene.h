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
	void onMouseDown(cocos2d::EventMouse* event);
	cocos2d::Vec2 TranslatePos(cocos2d::Vec2 origin);

	//���λ���Ƿ�����ײ
	bool checkCollision(cocos2d::Vec2 position);
	//���ڵ���Ƭ��ͼ����ת��
	cocos2d::Vec2 tileCoordForPosition(cocos2d::Vec2 position);
	void operatemyPanel();
	void showSelectionPopup_positionSwitchPoints();
	void showSelectionPopup_taskStartPosition();
	void hidePopup();
private:
	cocos2d::TMXTiledMap* othermap;
	std::string mapname;
	//��ͼ�л������Ϣ����
	std::vector<SceneSwitchPoint> sceneSwitchPoints;

	//λ�ô��͵����Ϣ����
	std::vector<PositionSwitchPoint> positionSwitchPoints;
	//���񴥷���
	cocos2d::Vec2 taskStartPosition;
	//���������
	cocos2d::Vec2 taskEndPosition;
	//��ǰ�Ƿ��е�ͼ�л��㵯��
	bool isDialogActive = false;
	//��ǰ�Ƿ���λ�ô��͵㵯��    
	bool isPopupVisible = false;
	//��ǰ�Ƿ������񵯴� 
	bool isTaskVisible = false;
	//��ǰ�Ƿ���ʾ��ҽ�ɫ���
	bool isPanelVisible = false;

	//��¼ E ���Ƿ񱻰���
	bool isKeyPressedE = false;
	//��¼ P ���Ƿ񱻰��£������壩
	bool isKeyPressedP = false;
	//��¼ R ���Ƿ񱻰��£���������
	bool isKeyPressedR = false;
	bool tasking = false;
	//��¼ B ���Ƿ񱻰��£�������
	bool isKeyPressedB = false;
	PlayerPanel* m_playerPanel = nullptr;

};
#endif // __OtherScene_SCENE_H__