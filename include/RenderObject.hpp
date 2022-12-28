#pragma once

#include "../include/Ray.hpp"

class RenderObject
{
public:
	virtual double getIntersection(const Ray &r) const;
	virtual Vector3 getNormal(const Vector3 &hit) const;
};
