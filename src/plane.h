#pragma once
#include "shape.h"

class Plane : public Shape
{
public:
    Plane(const std::string&, const std::string&, float, float, float);
    ~Plane();
};