/* light.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all functions that calculate light intensity for the scene.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "color.h"
#include "light.h"
#include "vector.h"
#include "intersection.h"
#include "object.h"

extern Light* g_lights;
extern int g_lights_length;
extern long double g_environment_light;
extern Color g_background;
extern int g_max_mirror_level;
extern int g_max_transparency_level;

// Methods

/*
 * Adds two RGB colors.
 *
 * color1: Original color.
 * color2. Color that we are adding to the original color.
 */
Color add_colors(Color color1, Color color2)
{
    color1.red += color2.red;
    color1.green += color2.green;
    color1.blue += color2.blue;
    // Make overflow validation
    if (color1.red > 1.0) color1.red = 1.0;
    if (color1.green > 1.0) color1.green = 1.0;
    if (color1.blue > 1.0) color1.blue = 1.0;
    return color1;
}

/*
 * Multiplies an RGB color by a scalar value.
 *
 * value: Scalar value by which the color is being multiplied
 * color: Color being multiplied.
 */
Color multiply_color(long double value, Color color)
{
    color.red *= value;
    color.green *= value;
    color.blue *= value;
    return color;
}

/*
 * Returns the normal vector of an intersection. The normal vector is already
 * normalized.
 *
 * inter: Pointer to the intersection for which the normal vector is calculated.
 */
Vector get_normal_vector(Intersection* inter)
{
	Object obj = inter->obj;
	return inter->obj.get_normal_vector(inter->posn, obj.figure);
}

/*
 * Gives the attenuation factor of a light. It depends on the distance
 * from which the light is received.
 *
 * light: Light for which the attenuation factor is calculated.
 * distance: Distance between the light and an illuminated spot.
 */
long double get_attenuation_factor(Light light, long double distance)
{
	long double att_factor =    1 /(light.const_att_factor +
                                light.lin_att_factor * distance +
                                pow(light.expo_att_factor * distance, 2.0));
	if(att_factor > 1.0) return 1.0;
	else return att_factor;
}

/*
 * Gets the color of the intersection by calculating light intensity,
 * (which includes specular light, shadows, transparency, etc)
 * light: Light for which the attenuation factor is calculated.
 * distance: Distance between the light and an illuminated spot.
 * mirror_level: Current level of reflection.
 */
Color get_intersection_color(Vector eye,
                             Vector dir_vec,
                             Intersection *inter_list,
                             int inter_length,
                             int mirror_level,
                             int transparency_level)
{
    Intersection inter;
    int light_index;
    long double light_factor, light_distance, illum_cos, spec_light_factor,
                att_factor, spec_cos, mirror_factor, transparency_factor;
    Vector normal_vec, light_vec, rev_dir_vec, reflection_vec;
    Light light;
    Color color_found, reflection_color, transparency_color, final_color;

    inter = inter_list[transparency_level];
    // Light intensity
    light_factor = 0.0;
    // Specular light intensity
    spec_light_factor = 0.0;
    normal_vec = get_normal_vector(&inter);
    // Pick up the normal vector that is pointing to the eye
    if(do_dot_product(normal_vec, dir_vec) > 0)
        normal_vec = multiply_vector(-1, normal_vec);
    // Initialize reverse direction vector for mirrors and specular light
    rev_dir_vec = multiply_vector(-1, dir_vec);
    for(light_index = 0; light_index < g_lights_length; light_index++)
    {
        light = g_lights[light_index];
        // Find the vector that points from the intersection point to the light
        // source, and normalize it
        light_vec = subtract_vectors(light.anchor, inter.posn);
        light_distance = normalize_vector(&light_vec);
        // We check for any object making a shadow from that light
        Intersection* shadow_inter = get_intersections(inter.posn, light_vec, NULL);
        // If the intersection is beyond the light source, we ignore it
        if(shadow_inter && shadow_inter->distance >= light_distance)
        {   // Object(s) is/are actually behind the light
            free(shadow_inter);
            shadow_inter = NULL;
        }
        // If there aren't any shadows
        if(!shadow_inter)
        {
            illum_cos = do_dot_product(normal_vec, light_vec);
            // We only take it into account if the angle is lower than 90 degrees
            if(illum_cos > 0)
            {
                Vector light_mirror_vec = subtract_vectors(multiply_vector(2 * illum_cos, normal_vec), light_vec);
                // Attenuation factor, reduces the light energy depending on the distance
                att_factor = get_attenuation_factor(light, light_distance);
                spec_cos = do_dot_product(rev_dir_vec, light_mirror_vec);
                // We add the light source effect
                light_factor += illum_cos * light.intensity * inter.obj.light_material * att_factor;
                // The specular light, is the white stain on the objects
                if(spec_cos > 0)
                {
                    spec_light_factor += pow(spec_cos * inter.obj.specular_material * att_factor, inter.obj.specular_pow);
                }

            }
        }
        else free(shadow_inter);
    }
    // We add the environmental light of the scene
    light_factor += g_environment_light * inter.obj.light_ambiental;
    if(light_factor > 1.0)
        light_factor = 1.0;
    if(spec_light_factor > 1.0)
        spec_light_factor = 1.0;
    color_found = multiply_color(light_factor, inter.obj.color);
    // Specular light gives a color between enlightened color and the light color.
    color_found.red += (1 - color_found.red) * spec_light_factor;
    color_found.green += (1 - color_found.green) * spec_light_factor;
    color_found.blue += (1 - color_found.blue) * spec_light_factor;
    // Get transparency color
    transparency_factor = inter.obj.transparency_material;
    if (transparency_level < g_max_transparency_level &&
        transparency_factor > 0.0)
    {
        if(transparency_level + 1 < inter_length)
        {
            transparency_color = get_intersection_color(eye,dir_vec,inter_list,inter_length,0,transparency_level+1);
        }
        else
        {
            transparency_color = g_background;
        }
    }
    else
    {
        transparency_factor = 0.0;
        transparency_color = (Color){ .red = 0.0, .green = 0.0, .blue = 0.0 };
    }
    // Get reflection color
    mirror_factor = inter.obj.mirror_material;
    if (mirror_level < g_max_mirror_level &&
        mirror_factor > 0.0)
    {
        reflection_vec = subtract_vectors(multiply_vector(2 * do_dot_product(normal_vec, rev_dir_vec), normal_vec), rev_dir_vec);
        reflection_color = get_color(inter.posn, reflection_vec, mirror_level + 1);
    }
    else
    {
        mirror_factor = 0;
        reflection_color = (Color){ .red = 0.0, .green = 0.0, .blue = 0.0 };
    }
    // Calculate final color
    transparency_color = multiply_color(transparency_factor, transparency_color);
    reflection_color = multiply_color((1.0-transparency_factor) * mirror_factor, reflection_color);
    color_found = multiply_color((1.0-transparency_factor) * (1.0-mirror_factor), color_found);
    final_color = add_colors(transparency_color, add_colors(reflection_color, color_found));
    return final_color;
}

/*
 * Returns the color that is seen from the position 'eye' when looking at the
 * tridimensional scene towards the direction 'dir_vec'.
 *
 * eye: Position from which the scene is seen.
 * dir_vec: Direction at which the eye is looking. This vector must be normalized.
 * mirror_level: Current level of reflection.
 */
Color get_color(Vector eye, Vector dir_vec, int mirror_level)
{
	Intersection *inter_list;
	Color color;
	// Get intersections on the given direction. Intersections are ordered from the nearest to the farthest.
	int inter_list_length;
	inter_list = get_intersections(eye, dir_vec, &inter_list_length);
	// If we don't find an intersection we return the background, otherwise we check for the intersections's color.
	if (!inter_list) return g_background;
	color = get_intersection_color(eye, dir_vec, inter_list, inter_list_length, mirror_level, 0);
	free(inter_list);
	return color;
}
