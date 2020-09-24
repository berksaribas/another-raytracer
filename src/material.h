#ifndef MATERIAL_H
#define MATERIAL_H

#include "maths.h"
#include "shapes.h"

enum MaterialType {
    LAMBERTIAN,
    METAL,
    DIELECTRIC
};

struct Material {
    Vector3 albedo;
    union {
        float fuzziness;
        float ref_idx;
    };
    MaterialType type;
};

bool scatter(const Material& material, const Ray& incoming_ray, const Hit& hit, Vector3& attenuation, Ray& outgoing_ray);
#endif