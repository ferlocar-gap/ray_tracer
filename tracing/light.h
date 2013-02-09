#ifndef LIGHT_H
#define LIGHT_H

#include "../scene_config.h"
#include "vector.h"
#include "color.h"

/*
 * Represents a light source on the scene.
 *
 * color: Color of the light. It works as the intensity as well.
 * anchor: Position of the light.
 * const_att_factor: Constant distance attenuation factor.
 * lin_att_factor: Lineal distance attenuation factor.
 * expo_att_factor: Exponential distance attenuation factor.
 */
typedef struct
{
    Color color;
	Vector anchor;
	long double const_att_factor;
	long double lin_att_factor;
	long double expo_att_factor;
} Light;

#endif
