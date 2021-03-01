#pragma once
#include "shape.h"

class Cube : public Shape
{
public:
    Cube(const std::string&, const std::string&, float, float, float);
    ~Cube();
};