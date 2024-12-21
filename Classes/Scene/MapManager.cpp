#include "MapManager.h"

// ��ȡ MapManager �ĵ���
MapManager* MapManager::getInstance() {
    static MapManager instance;
    return &instance;
}

// ���� MainScene �Ĵ��͵�����
void MapManager::saveMainSceneSwitchPoints(const std::vector<PositionSwitchPoint>& points) {
    mainSceneSwitchPoints = points;
}

void MapManager::saveforestSwitchPoints(const std::vector<PositionSwitchPoint>& points)
{
    forestSwitchPoints = points;
}

void MapManager::savedesertSwitchPoints(const std::vector<PositionSwitchPoint>& points)
{
    desertSwitchPoints = points;
}

void MapManager::savetownSwitchPoints(const std::vector<PositionSwitchPoint>& points)
{
    townSwitchPoints = points;
}

// ��ȡ����� MainScene �Ĵ��͵�����
std::vector<PositionSwitchPoint> MapManager::getMainSceneSwitchPoints() const {
    return mainSceneSwitchPoints;
}

std::vector<PositionSwitchPoint> MapManager::getforestSwitchPoints() const
{
    return forestSwitchPoints;
}

std::vector<PositionSwitchPoint> MapManager::getdesertSwitchPoints() const
{
    return desertSwitchPoints;
}

std::vector<PositionSwitchPoint> MapManager::gettownSwitchPoints() const
{
    return townSwitchPoints;
}
