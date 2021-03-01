#include "skybox.h"

#define NUM_VERTS 6*3*2

static Vertex skyboxVertices[] = {
    // Front
    {-0.5f, -0.5f, 0.5f, 1/4.0f, 2/3.0f, 0.0f, 0.0f, 1.0f, },
    {-0.5f, 0.5f, 0.5f, 1/4.0f, 1/3.0f, 0.0f, 0.0f, 1.0f, },
    {0.5f, 0.5f, 0.5f, 1/2.0f, 1/3.0f, 0.0f, 0.0f, 1.0f, },
    {0.5f, 0.5f, 0.5f, 1/2.0f, 1/3.0f, 0.0f, 0.0f, 1.0f, },
    {0.5f, -0.5f, 0.5f, 1/2.0f, 2/3.0f, 0.0f, 0.0f, 1.0f, },
    {-0.5f, -0.5f, 0.5f, 1/4.0f, 2/3.0f, 0.0f, 0.0f, 1.0f, },

    // Back
    {0.5f, 0.5f, -0.5f, 3/4.0f, 2/3.0f, 0.0f, 0.0f, -1.0f, },
    {-0.5f, 0.5f, -0.5f, 1.0f, 2/3.0f, 0.0f, 0.0f, -1.0f, },
    {-0.5f, -0.5f, -0.5f, 1.0f, 1/3.0f, 0.0f, 0.0f, -1.0f, },
    {-0.5f, -0.5f, -0.5f, 1.0f, 1/3.0f, 0.0f, 0.0f, -1.0f, },
    {0.5f, -0.5f, -0.5f, 3/4.0f, 1/3.0f, 0.0f, 0.0f, -1.0f, },
    {0.5f, 0.5f, -0.5f, 3/4.0f, 2/3.0f, 0.0f, 0.0f, -1.0f, },

    // Left
    {-0.5f, 0.5f, -0.5f, 0.0f, 2/3.0f, -1.0f, 0.0f, 0.0f, },
    {-0.5f, 0.5f, 0.5f, 1/4.0f, 2/3.0f, -1.0f, 0.0f, 0.0f, },
    {-0.5f, -0.5f, 0.5f, 1/4.0f, 1/3.0f, -1.0f, 0.0f, 0.0f, },
    {-0.5f, -0.5f, 0.5f, 1/4.0f, 1/3.0f, -1.0f, 0.0f, 0.0f, },
    {-0.5f, -0.5f, -0.5f, 0.0f, 1/3.0f, -1.0f, 0.0f, 0.0f, },
    {-0.5f, 0.5f, -0.5f, 0.0f, 2/3.0f, -1.0f, 0.0f, 0.0f, },

    // Right
    {0.5f, 0.5f, 0.5f, 1/2.0f, 2/3.0f, 1.0f, 0.0f, 0.0f, },
    {0.5f, 0.5f, -0.5f, 3/4.0f, 2/3.0f, 1.0f, 0.0f, 0.0f, },
    {0.5f, -0.5f, -0.5f, 3/4.0f, 1/3.0f, 1.0f, 0.0f, 0.0f, },
    {0.5f, -0.5f, -0.5f, 3/4.0f, 1/3.0f, 1.0f, 0.0f, 0.0f, },
    {0.5f, -0.5f, 0.5f, 1/2.0f, 1/3.0f, 1.0f, 0.0f, 0.0f, },
    {0.5f, 0.5f, 0.5f, 1/2.0f, 2/3.0f, 1.0f, 0.0f, 0.0f, },

    // Top
    {-0.5f, 0.5f, 0.5f, 1/4.0f, 2/3.0f, 0.0f, 1.0f, 0.0f, },
    {-0.5f, 0.5f, -0.5f, 1/4.0f, 1.0f, 0.0f, 1.0f, 0.0f, },
    {0.5f, 0.5f, -0.5f, 1/2.0f, 1.0f, 0.0f, 1.0f, 0.0f, },
    {0.5f, 0.5f, -0.5f, 1/2.0f, 1.0f, 0.0f, 1.0f, 0.0f, },
    {0.5f, 0.5f, 0.5f, 1/2.0f, 2/3.0f, 0.0f, 1.0f, 0.0f, },
    {-0.5f, 0.5f, 0.5f, 1/4.0f, 2/3.0f, 0.0f, 1.0f, 0.0f, },

    // Bottom
    {-0.5f, -0.5f, 0.5f, 1/4.0f, 1/3.0f, 0.0f, -1.0f, 0.0f, },
    {0.5f, -0.5f, 0.5f, 1/2.0f, 1/3.0f, 0.0f, -1.0f, 0.0f, },
    {0.5f, -0.5f, -0.5f, 1/2.0f, 0.0f, 0.0f, -1.0f, 0.0f, },
    {0.5f, -0.5f, -0.5f, 1/2.0f, 0.0f, 0.0f, -1.0f, 0.0f, },
    {-0.5f, -0.5f, -0.5f, 1/4.0f, 0.0f, 0.0f, -1.0f, 0.0f, },
    {-0.5f, -0.5f, 0.5f, 1/4.0f, 1/3.0f, 0.0f, -1.0f, 0.0f, },
};

Skybox::~Skybox()
{

}

Skybox::Skybox(const Camera* camera) : Shape(skyboxVertices, NUM_VERTS, "skybox/night", "skybox", 0.0f, 0.0f, 0.0f), m_camera(camera) 
{
    m_texture->SetFilterMode(wolf::Texture::FM_Linear, wolf::Texture::FM_Linear);
    m_texture->SetWrapMode(wolf::Texture::WM_Clamp);
}

glm::vec3 Skybox::getLightDirection()
{
    return glm::mat3(cos(m_rotationX), 0.0f, sin(m_rotationX), 0.0f, 1.0f, 0.0f, -sin(m_rotationX), 0.0f, cos(m_rotationX)) * glm::vec3(0, -500.0f, -500.0f);
}

void Skybox::update(float dt)
{
    rotate(dt * 0.2f, 0.0f);
}

void Skybox::render(int width, int height, const glm::mat4& mProj, const glm::mat4& mView)
{
    Shape::render(width, height, mProj, glm::mat4(glm::mat3(mView)));
}