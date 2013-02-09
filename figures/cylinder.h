#ifndef CYLINDER_H
#define CYLINDER_H

#include "../tracing/vector.h"
#include "../tracing/intersection.h"

/*
 * Represents a cylinder object
 *
 * direction: Axis where the cylinder is located.
 * anchor: Anchor from which the cylinder's back and front emerge.
 * radius: Radius of the cylinder.
 * is_finite: False if the cylinder continues indefinitely. True if it has a
 *            front length and a back length.
 * front_length: Length at which the cylinder spreads towards the 'direction'
 *               direction. It's normally positive.
 * back_length: Length at which the cylinder spreads towards the 'direction'
 *              inverse direction. It's normally negative.
 */
typedef struct
{
	Vector direction;
	Vector anchor;
	long double radius;
	int is_finite;
	long double front_length;
	long double back_length;
} Cylinder;

Intersection* get_cyl_cone_intersection(
    long double a, long double b, long double c,
    Vector eye, Vector dir_vec, void* object_ptr, int *length);
void* get_cylinder_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length);
Vector get_cylinder_normal_vector(Vector posn, void* cylinder_ptr);

#endif
