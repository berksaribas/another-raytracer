#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "maths.h"
#include "utils.h"
#include "shapes.h"
#include "material.h"
#include <stdint.h>

void run_raytracer(const int image_width, const int image_height, uint8_t* image_data);
void create_raytracing_context(const int image_width, const int image_height);

#endif