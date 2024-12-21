#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__

#include "cocos2d.h"
#include <vector>
#include "Scene/MainScene.h"

class MapManager {
public:
    // 获取 MapManager 的实例（单例模式）
    static MapManager* getInstance();

    // 保存 MainScene 的传送点数据
    void saveMainSceneSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // 保存 forest 的传送点数据
    void saveforestSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // 保存 desert 的传送点数据
    void savedesertSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // 保存 town 的传送点数据
    void savetownSwitchPoints(const std::vector<PositionSwitchPoint>& points);

    // 获取保存的 MainScene 的传送点数据
    std::vector<PositionSwitchPoint> getMainSceneSwitchPoints() const;

    // 获取保存的 forest 的传送点数据
    std::vector<PositionSwitchPoint> getforestSwitchPoints() const;

    // 获取保存的 desert 的传送点数据
    std::vector<PositionSwitchPoint> getdesertSwitchPoints() const;

    // 获取保存的 town 的传送点数据
    std::vector<PositionSwitchPoint> gettownSwitchPoints() const;


private:
    // 私有构造函数，防止外部实例化
    MapManager() {}
    ~MapManager() {}

    // 存储传送点数据
    std::vector<PositionSwitchPoint> mainSceneSwitchPoints;
    std::vector<PositionSwitchPoint> forestSwitchPoints;
    std::vector<PositionSwitchPoint> desertSwitchPoints;
    std::vector<PositionSwitchPoint> townSwitchPoints;

};

#endif // __MAPMANAGER_H__