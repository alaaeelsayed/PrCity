#pragma once
#include "../wolf/wolf.h"
#include "camera.h"
#include "building.h"
#include "skybox.h"
#include "plane.h"

class City 
{
private:
    wolf::App* m_app;
    Camera* m_camera;
    Skybox* m_skybox;
    Plane* m_floor;
    std::vector<Building*> m_buildings;
    bool m_keyDown = false;
    
public:
    City(wolf::App*);
    ~City();

    void reset();
    void update(float);
    void render(int, int);
};