#include "shapes.h"

inline void set_face_normal(Hit& hit, const Ray& ray, const Vector3& outward_normal) {
    hit.front_face = dot(ray.direction, outward_normal) < 0;
    hit.normal = hit.front_face ? outward_normal :-outward_normal;
}

bool sphere_hit(const Sphere& sphere, const Ray& ray, float t_min, float t_max, Hit& hit) {
    auto diff = ray.origin_point - sphere.center;

    auto a = dot(ray.direction, ray.direction);
    auto b = dot(diff, ray.direction);
    auto c = dot(diff, diff) - sphere.radius * sphere.radius;
    
    auto discriminant = b*b - a*c;

    if(discriminant > 0) {
        auto discriminant_sqrt = sqrt(discriminant);
        auto first_root = (-b - discriminant_sqrt) / a;

        if(first_root > t_min && first_root < t_max) {
            hit.t = first_root;
            hit.point = ray_at(ray, hit.t);
            auto outward_normal = (hit.point - sphere.center) / sphere.radius;
            set_face_normal(hit, ray, outward_normal);
            return true;
        }

        auto second_root = (-b + discriminant_sqrt) / a;
        if(second_root > t_min && second_root < t_max) {
            hit.t = second_root;
            hit.point = ray_at(ray, hit.t);
            auto outward_normal = (hit.point - sphere.center) / sphere.radius;
            set_face_normal(hit, ray, outward_normal);
            return true;
        }
    }
    return false;
}

bool triangle_hit(const Triangle& triangle, const Ray& ray, float t_min, float t_max, Hit& hit) {
    const float EPSILON = 0.0000001;
    Vector3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = triangle.v1 - triangle.v0;
    edge2 = triangle.v2 - triangle.v0;
    h =  cross(ray.direction, edge2);
    a = dot(edge1, h);
    
    if(a > -EPSILON && a < EPSILON) {
        return false;
    }

    f = 1.0f / a;
    s = ray.origin_point - triangle.v0;
    u = f * dot(s, h);

    if(u < 0 || u > 1.0) {
        return false;
    }

    q = cross(s,edge1);
    v = f * dot(ray.direction, q);

    if(v < 0 || u + v > 1) {
        return false;
    }

    float t = f * dot(edge2, q);

    if(t > EPSILON) {
        hit.t = t;
        hit.point = ray_at(ray, hit.t);
        return true;
    }

    return false;
}