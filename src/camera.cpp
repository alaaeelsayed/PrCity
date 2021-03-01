#include "camera.h"

Camera::Camera(wolf::App* app)
    : m_app(app)
{
    m_lastMousePos = app->getMousePos();
}

Camera::~Camera()
{

}

void Camera::update(float dt)
{
    glm::vec2 mousePos = m_app->getMousePos();

    glm::vec2 mouseMovement = mousePos - m_lastMousePos;
    _rotate(mouseMovement);

    glm::vec2 mouseScroll = m_app->getMouseScroll();

    if(mouseScroll.y > 0) {
        m_fov -= (m_fov / 5.0f);
        m_fov = wolf::max(m_fov, 1.0f);
    } else if(mouseScroll.y < 0) {
        m_fov += (m_fov / 5.0f);
        m_fov = wolf::min(m_fov, 45.0f);
    }

    // LControl
    float cameraSpeed = m_app->isKeyDown(341) * 0.5f + 0.3f;

    // Gotta let the cool kids strafe
    if(m_app->isKeyDown('w'))
    {
        m_position += cameraSpeed * m_eye;
    }
    if(m_app->isKeyDown('s'))
    {
        m_position -= cameraSpeed * m_eye;
    }
    if (m_app->isKeyDown('a'))
    {
        m_position += _getCameraSide() * cameraSpeed;
    }
    if (m_app->isKeyDown('d'))
    {
        m_position -= _getCameraSide() * cameraSpeed;
    }
    // LShift
    if (m_app->isKeyDown(340))
    {
        m_position -= _getCameraUp() * cameraSpeed;
    }
    // Space
    if (m_app->isKeyDown(32))
    {
        m_position += _getCameraUp() * cameraSpeed;
    }
    m_lastMousePos = mousePos;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(m_position, _getCameraTarget(), _getCameraUp());
}

glm::mat4 Camera::getProjMatrix(int width, int height)
{
	return glm::perspective(glm::radians(m_fov), (float)width / (float)height, m_near, m_far);
}

void Camera::lookAt(const glm::vec3& target)
{
    m_eye.x = target.x;
    m_eye.y = target.y;
    m_eye.z = target.z;
}

void Camera::_rotate(const glm::vec2& mouseMovement)
{
    m_rotX += mouseMovement.x * 0.1f;
    m_rotY -= mouseMovement.y * 0.1f;
    m_rotY = wolf::max(wolf::min(89.0f, m_rotY), -89.0f);
    m_eye = glm::normalize(glm::vec3(cos(glm::radians(m_rotX)) * cos(glm::radians(m_rotY)), sin(glm::radians(m_rotY)), sin(glm::radians(m_rotX)) * cos(glm::radians(m_rotY))));
}

glm::vec3 Camera::_getCameraSide()
{
    glm::vec3 side = glm::cross(glm::vec3(0.0f,1.0f,0.0f), m_eye);
    return glm::normalize(side);
}

glm::vec3 Camera::_getCameraUp()
{
    glm::vec3 v = _getCameraSide();
    v = glm::cross(m_eye, v);
    return glm::normalize(v);
}

glm::vec3 Camera::_getCameraTarget()
{
    return m_position + m_eye;
}

glm::vec3 Camera::getViewDirection() const
{
    return glm::normalize(m_eye);
}