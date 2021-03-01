#pragma once
#include "shape.h"
#include "camera.h"

class Skybox : public Shape
{
private:
    const Camera* m_camera;
public:
    Skybox(const Camera* camera);
    ~Skybox();

    glm::vec3 getLightDirection();

    virtual void update(float);
    virtual void render(int, int, const glm::mat4&, const glm::mat4&);
};