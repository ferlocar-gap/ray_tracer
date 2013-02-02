/* plane.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all the plane object functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "../tracing/vector.h"
#include "../tracing/object.h"
#include "../tracing/intersection.h"
#include "plane.h"

/*
 * Returns true if the given position is in the hemisphere at which the plane's
 * normal is pointing.
 *
 * posn: Checked position.
 * plane: Plane with which the position is being checked.
 */
int is_up_the_plane(Vector posn, Plane plane)
{
    return (plane.direction.x * posn.x +
            plane.direction.y * posn.y +
            plane.direction.z * posn.z +
            plane.offset) >= 0;
}

/*
 * Returns a pointer to the intersection between a plane and a ray.
 *
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized
 * object_ptr: Pointer to the Object struct that represents the plane
 * length: Output parameter for the number of intersections found.
 */
void* get_plane_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
	Plane* plane_ptr =  (Plane*) ((Object*)object_ptr)->figure;
	// We check the direction vector isn't paralell to the plane
	long double dir_factor =    plane_ptr->direction.x * dir_vec.x +
                                plane_ptr->direction.y * dir_vec.y +
                                plane_ptr->direction.z * dir_vec.z;
	if(dir_factor)
	{
		// We calculate the intersection with the plane and return it
		Intersection* inter_found = get_memory(sizeof(Intersection), NULL);
		inter_found->distance = - ( plane_ptr->direction.x * eye.x +
                                    plane_ptr->direction.y * eye.y +
                                    plane_ptr->direction.z * eye.z +
                                    plane_ptr->offset) / dir_factor;
		if(inter_found->distance > 0)
		{
			inter_found->posn = get_ray_position(eye, dir_vec, inter_found->distance);
			inter_found->obj = *((Object*)object_ptr);
			inter_found->is_valid = 1;
			if(length) *length = 1;
			return inter_found;
		}
		else
		{
			free(inter_found);
			return NULL;
		}
	}
	else return NULL;
}

/*
 * Returns the normal vector of a plane on a given position. The vector is
 * already normalized.
 *
 * posn: Position at which the intersection occured
 * polygon_ptr: Pointer to a plane figure.
 */
Vector get_plane_normal_vector(Vector posn, void* plane_ptr)
{
	Plane plane = *((Plane*) plane_ptr);
	return plane.direction;
}
