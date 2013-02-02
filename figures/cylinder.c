/* cylinder.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all the cylinder object functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "../tracing/vector.h"
#include "../tracing/object.h"
#include "../tracing/intersection.h"
#include "cylinder.h"

/*
 * Obtains the intersection for a cone or cylinder using the given cuadratic
 * function.
 *
 * a: A term of the cuadratic function.
 * b: B term of the cuadratic function.
 * c: C term of the cuadratic function.
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized.
 * object_ptr: Pointer to the Object struct that represents the cylinder or cone.
 * length: Output parameter for the number of intersections found.
 */
Intersection* get_cyl_cone_intersection(long double a, long double b, long double c,
	Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
    long double l_distance;
    long double *distances;
    Intersection *inter_found;
	Cylinder* cyl_ptr =  (Cylinder*) ((Object*)object_ptr)->figure;
    // We obtain the distance between the eye and both of the cylinder/cone intersections
    distances = do_cuadratic_function(a, b, c);
    if (!distances) return NULL;

    // We create the intersection object according to the distance
    *length = is_one_distance(distances) ? 1 : 2;
    inter_found = get_memory(sizeof(Intersection) * *length, NULL);
    int inter_i;
    for(inter_i = 0; inter_i < *length; inter_i++)
    {
        inter_found[inter_i].posn = get_ray_position(eye, dir_vec, distances[inter_i] - 0.001);
        inter_found[inter_i].distance = distances[inter_i];
        inter_found[inter_i].obj = *((Object*)object_ptr);
        if(cyl_ptr->is_finite)
        {
            l_distance = do_dot_product(cyl_ptr->direction, subtract_vectors(inter_found[inter_i].posn, cyl_ptr->anchor));
            // We check for the limits of the cylinder/cone
            inter_found[inter_i].is_valid = l_distance < cyl_ptr->front_length && l_distance > cyl_ptr->back_length;
        }
        else
        {
            inter_found[inter_i].is_valid = 1;
        }

    }
    free(distances);
    return inter_found;
}

/*
 * Returns a pointer to the intersection between a cylinder and a ray.
 *
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized.
 * object_ptr: Pointer to the Object struct that represents the cylinder
 * length: Output parameter for the number of intersections found.
 */
void* get_cylinder_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
    *length = 1;
	Cylinder* cyl_ptr =  (Cylinder*) ((Object*)object_ptr)->figure;

	long double termQD, termQE;
	Vector varD, varE, anchor_to_anchor;

	anchor_to_anchor = subtract_vectors(eye, cyl_ptr->anchor);

	termQD = do_dot_product(cyl_ptr->direction, dir_vec);
	termQE = do_dot_product(cyl_ptr->direction, anchor_to_anchor);

	varD = subtract_vectors(multiply_vector(termQD, cyl_ptr->direction), dir_vec);
	varE = get_ray_position(cyl_ptr->anchor, subtract_vectors(multiply_vector(termQE, cyl_ptr->direction), eye), 1);

	//Obtenemos el discriminante
	long double a = pow(varD.x, 2) + pow(varD.y, 2) + pow(varD.z, 2);
	long double b = 2 * do_dot_product(varE, varD);
	long double c = pow(varE.x, 2) + pow(varE.y, 2) + pow(varE.z, 2) - pow(cyl_ptr->radio, 2);

	return get_cyl_cone_intersection(a, b, c, eye, dir_vec, object_ptr, length);
}

/*
 * Returns the normal vector of a cylinder on a given position. The vector is
 * already normalized.
 *
 * posn: Position at which the intersection occured
 * cylinder_ptr: Pointer to a cylinder figure.
 */
Vector get_cylinder_normal_vector(Vector posn, void* cylinder_ptr)
{
	Cylinder cyl = *((Cylinder*) cylinder_ptr);
	long double m_distance = do_dot_product(cyl.direction, subtract_vectors(posn, cyl.anchor));
	Vector normal_vector = multiply_vector(1.0 / cyl.radio, subtract_vectors(posn, get_ray_position(cyl.anchor, cyl.direction, m_distance)));
	return normal_vector;
}
