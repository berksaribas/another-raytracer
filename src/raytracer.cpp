#include "raytracer.h"
#include <vector>

Vector3* previous_render;
int render_run = 1;

Vector3 random_in_unit_disk() {
    while (true) {
        auto p = Vector3(random_float(-1,1), random_float(-1,1), 0);
        if (dot(p, p) >= 1) continue;
        return p;
    }
}

Ray get_camera_ray(const Camera& cam, float u, float v) { 
    Vector3 rd = random_in_unit_disk() * cam.lens_radius;
    Vector3 offset = cam.u * rd.x + cam.v * rd.y;

    return Ray(cam.origin + offset, cam.lower_left_corner + cam.horizontal * u + cam.vertical * v - cam.origin - offset); 
}

bool check_object_hit(const std::vector<Sphere>& objects, const Ray& ray, float t_min, float t_max, Hit& hit) {
    Hit closest_hit;
    bool has_hit = false;
    auto closest_hit_distance = t_max;
    auto selected_index = -1;

    for(int i = 0; i < objects.size(); i++) {
        const auto& object = objects[i];
        if(sphere_hit(object, ray, t_min, closest_hit_distance, closest_hit)) {
            has_hit = true;
            closest_hit_distance = closest_hit.t;
            selected_index = i;
        }
    }

    if(has_hit) {
        hit = closest_hit;
        hit.object_index = selected_index;
    }
    
    return has_hit;
}

Vector3 trace_ray(const Ray& ray, const std::vector<Sphere>& objects, const std::vector<Material> materials, int depth) {
    if (depth <= 0) {
        return Vector3(0, 0, 0);
    }

    Hit hit;
    if(check_object_hit(objects, ray, 0.001f, FLT_MAX, hit)) {
        Ray outgoing_ray;
        Vector3 attenuation;
        if(scatter(materials[hit.object_index], ray, hit, attenuation, outgoing_ray)) {
            auto ray_color = trace_ray(outgoing_ray, objects, materials, depth - 1);
            return Vector3(ray_color.x * attenuation.x, ray_color.y * attenuation.y, ray_color.z * attenuation.z);
        }
        return Vector3(0, 0, 0);
    }

    auto t = 0.5f * (unit_vector(ray.direction).y + 1.0f);
    return lerp(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.5f, 0.7f, 1.0f), t);
}

void run_raytracer(const int image_width, const int image_height, uint8_t* image_data) {
    const int samples_per_pixel = 16;
    const auto aspect_ratio = (float) image_width / image_height;
    Camera camera(Vector3(0,1,1), Vector3(0,0,-1), Vector3(0,1,0), aspect_ratio, 90, 0);

    // Scene setup
    std::vector<Sphere> objects;
    std::vector<Material> materials;

    Material mat_lambert_yellowish;
    mat_lambert_yellowish.albedo = Vector3(0.8f, 0.8f, 0.0f);
    mat_lambert_yellowish.type = MaterialType::LAMBERTIAN;

    Material mat_lambert_reddish;
    mat_lambert_reddish.albedo = Vector3(0.7f, 0.3f, 0.3f);
    mat_lambert_reddish.type = MaterialType::LAMBERTIAN;

    Material mat_lambert_greenish;
    mat_lambert_greenish.albedo = Vector3(0.3f, 0.7f, 0.3f);
    mat_lambert_greenish.type = MaterialType::LAMBERTIAN;

    Material mat_lambert_bluish;
    mat_lambert_bluish.albedo = Vector3(0.3f, 0.3f, 0.7f);
    mat_lambert_bluish.type = MaterialType::LAMBERTIAN;

    Material mat_mirror;
    mat_mirror.albedo = Vector3(0.8f, 0.8f, 0.8f);
    mat_mirror.type = MaterialType::METAL;
    mat_mirror.fuzziness = 0.0f;

    Material mat_less_fuzzy_metal;
    mat_less_fuzzy_metal.albedo = Vector3(0.8f, 0.8f, 0.8f);
    mat_less_fuzzy_metal.type = MaterialType::METAL;
    mat_less_fuzzy_metal.fuzziness = 0.3f;

    Material mat_less_fuzzy_pink_metal;
    mat_less_fuzzy_pink_metal.albedo = Vector3(0.6f, 0.0f, 0.3f);
    mat_less_fuzzy_pink_metal.type = MaterialType::METAL;
    mat_less_fuzzy_pink_metal.fuzziness = 0.5f;

    Material mat_fuzzy_metal;
    mat_fuzzy_metal.albedo = Vector3(0.3f, 0.6f, 0.8f);
    mat_fuzzy_metal.type = MaterialType::METAL;
    mat_fuzzy_metal.fuzziness = 0.7f;

    objects.push_back(Sphere(Vector3(0,-100.5f,-1), 100));
    materials.push_back(mat_mirror);

    objects.push_back(Sphere(Vector3(0,0,-1), 0.5f));
    materials.push_back(mat_mirror);

    objects.push_back(Sphere(Vector3(0,1,-1), 0.5f));
    materials.push_back(mat_mirror);

    objects.push_back(Sphere(Vector3(-1.2f,0,-1.5f), 0.5f));
    materials.push_back(mat_lambert_yellowish);

    objects.push_back(Sphere(Vector3(1.2f,0,-1), 0.5f));
    materials.push_back(mat_lambert_reddish);

    objects.push_back(Sphere(Vector3(0.5f,-0.2,0), 0.3f));
    materials.push_back(mat_lambert_greenish);

    objects.push_back(Sphere(Vector3(1.4,-0.2,0), 0.3));
    materials.push_back(mat_fuzzy_metal);

    objects.push_back(Sphere(Vector3(-0.8f,-0.2,-0.2), 0.3f));
    materials.push_back(mat_lambert_bluish);

    objects.push_back(Sphere(Vector3(-0.2f,-0.4,0), 0.1f));
    materials.push_back(mat_less_fuzzy_pink_metal);
    
    // Render loop
    int counter = 0;
    
    #pragma omp parallel for schedule(dynamic) //num_threads(12) //for now, num thread is not really required
    for(int y = image_height - 1; y >= 0; y--) {
        for(int x = 0; x < image_width; x++) {
            auto& color = previous_render[y * image_width + x];

            auto u = (float) (x + random_float()) / (image_width - 1);
            auto v = (float) (y + random_float()) / (image_height - 1);
            auto r = get_camera_ray(camera, u, v);
            color += trace_ray(r, objects, materials, 50);
            
            Vector3 write_color(0, 0, 0);

            auto scale = 1.0f / render_run;
            write_color.x = clamp(sqrt(scale * color.x), 0.0f, 0.999f);
            write_color.y = clamp(sqrt(scale * color.y), 0.0f, 0.999f);
            write_color.z = clamp(sqrt(scale * color.z), 0.0f, 0.999f);

            int pos = ((image_height - 1 - y) * image_width + x) * 4;
            image_data[pos] = static_cast<uint8_t>(write_color.z * 256);
            image_data[pos + 1] = static_cast<uint8_t>(write_color.y * 256);
            image_data[pos + 2] = static_cast<uint8_t>(write_color.x * 256);
        }
    }

    render_run ++;
    //save_image("test.ppm", image_width, image_height, image_data);
}

void create_raytracing_context(const int image_width, const int image_height) {
    previous_render = static_cast<Vector3*>(malloc(image_width * image_height * sizeof(Vector3)));
    memset(previous_render, 0, image_width * image_height * sizeof(Vector3));
}