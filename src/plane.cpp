#include "plane.h"

static Vertex planeVertices []{
    //   x     y     z     u     v    nx    ny    nz
    {  -1.0f, 0.0f, 1.0f, 0.0f, 20.0f, 0.0f, 1.0f, 0.0f, },
	{  1.0f, 0.0f, 1.0f, 20.0f, 20.0f, 0.0f, 1.0f, 0.0f, },
	{  1.0f, 0.0f, -1.0f, 20.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
	{  1.0f, 0.0f, -1.0f, 20.0f, 20.0f, 0.0f, 1.0f, 0.0f, },
	{  -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
	{  -1.0f, 0.0f, 1.0f, 0.0f, 20.0f, 0.0f, 1.0f, 0.0f, }
};

Plane::~Plane()
{

}

Plane::Plane(const std::string& textureName, const std::string& matName, float initX, float initY, float initZ) 
    : Shape(planeVertices, 6, textureName, matName, initX, initY, initZ)
{
}