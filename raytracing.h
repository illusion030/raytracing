#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

typedef struct __RAYTRACING_DETAILS {
    uint8_t *pixels;
    color background_color;
    rectangular_node rectangulars;
    sphere_node spheres;
    light_node lights;
    const viewpoint *view;
    int width_start;
    int height_start;
    int width_end;
    int height_end;
    int width;
    int height;
} details;

void raytracing(uint8_t *pixels, color background_color,
                rectangular_node rectangulars, sphere_node spheres,
                light_node lights, const viewpoint *view,
                int width, int height);

void *raytracing_thread(void *agrs);
void insert_detail(uint8_t *pixels, color background_color,
                   rectangular_node rectangulars,
                   sphere_node spheres,
                   light_node lights, const viewpoint *view,
                   int width, int height, int width_start,
                   int width_end, int height_start, int height_end,
                   details *d);
#endif
