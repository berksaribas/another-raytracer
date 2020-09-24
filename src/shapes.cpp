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