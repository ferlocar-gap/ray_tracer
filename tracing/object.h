#ifndef OBJECT_H
#define OBJECT_H

#include "color.h"
#include "../figures/plane.h"

/*
 * Represents an object in the scene
 *
 * color: Color of the object.
 * figure: Pointer to the specific figure of the object. Possible figures:
 *      - Sphere
 * light_material: Factor for how much the material is affected by the light. Value between 0 and 1.
 * light_ambiental: Factor for how much the ambiental light affects the object. Value between 0 and 1.
 * specular_material: Factor for how much the material is affected by specular light. Value between 0 and 1.
 * mirror_material: Factor for how much the material reflects other objects. Value between 0 and 1.
 * transparency_material: Factor for how transparent is the material. Value between 0 and 1.
 * translucency_material: Factor for how much light can get pass through the object. Value between 0 and 1.
 * specular_pow: Factor for how big is the specular light spot. The larger the factor, the bigger the spot.
 * cutting_planes: Planes that cut the object.
 * cutting_planes_length: Number of planes that cut the object.
 * get_intersections: Calculates the intersections of a ray with the object.
 *      - First parameter: Position from which the ray is thrown.
 *      - Second parameter: Direction towards which the ray is thrown. Must be normalized.
 *      - Third parameter: A pointer to the object itself.
 *      - Fourth parameter: Outuput parameter for the number of intersections found.
 *      - Returns: The intersections, if any. Otherwise, it retuns NULL.
 * get_normal_vector: Returns the normal vector of object's figure on a given position.
 *      - First parameter: Position at which the normal is being calculated.
 *      - Second parameter: Pointer to the object's figure.
 *      - Returns: The normal vector (The vector is normalized).
 */
typedef struct
{
	Color color;
	void* figure;
	long double light_material;
	long double light_ambiental;
	long double specular_material;
	long double mirror_material;
	long double transparency_material;
	long double translucency_material;
	long double specular_pow;
	Plane *cutting_planes;
	int cutting_planes_length;
	void* (*get_intersections) (Vector, Vector, void*, int*);
	Vector (*get_normal_vector) (Vector, void*);
} Object;

#endif
