#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"

class material;

struct hit_record
{
    double t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hittable  {
    public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(aabb& box) const  = 0;
};

#endif