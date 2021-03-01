#include "city.h"
#include "rectbuilding.h"
#include "stackedbuilding.h"
#include "tiledbuilding.h"

City::~City()
{
    delete m_camera;
    delete m_skybox;
    for (auto it = m_buildings.begin(); it != m_buildings.end(); ++it)
    {
        Building *building = *it;
        delete building;
    }
    delete m_floor;
}

City::City(wolf::App *app) : m_app(app)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);

    m_camera = new Camera(app);
    m_skybox = new Skybox(m_camera);
    m_floor = new Plane("tile", "city", 0.0f, 0.0f, 0.0f);
    m_floor->setWidth(250.0f);
    m_floor->setHeight(250.0f);
    m_floor->setDepth(250.0f);
    reset();
}

void City::reset()
{
    for (auto it = m_buildings.begin(); it != m_buildings.end(); ++it)
    {
        Building *building = *it;
        delete building;
    }
    m_buildings.clear();
    float x = -150.0f;
    float z = -150.0f;
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            Building *building;
            int chosen = rand() % 3;
            int distanceFactor = (int)(sqrt(x * x + z * z) / 3.0f);
            switch (chosen)
            {
            case 0:
                building = new RectBuilding(rand() % 20 + 72 - distanceFactor, x, 0.0f, z);
                break;
            case 1:
                building = new StackedBuilding(rand() % 20 + 72 - distanceFactor, x, 0.0f, z);
                break;
            case 2:
                building = new TiledBuilding(rand() % 20 + 72 - distanceFactor, x, 0.0f, z);
                break;
            }
            m_buildings.push_back(building);
            x += 15.0f;
        }
        x = -150.0f;
        z += 15.0f;
    }
}

void City::update(float dt)
{
    m_camera->update(dt);
    m_skybox->update(dt);

    for (auto it = m_buildings.begin(); it != m_buildings.end(); ++it)
    {
        Building *building = *it;
        building->setLightDirection(m_skybox->getLightDirection());
    }

    if (m_app->isKeyDown('r'))
        m_keyDown = true;
    else
    {
        if (m_keyDown)
            reset();

        m_keyDown = false;
    }
}

void City::render(int width, int height)
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 mProj = m_camera->getProjMatrix(width, height);
    glm::mat4 mView = m_camera->getViewMatrix();

    m_floor->render(width, height, mProj, mView);
    for (auto it = m_buildings.begin(); it != m_buildings.end(); ++it)
    {
        Building *building = *it;
        building->render(width, height, mProj, mView);
    }
    m_skybox->render(width, height, mProj, mView);
}