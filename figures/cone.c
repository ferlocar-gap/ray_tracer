/* cone.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all the cone object functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "../tracing/vector.h"
#include "../tracing/object.h"
#include "../tracing/intersection.h"
#include "cylinder.h"
#include "cone.h"

/*
 * Returns a pointer to the intersection between a cone and a ray.
 *
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized.
 * object_ptr: Pointer to the Object struct that represents the cone.
 * length: Output parameter for the number of intersections found.
 */
void* get_cone_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
    *length = 1;
	Cone* cone_ptr =  (Cone*) ((Object*)object_ptr)->figure;
	long double termQD, termQE;
	Vector varD, varE, anchor_to_anchor;
	anchor_to_anchor = subtract_vectors(eye, cone_ptr->anchor);

	termQD = do_dot_product(cone_ptr->direction, dir_vec);
	termQE = do_dot_product(cone_ptr->direction, anchor_to_anchor);

	varD = subtract_vectors(multiply_vector(termQD, cone_ptr->direction), dir_vec);
	varE = get_ray_position(cone_ptr->anchor, subtract_vectors(multiply_vector(termQE, cone_ptr->direction), eye), 1);

	long double a = pow(varD.x, 2) + pow(varD.y, 2) + pow(varD.z, 2) - pow(cone_ptr->radio, 2) * pow(termQD, 2);
	long double b = 2 * (do_dot_product(varE, varD) - pow(cone_ptr->radio, 2) * termQD * termQE);
	long double c = pow(varE.x, 2) + pow(varE.y, 2) + pow(varE.z, 2) - pow(cone_ptr->radio, 2) * pow(termQE, 2);

	return get_cyl_cone_intersection(a, b, c, eye, dir_vec, object_ptr, length);
}

/*
 * Returns the normal vector of a cone on a given position. The vector is
 * already normalized.
 *
 * posn: Position at which the intersection occured
 * cone_ptr: Pointer to a cone figure.
 */
Vector get_cone_normal_vector(Vector posn, void* cone_ptr)
{
	Cone cone = *((Cone*) cone_ptr);
	Vector border, normal_vec;
	border = subtract_vectors(posn, cone.anchor);
	long double distance = normalize_vector(&border);
	if(do_dot_product(border, cone.direction) < 0)
		distance *= -1;
	normal_vec = multiply_vector(1.0/ fabsl(distance), subtract_vectors(posn, get_ray_position(cone.anchor, cone.direction, distance * sqrt(2))));
	return normal_vec;
}










/*



// Description:
// Obtains an intersection pointer for the intersection between a cone
// object pointer ('object_ptr') and the ray defined by 'eye' and 'dir_vector'.
void* GetConeIntersection(VECTOR eye, VECTOR dir_vector, void* object_ptr)
{
	CYLINDER* cone_ptr =  (CYLINDER*) ((OBJECT*)object_ptr)->figure;
	long double termQD, termQE;
	VECTOR varD, varE, anchor_to_anchor;
	anchor_to_anchor = SubtractVectors(eye, cone_ptr->anchor);

	termQD = DotProduct(cone_ptr->dir_vector, dir_vector);
	termQE = DotProduct(cone_ptr->dir_vector, anchor_to_anchor);

	varD = SubtractVectors(MultiplyVector(termQD, cone_ptr->dir_vector), dir_vector);
	varE = GetRayPosition(cone_ptr->anchor, SubtractVectors(MultiplyVector(termQE, cone_ptr->dir_vector), eye), 1);


	long double a = pow(varD.x, 2) + pow(varD.y, 2) + pow(varD.z, 2)
		- pow(cone_ptr->radio, 2) * pow(termQD, 2);

	long double b = 2 * (DotProduct(varE, varD) - pow(cone_ptr->radio, 2) * termQD * termQE);

	long double c = pow(varE.x, 2) + pow(varE.y, 2) + pow(varE.z, 2)
		- pow(cone_ptr->radio, 2) * pow(termQE, 2);

	return GetCylConeIntersection(a, b, c, eye, dir_vector, object_ptr);
}

// Description:
// Obtains the normal vector of the cone pointed by 'cone_ptr'
// according to the intersection point 'inter_posn'. The normal vector
// is already normalized.
VECTOR GetConeNormalVector(VECTOR inter_posn, void* cone_ptr)
{
	CYLINDER cone = *((CYLINDER*) cone_ptr);
	VECTOR border = SubtractVectors(inter_posn, cone.anchor);
	long double distance = NormalizeVector(&border);
	if(DotProduct(border, cone.dir_vector) < 0)
		distance *= -1;
	VECTOR normal_vector = MultiplyVector(1.0/ AbsValue(distance), SubtractVectors(inter_posn, GetRayPosition(cone.anchor, cone.dir_vector, distance * sqrt(2))));
	return normal_vector;
}

*/