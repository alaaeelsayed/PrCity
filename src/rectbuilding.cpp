#include "rectbuilding.h"

RectBuilding::~RectBuilding()
{

}

RectBuilding::RectBuilding(int height, float initX, float initY, float initZ)
    : Building(height, initX, initY, initZ)
{
    std::vector<std::string> buildings
    {
        "building1",
        "building2",
        "building3",
        "building4"
    };
    std::string chosen = buildings[rand() % 4];
    Cube* cube = new Cube(chosen, "city", m_x, m_y + height / 2.0f, m_z);
    cube->setWidth(8);
    cube->setDepth(8);
    cube->setHeight((float)height);
    m_baseCubes.push_back(cube);
}