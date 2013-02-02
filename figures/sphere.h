#ifndef SPHERE_H
#define SPHERE_H

#include "../tracing/vector.h"

/*
 * Represents a sphere object
 *
 * radius: Radius of the sphere.
 * center: Position of the sphere of the sphere.
 */
typedef struct
{
	long double radius;
	Vector center;
} Sphere;

void* get_sphere_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length);
Vector get_sphere_normal_vector(Vector posn, void* sphere_ptr);

#endif
