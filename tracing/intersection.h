#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "vector.h"
#include "object.h"

/*
 * Represents an intersection with a scene object.
 *
 * distance: Distance at which the intersection is from the eye.
 * posn: Position at which the intersection was made.
 * obj: Object with which the intersection was made.
 * is_valid: True if the intersection can be used. Some intersections are
 *           rendered invalid because they are cut by a cutting plane.
 */
typedef struct
{
	long double distance;
	Vector posn;
	Object obj;
	int is_valid;
} Intersection;

Intersection* get_intersections(Vector eye, Vector dir_vec, int* length);

#endif
