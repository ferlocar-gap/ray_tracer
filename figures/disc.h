#ifndef DISC_H
#define DISC_H

#include "../tracing/vector.h"
#include "plane.h"

/*
 * Represents a disc object
 *
 * plane: Plane in which the disc is located.
 * center: Center of the disc.
 * inner_focus1: First focus point that defines the elipsis hole.
 * inner_focus2: Second focus point that defines the elipsis hole.
 * inner_dist: Maximum distance at which any point within the elipsis hole can exist.
 * ext_focus1: First focus point that defines the disc elipsis.
 * ext_focus2: Second focus point that defines the disc elipsis.
 * ext_dist: Maximum distance at which any point within the disc elipsis can exist.
 */
typedef struct
{
	Plane plane;
	Vector inner_focus1;
	Vector inner_focus2;
	long double inner_dist;
	Vector ext_focus1;
	Vector ext_focus2;
	long double ext_dist;
} Disc;

void* get_disc_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length);
Vector get_disc_normal_vector(Vector posn, void* disc_ptr);

#endif
