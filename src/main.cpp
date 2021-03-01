#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../wolf/wolf.h"
#include "city.h"

class Week2: public wolf::App
{
public:
    Week2() : App("Week 4")
    {
        m_city = new City(this);
    }

    ~Week2()
    {
    }

    void update(float dt) override
    {
        m_city->update(dt);
    }

    void render() override
    {
        m_city->render(m_width, m_height);
    }

private:
    City* m_city;
};

int main(int, char**) {
    Week2 week2;
    week2.run();
}