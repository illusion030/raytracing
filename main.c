#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include "primitives.h"
#include "raytracing.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512

#define OUT_FILE "8threads_rows_opt.txt"

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
{
    int i;
    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    color background = { 0.0, 0.1, 0.1 };
    struct timespec start, end;
    double exec_time;

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t thread[8];
    details *detail[8];

    for(i = 0; i < 8; i++)
        detail[i] = malloc(sizeof(details)),

                    insert_detail(pixels, background, rectangulars, spheres,
                                  lights, &view, ROWS, COLS, 0, ROWS / 4,
                                  0, COLS, detail[0]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS / 4,
                  ROWS / 3, 0, COLS, detail[1]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS / 3,
                  ROWS * 5 / 12, 0, COLS, detail[2]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS * 5 / 12,
                  ROWS / 2, 0, COLS, detail[3]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS / 2,
                  ROWS * 7 / 12, 0, COLS, detail[4]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS * 7 / 12,
                  ROWS * 2 / 3, 0, COLS, detail[5]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS * 2 / 3,
                  ROWS * 3 / 4, 0, COLS, detail[6]);
    insert_detail(pixels, background, rectangulars, spheres,
                  lights, &view, ROWS, COLS, ROWS * 3 / 4, ROWS,
                  0, COLS, detail[7]);

    for(i = 0; i < 8; i++)
        pthread_create(&thread[i], NULL, &raytracing_thread,
                       (void *)detail[i]);

    for(i = 0; i < 8; i++) {
        pthread_join(thread[i], NULL);
        free(detail[i]);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_rectangular_list(&rectangulars);
    delete_sphere_list(&spheres);
    delete_light_list(&lights);
    free(pixels);

    exec_time = diff_in_second(start, end);

    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", exec_time);

    FILE *output = fopen(OUT_FILE, "a");
    fprintf(output, "time %lf\n", exec_time);
    fclose(output);

    return 0;
}
