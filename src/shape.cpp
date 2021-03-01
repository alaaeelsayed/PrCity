#include "shape.h"

Shape::~Shape()
{
	delete m_decl;
	wolf::BufferManager::DestroyBuffer(m_vb);
	wolf::MaterialManager::DestroyMaterial(m_mat);
    wolf::TextureManager::DestroyTexture(m_texture);
}

Shape::Shape(Vertex* vertices, int numVertices, const std::string& textureName, const std::string& matName, float initX, float initY, float initZ)
      : m_x(initX), m_y(initY), m_z(initZ), m_numVertices(numVertices)
{

    for(int i = 0; i < numVertices/3; i++)
    {
        int j = i * 3;
        Vertex vert1 = vertices[j];
        Vertex vert2 = vertices[j+1];
        Vertex vert3 = vertices[j+2];
        
        float x1 = vert2.x - vert1.x;
        float x2 = vert3.x - vert1.x;
        float y1 = vert2.y - vert1.y;
        float y2 = vert3.y - vert1.y;
        float z1 = vert2.z - vert1.z;
        float z2 = vert3.z - vert1.z;
        
        float s1 = vert2.u - vert1.u;
        float s2 = vert3.u - vert1.u;
        float t1 = vert2.v - vert1.v;
        float t2 = vert3.v - vert1.v;

        float f = 1.0f / (s1 * t2 - s2 * t1);

        (vertices+j)->tx = f * (t2 * x1 - t1 * x2);
        (vertices+j)->ty = f * (t2 * y1 - t1 * y1);
        (vertices+j)->tz = f * (t2 * z1 - t1 * z1);
        (vertices+j+1)->tx = f * (t2 * x1 - t1 * x2);
        (vertices+j+1)->ty = f * (t2 * y1 - t1 * y1);
        (vertices+j+1)->tz = f * (t2 * z1 - t1 * z1);
        (vertices+j+2)->tx = f * (t2 * x1 - t1 * x2);
        (vertices+j+2)->ty = f * (t2 * y1 - t1 * y1);
        (vertices+j+2)->tz = f * (t2 * z1 - t1 * z1);
    }

    m_texture = wolf::TextureManager::CreateTexture("data/textures/" + textureName + ".png");
	m_texture->SetFilterMode(wolf::Texture::FM_LinearMipmap, wolf::Texture::FM_Linear);
    m_texture->SetWrapMode(wolf::Texture::WM_Repeat);

    if(textureName.compare("skybox/night") != 0)
    {
        m_normalMap = wolf::TextureManager::CreateTexture("data/textures/" + textureName + "_normal.png");
        m_normalMap->SetFilterMode(wolf::Texture::FM_LinearMipmap, wolf::Texture::FM_Linear);
        m_normalMap->SetWrapMode(wolf::Texture::WM_Repeat);
    }

    m_mat = wolf::MaterialManager::CreateMaterial(textureName + "_" + matName);
	m_mat->SetProgram("data/shaders/" + matName + ".vsh", "data/shaders/" + matName + ".fsh");
	m_mat->SetTexture("tex", m_texture);
    if(textureName.compare("skybox/night") != 0) m_mat->SetTexture("u_normalMap", m_normalMap);

    m_vb = wolf::BufferManager::CreateVertexBuffer(vertices, sizeof(Vertex) * numVertices);

    m_decl = new wolf::VertexDeclaration();
    m_decl->Begin();
    m_decl->AppendAttribute(wolf::AT_Position, 3, wolf::CT_Float);
    m_decl->AppendAttribute(wolf::AT_TexCoord1, 2, wolf::CT_Float);
    m_decl->AppendAttribute(wolf::AT_Normal, 3, wolf::CT_Float);
    m_decl->AppendAttribute(wolf::AT_Tangent, 3, wolf::CT_Float);
    m_decl->SetVertexBuffer(m_vb);
    m_decl->End();
}

void Shape::setPosition(float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void Shape::setWidth(float width)
{
    m_width = width;
}

void Shape::setHeight(float height)
{
    m_height = height;
}

void Shape::setDepth(float depth)
{
    m_depth = depth;
}

void Shape::rotate(float xd, float yd)
{
    m_rotationX += xd;
    m_rotationY += yd;
}

void Shape::setLightDirection(const glm::vec3& lightDirection)
{
    m_lightDirection = lightDirection;
}

void Shape::update(float dt)
{
    // Empty by default
}

void Shape::render(int width, int height, const glm::mat4& mProj, const glm::mat4& mView)
{
    // Expected: Scale, Rotate X, Rotate Y, Translate

    // Translate
    glm::mat4 mWorld = glm::translate(glm::mat4(1.0f), glm::vec3(m_x, m_y, m_z));
    // Rotate Y
    mWorld *= glm::rotate(glm::mat4(1.0f), m_rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    // Rotate X
    mWorld *= glm::rotate(glm::mat4(1.0f), m_rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    // Scale
    mWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(m_width, m_height, m_depth));

    m_mat->SetUniform("projection", mProj);
    m_mat->SetUniform("view", mView);
    m_mat->SetUniform("world", mWorld);
    m_mat->SetUniform("scale", glm::scale(glm::mat4(1.0f), glm::vec3(m_width / 15.0f, m_height / 15.0f, m_depth / 15.0f)));
    m_mat->SetUniform("lightDirection", m_lightDirection);
    m_mat->SetUniform("lightColor", glm::vec3(0.0194f, 0.018f, 0.0148f)); // 0.0194f, 0.018f, 0.0148f
    m_mat->SetUniform("worldIT", glm::transpose(glm::inverse(mWorld)));
    m_mat->Apply();

	m_decl->Bind();

	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}