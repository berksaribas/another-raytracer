#include "material.h"

Vector3 random_in_unit_sphere() {
    while (true) {
        auto p = random_vector3(-1,1);
        if (dot(p,p) >= 1) continue;
        return p;
    }
}

Vector3 random_unit_vector() {
    auto a = random_float(0, 2 * PI);
    auto z = random_float(-1, 1);
    auto r = sqrt(1 - z*z);
    return Vector3(r * cos(a), r * sin(a), z);
}

bool lambertian_scatter(const Material& material, const Ray& incoming_ray, const Hit& hit, Vector3& attenuation, Ray& outgoing_ray) {
    auto dir = hit.normal + random_unit_vector();
    outgoing_ray = Ray(hit.point, dir);
    attenuation = material.albedo;
    return true;
}

bool metal_scater(const Material& material, const Ray& incoming_ray, const Hit& hit, Vector3& attenuation, Ray& outgoing_ray) {
    auto reflected_vec = reflect(unit_vector(incoming_ray.direction), hit.normal);
    outgoing_ray = Ray(hit.point, reflected_vec + random_in_unit_sphere() * material.fuzziness);
    attenuation = material.albedo;
    return (dot(outgoing_ray.direction, hit.normal) > 0);
}

bool dielectric_scater(const Material& material, const Ray& incoming_ray, const Hit& hit, Vector3& attenuation, Ray& outgoing_ray) {
    return false;
}

bool scatter(const Material& material, const Ray& incoming_ray, const Hit& hit, Vector3& attenuation, Ray& outgoing_ray) {
    switch(material.type) {
        case LAMBERTIAN:
            return lambertian_scatter(material, incoming_ray, hit, attenuation, outgoing_ray);
        case METAL:
            return metal_scater(material, incoming_ray, hit, attenuation, outgoing_ray);
        case DIELECTRIC:
            return dielectric_scater(material, incoming_ray, hit, attenuation, outgoing_ray);
        default:
            break;
    }
    return false;
}