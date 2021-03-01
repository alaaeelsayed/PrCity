#include "stackedbuilding.h"

StackedBuilding::~StackedBuilding()
{

}

StackedBuilding::StackedBuilding(int height, float initX, float initY, float initZ)
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
    
    Cube* base = new Cube(chosen, "city", m_x, m_y + height/2.0f, m_z);
    base->setWidth(8);
    base->setHeight((float)height);
    base->setDepth(8);
    m_baseCubes.push_back(base);

    for(int i = 0; i < 3; i++)
    {
        float stackHeight = (float)(height - rand() % (height/2));
        Cube* stack = new Cube(chosen, "city", m_x + rand() % 6 - 3, m_y + stackHeight/2.0f, m_z + rand() % 6 - 3);
        stack->setWidth(8);
        stack->setHeight(stackHeight);
        stack->setDepth(8);
        m_baseCubes.push_back(stack);
    }
}