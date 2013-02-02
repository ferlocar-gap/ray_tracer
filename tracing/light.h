#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include "color.h"
#include "intersection.h"

/*
 * Represents a light source on the scene.
 *
 * intensity: Factor for the light brightness. Must be between 0 and 1.
 * color: Color of the light.
 * anchor: Position of the light.
 * const_att_factor: Constant distance attenuation factor.
 * lin_att_factor: Lineal distance attenuation factor.
 * expo_att_factor: Exponential distance attenuation factor.
 */
typedef struct
{
	long double intensity;
    Color color;
	Vector anchor;
	long double const_att_factor;
	long double lin_att_factor;
	long double expo_att_factor;
} Light;

Color get_intersection_color(Vector eye, Vector dir_vec, Intersection *inter);

#endif
