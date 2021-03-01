#pragma once
#include "../wolf/wolf.h"
#include "cube.h"

class Building 
{
protected:
    float m_x, m_y, m_z;
    int m_height;
    std::vector<Cube*> m_roofCubes;
    std::vector<Cube*> m_baseCubes;
public:
    Building(int, float, float, float);
    virtual ~Building();

    void setPosition(float, float, float);
    void setLightDirection(const glm::vec3&);
    virtual void update(float);
    virtual void render(int, int, const glm::mat4&, const glm::mat4&);
};