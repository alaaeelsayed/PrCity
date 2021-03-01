#pragma once
#include "../wolf/wolf.h"

// Modified samplefw/OrbitCamera class
class Camera
{
private:
    void _rotate(const glm::vec2& mouseMovement);
    glm::vec3 _getCameraUp();
    glm::vec3 _getCameraSide();
    glm::vec3 _getCameraTarget();

    float m_rotX                = -90.0f;
    float m_rotY                = 0.0f;
    glm::vec3 m_position        = glm::vec3(0.0f,50.0f,3.0f);
    glm::vec3 m_eye             = glm::vec3(0.0f,0.0f,-1.0f);
    float m_fov                 = 45.0f;
    glm::vec2 m_lastMousePos    = glm::vec2(0.0f,0.0f);
    wolf::App* m_app            = nullptr;

    const float m_near                = 0.1f;
    const float m_far                 = 1000.0f;
public:
    Camera(wolf::App* app);
    virtual ~Camera();

    void update(float);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjMatrix(int, int);
    glm::vec3 getViewDirection() const;

    void lookAt(const glm::vec3&);
};