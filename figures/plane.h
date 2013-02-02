#ifndef PLANE_H
#define PLANE_H

#include "../tracing/vector.h"

/*
 * Represents a plane object
 *
 * direction: Direction at which the plane is looking.
 * offset: Offset in the direction at which the plane is looking
 */
// Structure for a sphere object
typedef struct
{
	Vector direction;
	long double offset;
} Plane;

int is_up_the_plane(Vector posn, Plane plane);
void* get_plane_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length);
Vector get_plane_normal_vector(Vector posn, void* plane_ptr);

#endif
