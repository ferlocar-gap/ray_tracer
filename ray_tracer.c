/* ray_tracer.c
 *		Copyright 2010 Carlos Fernandez <cfernandez@TOSHI>
 *
 * Programming Standard:
 * Method names: All with lower case, separate words with underscore
 * Global varialbes: Start with 'g_', all with upper case, separate words with underscore
 * Local variables: All with lower case, separate words with underscore
 * Constants: All with upper case, separate words with underscore
 * Structures: All words with lower case, separate words with underscore
 * Structure attributes: All words with lower case, separate words with underscore
 *
 * RayTracer:
 * This program obtains the scene information and draws an image with that. It has
 * the general logic of the ray tracer.
 */

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utilities/memory_handler.h"
#include "utilities/error_handler.h"
#include "utilities/file_handler.h"
#include "loading/scene_loader.h"
#include "tracing/color.h"
#include "tracing/window.h"
#include "tracing/vector.h"
#include "tracing/object.h"
#include "tracing/light.h"
#include "tracing/intersection.h"
#include "tracing/cached_ray.h"

// Global Variables
Window g_window;
Vector g_eye;
Color g_background;
Object *g_objs;
int g_objs_length;
Light *g_lights;
int g_lights_length;
long double g_environment_light;
int g_max_mirror_level = 1;
int g_max_antialiase_level = 2;
CachedRay *g_ray_cache;
int g_pixel_density;
int g_row_ray_count;
int g_cache_size;
int g_current_row;
int g_max_transparency_level = 1;

/*
 * Returns the color that is seen from the position 'eye' when looking at the
 * tridimensional scene towards the direction 'dir_vec'.
 *
 * eye: Position from which the scene is seen.
 * dir_vec: Direction at which the eye is looking. This vector must be normalized.
 */
Color get_ray_color(long double w_coord, long double h_coord, int width_res, int height_res)
{
    int w_cache, h_cache, cache_index;
    CachedRay cached_ray;
    long double x_window, y_window, z_window;
    Vector dir_vec;
    // Get cached ray according to given coordinates
    w_cache = w_coord * g_pixel_density;
    h_cache = (h_coord - g_current_row) * g_pixel_density * g_row_ray_count;
    cache_index = w_cache + h_cache;
    cached_ray = g_ray_cache[cache_index];
    if(h_cache == 0 && cached_ray.row > -1)
    {
        cached_ray = g_ray_cache[w_cache + g_pixel_density * g_row_ray_count];
        cached_ray.row = g_current_row;
    }
    // Check if we already know the color for this ray
    if(cached_ray.row < g_current_row)
    {
        // Map the framebuffer position to universal coordinates
        x_window = g_window.x_min + ((w_coord * (g_window.x_max - g_window.x_min)) / width_res);
        y_window = g_window.y_min + ((h_coord * (g_window.y_max - g_window.y_min)) / height_res);
        z_window = g_window.z_anchor;
        // Get the ray vector for the current pixel
        dir_vec.x = x_window - g_eye.x;
        dir_vec.y = y_window - g_eye.y;
        dir_vec.z = z_window - g_eye.z;
        normalize_vector(&dir_vec);
        // We save the color of the pixel
        cached_ray.color = get_color(g_eye, dir_vec, 0);
        cached_ray.row = g_current_row;
    }
    g_ray_cache[cache_index] = cached_ray;
    return cached_ray.color;
}

/*
 * Returns the average color of an array of four colors.
 *
 * ray_colors: Array that contains four different colors.
 */
Color get_avg_color(Color *ray_colors)
{
    Color avg_color;
    int ray_i;

    avg_color.red = 0;
    for(ray_i = 0; ray_i < 4; ray_i++)
        avg_color.red += ray_colors[ray_i].red;
    avg_color.red /= 4.0;
    avg_color.green = 0;
    for(ray_i = 0; ray_i < 4; ray_i++)
        avg_color.green += ray_colors[ray_i].green;
    avg_color.green /= 4.0;
    avg_color.blue = 0;
    for(ray_i = 0; ray_i < 4; ray_i++)
        avg_color.blue += ray_colors[ray_i].blue;
    avg_color.blue /= 4.0;
    return avg_color;
}

/*
 * Returns true if the two given colors are not equal. This function is used to
 * determine if it is needed to go a level deeper for the antialiasing.
 * ToDo: Add a new parameter to configure the 'too_different' crieteria.
 *
 * color1: First color that will be compared.
 * color2: Second color that will be compared.
 */
int are_colors_too_different(Color color1, Color color2)
{
    return color1.red != color2.red || color1.green != color2.green || color1.blue != color2.blue;
}

/*
 * Returns the color of a pixel/subpixel. The pixel/subpixel location is given
 * by w_coord and h_coord, which indicate the upper left corner of the pixel.
 * The size of the pixel/subpixel varies according to the given antialiasing
 * level. The higher the level, the smaller the pixel.
 *
 * w_coord: Horizontal coordinate of the pixel in the image.
 * h_coord: Vertical coordinate of the pixel in the image.
 * width_res: Width in pixels of the image.
 * height_res: Height in pixels of the image.
 * level: Antialiasing level. It indicates the size of the pixel/subpixel.
 *          Level 1 -> Normal sized pixel
 *          Level 2 -> Subpixel, one half the size and width of a normal pixel
 *          Level 3 -> Subpixel, one quarter the size and width of a normal pixel
 *          Level 4 -> Subpixel, one eigth the size and width of a normal pixel
 *          etc...
 */
Color get_pixel_color(long double w_coord, long double h_coord, int width_res, int height_res, int level)
{
    Color colors[4];
    Color avg_color;
    long double vertex_diff, sub_pixel_diff;

    vertex_diff = 1.0 / level;
    // Throw a ray for all vertex of the pixel
    colors[0] = get_ray_color(w_coord, h_coord, width_res, height_res);
    colors[1] = get_ray_color(w_coord + vertex_diff, h_coord, width_res, height_res);
    colors[2] = get_ray_color(w_coord, h_coord + vertex_diff, width_res, height_res);
    colors[3] = get_ray_color(w_coord + vertex_diff, h_coord + vertex_diff, width_res, height_res);
    avg_color = get_avg_color(colors);
    // Check if we have reached the max antialiasing level
    if(level + 1 > g_max_antialiase_level) return avg_color;
    // Antialiase
    sub_pixel_diff = vertex_diff / 2.0;
    if(are_colors_too_different(colors[0], avg_color))
    {
        colors[0] = get_pixel_color(w_coord, h_coord, width_res, height_res, level+1);
    }
    if(are_colors_too_different(colors[1], avg_color))
    {
        colors[1] = get_pixel_color(w_coord + sub_pixel_diff, h_coord, width_res, height_res, level+1);
    }
    if(are_colors_too_different(colors[2], avg_color))
    {
        colors[2] = get_pixel_color(w_coord, h_coord + sub_pixel_diff, width_res, height_res, level+1);
    }
    if(are_colors_too_different(colors[3], avg_color))
    {
        colors[3] = get_pixel_color(w_coord + sub_pixel_diff, h_coord + sub_pixel_diff, width_res, height_res, level+1);
    }
    return get_avg_color(colors);
}

/*
 * It paints the ray tracer scene and stores it in a .bmp image with a
 * resolution of width_res * height_res. Scene environment and
 * objects should be initialized before calling this method, by calling the
 * 'load_scene' method.
 *
 * width_res: Horizontal resolution of the image.
 * height_res: Vertical resolution of the image.
 */
void paint_scene(int width_res, int height_res)
{
	int w_index, h_index, cache_i;
	Color *image, *image_back_up;

	g_pixel_density = pow(2, g_max_antialiase_level - 1);
	g_row_ray_count = (width_res * g_pixel_density) + 1;
	g_cache_size = (g_pixel_density + 1) * g_row_ray_count;
	g_ray_cache = get_memory(sizeof(CachedRay) * g_cache_size, NULL);
	// Initialize cache
	for(cache_i = 0; cache_i < g_cache_size; cache_i++)
        g_ray_cache[cache_i].row = -1;

	image = get_memory(sizeof(Color) * width_res * height_res, NULL);
	image_back_up = image;
	g_current_row = 0;
	// Calculate the color of each pixel of the framebuffer
	for(h_index = 0; h_index < height_res; h_index++)
    {
        for(w_index = 0; w_index < width_res; w_index++)
		{
			*(image++) = get_pixel_color(w_index, h_index, width_res, height_res, 1);
		}
		g_current_row++;
    }
    create_image(image_back_up, height_res, width_res);
    free(image_back_up);
    free(g_ray_cache);
}

int main(int argc, char** argv)
{
	load_scene("scene.cfg");
	paint_scene(800, 800);
	return 0;
}
