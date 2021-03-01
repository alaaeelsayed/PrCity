#include "building.h"

Building::~Building()
{
    for(auto it = m_baseCubes.begin(); it != m_baseCubes.end(); ++it) 
    {
        Cube* cube = *it;
        delete cube;
    }
    for(auto it = m_roofCubes.begin(); it != m_roofCubes.end(); ++it) 
    {
        Cube* cube = *it;
        delete cube;
    }
}

Building::Building(int height, float initX, float initY, float initZ)
    : m_height(height), m_x(initX), m_y(initY), m_z(initZ)
{

}

void Building::setPosition(float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void Building::setLightDirection(const glm::vec3& lightDirection)
{
    for(auto it = m_baseCubes.begin(); it != m_baseCubes.end(); ++it)
    {
        Cube* cube = *it;
        cube->setLightDirection(lightDirection);
    }
    for(auto it = m_roofCubes.begin(); it != m_roofCubes.end(); ++it)
    {
        Cube* cube = *it;
        cube->setLightDirection(lightDirection);
    }
}

void Building::update(float dt)
{
    // Empty by default
}

void Building::render(int width, int height, const glm::mat4& mProj, const glm::mat4& mView)
{
    for(auto it = m_baseCubes.begin(); it != m_baseCubes.end(); ++it)
    {
        Cube* cube = *it;
        cube->render(width, height, mProj, mView);
    }
    for(auto it = m_roofCubes.begin(); it != m_roofCubes.end(); ++it)
    {
        Cube* cube = *it;
        cube->render(width, height, mProj, mView);
    }
}