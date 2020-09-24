#ifndef SHAPES_H
#define SHAPES_H

#include "maths.h"

struct Sphere {
    Vector3 center;
    float radius;

    Sphere(Vector3 cen, float r) : center(cen), radius(r) {}
};

struct Hit {
    Vector3 point;
    Vector3 normal;
    float t;
    bool front_face;
    int object_index;
};

bool sphere_hit(const Sphere& sphere, const Ray& ray, float t_min, float t_max, Hit& hit);

#endif