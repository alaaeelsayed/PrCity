#pragma once
#include "../wolf/wolf.h"
#include <string>

struct Vertex
{
    GLfloat x,y,z;
    GLfloat u,v;
    GLfloat nx,ny,nz;
    GLfloat tx,ty,tz;
};

class Shape
{
protected:
    float m_x, m_y, m_z;
    float m_rotationX = 0, m_rotationY = 0;
    float m_width = 1, m_height = 1, m_depth = 1;
    int m_numVertices;
    glm::vec3 m_lightDirection = glm::vec3(0, -500.0f, -500.0f);
    wolf::VertexBuffer* m_vb = 0;
    wolf::VertexDeclaration* m_decl = 0;
    wolf::Material* m_mat = nullptr;
    wolf::Texture* m_texture = nullptr;
    wolf::Texture* m_normalMap = nullptr;
public:
    Shape(Vertex*, int, const std::string&, const std::string&, float, float, float);
    virtual ~Shape();
    void setPosition(float, float, float);
    void setWidth(float);
    void setHeight(float);
    void setDepth(float);
    void rotate(float, float);
    void setLightDirection(const glm::vec3&);
    virtual void update(float);
    virtual void render(int, int, const glm::mat4&, const glm::mat4&);
};