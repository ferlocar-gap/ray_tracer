/* disc.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all the disc object functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "../tracing/vector.h"
#include "../tracing/object.h"
#include "../tracing/intersection.h"
#include "plane.h"
#include "disc.h"

/*
 * Returns true if a point is inside an elipsis defined by 2 focus points and a
 * maximum distance at which any point within the elipsis can exist.
 *
 * posn: Position that is being checked if is inside the elipsis/disc.
 * focus1: First focus point that defines the elipsis.
 * focus2: Second focus point that defines the elipsis.
 * max_dist: Maximum distance at which any point within the elipsis can exist.
 */
int is_inside_disc(Vector posn, Vector focus1, Vector focus2, long double max_dist)
{
    Vector focus1_vector = subtract_vectors(posn, focus1);
    Vector focus2_vector = subtract_vectors(posn, focus2);
    long double focus1_distance = normalize_vector(&focus1_vector);
    long double focus2_distance = normalize_vector(&focus2_vector);
    return (max_dist > focus1_distance + focus2_distance);
}

/*
 * Returns a pointer to the intersection between a disc and a ray.
 *
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized.
 * object_ptr: Pointer to the Object struct that represents the disc.
 * length: Output parameter for the number of intersections found.
 */
void* get_disc_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
    *length = 1;
	Disc* disc_ptr =  (Disc*) ((Object*)object_ptr)->figure;
	// We get the disc's plane intersection
	((Object*)object_ptr)->figure = &(disc_ptr->plane);
	Intersection* inter = get_plane_intersection(eye, dir_vec, object_ptr, NULL);
	((Object*)object_ptr)->figure = disc_ptr;
	if(inter)
	{
		if(!is_inside_disc(inter->posn, disc_ptr->inner_focus1, disc_ptr->inner_focus2, disc_ptr->inner_dist) &&
           is_inside_disc(inter->posn, disc_ptr->ext_focus1, disc_ptr->ext_focus2, disc_ptr->ext_dist))
        {
           *length = 1;
           return inter;
        }
		else free(inter);
	}
	return NULL;
}

/*
 * Returns the normal vector of a disc on a given position. The vector is
 * already normalized.
 *
 * posn: Position at which the intersection occured
 * disc_ptr: Pointer to a disc figure.
 */
Vector get_disc_normal_vector(Vector posn, void* disc_ptr)
{
	return get_plane_normal_vector(posn, &((Disc*) disc_ptr)->plane);
}
