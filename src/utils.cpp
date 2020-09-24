#include "utils.h"

void save_image(char* filename, int width, int height, int* image_data) {
    FILE* file;
    fopen_s(&file, filename, "w+");

    fprintf(file, "P3\n%d %d\n255\n", width, height);

    int pixel_count = width * height * 3;
    for(int i = 0; i < pixel_count; i+=3) {
        fprintf(file, "%d %d %d\n", image_data[i], image_data[i + 1], image_data[i + 2]);
    }

    fclose(file);
}