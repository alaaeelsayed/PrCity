#include "tiledbuilding.h"

TiledBuilding::~TiledBuilding()
{

}

TiledBuilding::TiledBuilding(int height, float initX, float initY, float initZ)
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
    int roofTop = rand() % 3;
    int roofBottom = height - roofTop;
    Cube* cube = new Cube(chosen, "city", m_x, m_y + roofBottom/2.0f, m_z);
    cube->setWidth(8);
    cube->setHeight((float)roofBottom);
    cube->setDepth(8);
    m_baseCubes.push_back(cube);
    float roofHeight = roofTop * (height/4.0f);
    Cube* roofCube = new Cube(chosen, "city", m_x, m_y + roofBottom + roofHeight/2.0f, m_z);
    roofCube->setWidth(4);
    roofCube->setHeight(roofHeight);
    roofCube->setDepth(4);
    m_roofCubes.push_back(roofCube);
}