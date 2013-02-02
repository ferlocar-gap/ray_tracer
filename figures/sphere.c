/* sphere.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all the sphere object functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "../tracing/vector.h"
#include "../tracing/object.h"
#include "../tracing/intersection.h"
#include "sphere.h"

/*
 * Returns a pointer to the intersection between a sphere and a ray.
 *
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized
 * object_ptr: Pointer to the Object struct that represents the sphere
 * length: Output parameter for the number of intersections found.
 */
void* get_sphere_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
    Intersection *inter_found;
	Sphere* sphere_ptr =  (Sphere*) ((Object*)object_ptr)->figure;
	long double a, b, c;
	Vector inter_diff;
	inter_diff.x = eye.x - sphere_ptr->center.x;
	inter_diff.y = eye.y - sphere_ptr->center.y;
	inter_diff.z = eye.z - sphere_ptr->center.z;
	// Sphere optimization: Is dir_vec length, which is always 1 because it should be normalized.
	a = 1;
	b = 2.0 * (	dir_vec.x * (inter_diff.x) +
                dir_vec.y * (inter_diff.y) +
				dir_vec.z * (inter_diff.z));
	c = pow(inter_diff.x, 2.0) +
        pow(inter_diff.y, 2.0) +
        pow(inter_diff.z, 2.0) -
        pow(sphere_ptr->radius, 2.0);

	// We obtain the distance between the eye and the intersection point
	long double *distances = do_cuadratic_function(a, b, c);
	if (!distances) return NULL;
    // We create the intersection object according to the distance
    *length = is_one_distance(distances) ? 1 : 2;
    inter_found = get_memory(sizeof(Intersection) * *length, NULL);
    int inter_i;
    for(inter_i = 0; inter_i < *length; inter_i++)
    {
        inter_found[inter_i].posn = get_ray_position(eye, dir_vec, distances[inter_i]);
        inter_found[inter_i].distance = distances[inter_i];
        inter_found[inter_i].obj = *((Object*)object_ptr);
        inter_found[inter_i].is_valid = 1;
    }
    free(distances);
    return inter_found;
}

/*
 * Returns the normal vector of a sphere on a given position. The vector is
 * already normalized.
 *
 * posn: Position at which the intersection occured
 * sphere_ptr: Pointer to a sphere figure.
 */
Vector get_sphere_normal_vector(Vector posn, void* sphere_ptr)
{
	Sphere sphere = *((Sphere*) sphere_ptr);
	Vector normal_vector = multiply_vector(1.0 / sphere.radius, subtract_vectors(posn, sphere.center));
	return normal_vector;
}

