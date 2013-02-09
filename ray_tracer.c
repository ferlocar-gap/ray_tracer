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
#include "scene_config.h"
#include "utilities/memory_handler.h"
#include "utilities/error_handler.h"
#include "utilities/file_handler.h"
#include "loading/scene_loader.h"
#include "tracing/color.h"
#include "tracing/window.h"
#include "tracing/vector.h"
#include "tracing/object.h"
#include "tracing/light_f.h"
#include "tracing/intersection.h"
#include "tracing/cached_ray.h"

// Global Variables
int g_current_row;

/*
 * Returns the color found by a ray thrown from the eye towards a coordinate
 * from the scene window.
 *
 * w_coord: Horizontal coordinate of the scene window.
 * h_coord: Vertical coordinate of the scene window.
 * conf: Configuration of the scene.
 */
Color get_ray_color(long double w_coord, long double h_coord, SceneConfig conf)
{
    int w_cache, h_cache, cache_index;
    CachedRay cached_ray;
    long double x_window, y_window, z_window;
    Vector dir_vec;
    // Get cached ray according to given coordinates
    w_cache = w_coord * conf.pixel_density;
    h_cache = (h_coord - g_current_row) * conf.pixel_density * conf.row_ray_count;
    cache_index = w_cache + h_cache;
    cached_ray = conf.ray_cache[cache_index];
    if(h_cache == 0 && cached_ray.row > -1)
    {
        cached_ray = conf.ray_cache[w_cache + conf.pixel_density * conf.row_ray_count];
        cached_ray.row = g_current_row;
    }
    // Check if we already know the color for this ray
    if(cached_ray.row < g_current_row)
    {
        // Map the framebuffer position to universal coordinates
        x_window = conf.window.x_min + ((w_coord * (conf.window.x_max - conf.window.x_min)) / conf.width_res);
        y_window = conf.window.y_min + ((h_coord * (conf.window.y_max - conf.window.y_min)) / conf.height_res);
        z_window = conf.window.z_anchor;
        // Get the ray vector for the current pixel
        dir_vec.x = x_window - conf.eye.x;
        dir_vec.y = y_window - conf.eye.y;
        dir_vec.z = z_window - conf.eye.z;
        normalize_vector(&dir_vec);
        // We save the color of the pixel
        cached_ray.color = get_color(conf.eye, dir_vec, 0, conf);
        cached_ray.row = g_current_row;
    }
    conf.ray_cache[cache_index] = cached_ray;
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
 * conf: Configuration of the scene.
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
 * level: Antialiasing level. It indicates the size of the pixel/subpixel.
 *          Level 1 -> Normal sized pixel
 *          Level 2 -> Subpixel, one half the size and width of a normal pixel
 *          Level 3 -> Subpixel, one quarter the size and width of a normal pixel
 *          Level 4 -> Subpixel, one eigth the size and width of a normal pixel
 *          etc...
 * conf: Configuration of the scene.
 */
Color get_pixel_color(long double w_coord, long double h_coord, int level, SceneConfig conf)
{
    Color colors[4];
    Color avg_color;
    long double vertex_diff, sub_pixel_diff;

    vertex_diff = 1.0 / level;
    // Throw a ray for all vertex of the pixel
    colors[0] = get_ray_color(w_coord, h_coord, conf);
    colors[1] = get_ray_color(w_coord + vertex_diff, h_coord, conf);
    colors[2] = get_ray_color(w_coord, h_coord + vertex_diff, conf);
    colors[3] = get_ray_color(w_coord + vertex_diff, h_coord + vertex_diff, conf);
    avg_color = get_avg_color(colors);
    // Check if we have reached the max antialiasing level
    if(level + 1 > conf.max_antialiase_level) return avg_color;
    // Antialiase
    sub_pixel_diff = vertex_diff / 2.0;
    if(are_colors_too_different(colors[0], avg_color))
    {
        colors[0] = get_pixel_color(w_coord, h_coord, level+1, conf);
    }
    if(are_colors_too_different(colors[1], avg_color))
    {
        colors[1] = get_pixel_color(w_coord + sub_pixel_diff, h_coord, level+1, conf);
    }
    if(are_colors_too_different(colors[2], avg_color))
    {
        colors[2] = get_pixel_color(w_coord, h_coord + sub_pixel_diff, level+1, conf);
    }
    if(are_colors_too_different(colors[3], avg_color))
    {
        colors[3] = get_pixel_color(w_coord + sub_pixel_diff, h_coord + sub_pixel_diff, level+1, conf);
    }
    return get_avg_color(colors);
}

/*
 * It paints the ray tracer scene and stores it in a .bmp image with a
 * resolution of width_res * height_res. Scene environment and
 * objects should be initialized before calling this method, by calling the
 * 'load_scene' method.
 *
 * conf: Configuration of the scene.
 */
void paint_scene(SceneConfig conf)
{
	int w_index, h_index;
	Color *image, *image_back_up;

	image = get_memory(sizeof(Color) * conf.width_res * conf.height_res, NULL);
	image_back_up = image;
	g_current_row = 0;
	// Calculate the color of each pixel of the framebuffer
	for(h_index = 0; h_index < conf.height_res; h_index++)
    {
        for(w_index = 0; w_index < conf.width_res; w_index++)
		{
			*(image++) = get_pixel_color(w_index, h_index, 1, conf);
		}
		g_current_row++;
    }
    create_image(image_back_up, conf.height_res, conf.width_res);
    free(image_back_up);
}

int main(int argc, char** argv)
{
	SceneConfig scene_config = load_scene("scene.cfg");
	paint_scene(scene_config);
	return 0;
}
