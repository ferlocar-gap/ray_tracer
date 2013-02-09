#ifndef SCENE_CONFIGURATION_H
#define SCENE_CONFIGURATION_H

#include "tracing/vector.h"
#include "tracing/window.h"
#include "tracing/color.h"
#include "tracing/object.h"
#include "tracing/light.h"
#include "tracing/cached_ray.h"

/*
 * Holds all the high-level configuration of the scene that will be drawn.
 *
 * window: Window through which the scene universe is seen. This will be mapped to the final image.
 * eye: Position of the eye in the scene. Rays are thrown from here to the window.
 * background: Color returned for a ray when no intersection was found.
 * objs: List of objects in the scene.
 * objs_length: Number of objects in the scene.
 * lights: List of lights in the scene.
 * lights_length: Number of lights in the scene.
 * environment_light: Color of the light that affects the whole scene.
 * max_mirror_level: Maximum number of reflections that can exist in an object intersection.
 * max_antialiase_level: Maximum number of pixel divisions that can exist for the adaptive antialiasing.
 * ray_cache: Cache for ray colors. The size of the cache increases dinamically according to the width of the image,
 *            and the maximum antialiasing level.
 * pixel_density: Number of subpixels that exist on a pixel. This is calculated according to the max_antialiase_level.
 * row_ray_count: Number of rays per PIXEL ROW. Note that this IS NOT the number of rays per image row. This is
 *                calculated according to the pixel_density and the width of the image.
 * cache_size: Size of the ray_cache.
 * current_row: Number of the image row that is being processed by the ray tracer. Used to optimize the ray cache.
 * max_transparency_level: Maximum number of objects that are considered for the color of a ray due to transparency.
 * width_res: Width resolution of the generated image.
 * height_res: Height resolution of the generated image.
 */
typedef struct
{
	Window window;
    Vector eye;
    Color background;
    Object *objs;
    int objs_length;
    Light *lights;
    int lights_length;
    Color environment_light;
    int max_mirror_level ;
    int max_antialiase_level;
    CachedRay *ray_cache;
    int pixel_density;
    int row_ray_count;
    int cache_size;
    int current_row;
    int max_transparency_level;
    int width_res;
    int height_res;
} SceneConfig;

#endif
