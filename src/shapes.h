#ifndef SHAPES_H
#define SHAPES_H

#include "maths.h"
#include <vector>

struct Sphere {
    Vector3 center;
    float radius;

    Sphere(Vector3 cen, float r) : center(cen), radius(r) {}
};

struct Triangle {
    Vector3 v0, v1, v2;
    Vector3 normal;
};

struct Hit {
    Vector3 point;
    Vector3 normal;
    float t;
    bool front_face;
    int object_index;
};

struct ShapeContainer {
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
};

bool sphere_hit(const Sphere& sphere, const Ray& ray, float t_min, float t_max, Hit& hit);
bool triangle_hit(const Triangle& triangle, const Ray& ray, float t_min, float t_max, Hit& hit);

#endif