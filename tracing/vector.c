/* vector.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains mathematic functions used in vectors for ray tracing.
 */

// Headers
#include <stdio.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "vector.h"

// Methods

/*
 * Normalizes the given vector: it makes the vector length equal to 1.
 * This method returns the previous length of the vector.
 *
 * vector: Pointer to the vector that will be normalized
 */
long double normalize_vector(Vector *vector)
{
	long double length = sqrt(pow(vector->x, 2) + pow(vector->y, 2) + pow(vector->z, 2));
	vector->x = vector->x / length;
	vector->y = vector->y / length;
	vector->z = vector->z / length;
	return length;
}

/*
 * Returns true if there is only one distance returned in the result of a
 * cuadratic function.
 *
 * distances: Result of the function 'do_cuadratic_function'
 */
int is_one_distance(long double *distances)
{
    return distances[0] == distances[1];
}

/*
 * Uses a cuadratic function to return the distances between each intersection
 * and the eye, if any. Otherwise, it returns a NULL pointer. If there is only
 * one intersection, the array will contain the same distance twice. The first
 * distance is the one of the intersection that is nearest to the eye.
 * NOTE: A cuadratic function is described with the following formula
 * (A (+ and -) sqrt(B^2 - 4AC)) / 2A
 *
 * a: A term of the cuadratic function
 * b: B term of the cuadratic function
 * c: C term of the cuadratic function
 */
long double* do_cuadratic_function(long double a, long double b, long double c)
{
	long double * distances = get_memory(sizeof(long double) * 2, NULL);
	long double discr = pow(b, 2) - 4 * a * c;
	// There isn't any intersection
	if (discr < 0)
		return NULL;
	else
	{
		long double sqrt_disc = sqrt(discr);
		long double t1 = (- b - sqrt_disc) / (2 * a);
		long double t2 = (- b + sqrt_disc) / (2 * a);
		// Object in front of us
		if(t1 > 0 && t2 > 0)
		{
			distances[0] = t1;
			distances[1] = t2;
			return distances;
		}
		// Object behind us
		else if(t1 < 0 && t2 < 0)
			return NULL;
		// We are inside the object!
		else
		{
			distances[0] = t2;
			distances[1] = t2;
			return distances;
		}
	}
}

/*
 * Returns the position that is at 'distance' from 'anchor' in the
 * direction 'dir'.
 *
 * anchor: Position from which the ray is thrown.
 * dir: Direction towards which the ray is thrown.
 * distance: Length of the ray.
 */
Vector get_ray_position(Vector anchor, Vector dir, long double distance)
{
	Vector ray_position;
	ray_position.x = anchor.x + dir.x * distance;
	ray_position.y = anchor.y + dir.y * distance;
	ray_position.z = anchor.z + dir.z * distance;
	return ray_position;
}

/*
 * Returns the result of subtracting every coordinate in 'subtracting_vector'
 * to 'vector'.
 *
 * vector: Minuend vector.
 * subtracting_vector: Subtrahend vector.
 */
Vector subtract_vectors(Vector vector, Vector subtracting_vector)
{
	Vector new_vector = vector;
	new_vector.x -= subtracting_vector.x;
	new_vector.y -= subtracting_vector.y;
	new_vector.z -= subtracting_vector.z;
	return new_vector;
}

/*
 * Multiplies each of the vector coordinates by a scalar value
 *
 * scalar_value: Value by which all the vector coordinates will be multiplied.
 * vector: Vector that will be multiplied.
 */
Vector multiply_vector(long double scalar_value, Vector vector)
{
	Vector new_vector = vector;
	new_vector.x *= scalar_value;
	new_vector.y *= scalar_value;
	new_vector.z *= scalar_value;
	return new_vector;
}

/* Description:
 * Vector operation that returns a scalar value. It's the same as:
 * |vec1| * |vec2| * cos(a)
 * Where 'a' is the angle formed by both vectors.
 *
 * vec1: First vector with which the dot product is calculated.
 * vec2: Second vector with which the dot product is calculated.
 */
long double do_dot_product(Vector vec1, Vector vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

/*

// Description:
// Returns a vector that's perpendicular to both vectors.
VECTOR CrossProduct(VECTOR vec1, VECTOR vec2)
{
	VECTOR product;
	product.x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
	product.y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
	product.z = (vec1.x * vec2.y) - (vec1.y * vec2.x);
	return product;
}


// Description
// Gives the distance of both of the intersections with an object
// described by the given cuadratic function. The first distance is the
// nearest, the second is the farthest.
// If the object doesn't have an intersection it returns a NULL pointer.
long double* CuadraticFunctionBoth(long double a, long double b, long double c)
{
	long double * distances = GetMemory(sizeof(long double) * 2, NULL);
	long double discr = pow(b, 2) - 4 * a * c;
	// There isn't any intersection
	if (discr < 0)
		return NULL;
	else
	{
		long double sqrt_disc = sqrt(discr);
		long double t1 = (- b - sqrt_disc) / (2 * a);
		long double t2 = (- b + sqrt_disc) / (2 * a);
		// Object in front of us
		if(t1 > 0 && t2 > 0)
		{
			distances[0] = t1;
			distances[1] = t2;
			return distances;
		}
		// Object behind us
		else if(t1 < 0 && t2 < 0)
			return NULL;
		// We are inside the object!
		else
		{
			distances[0] = t2;
			distances[1] = t2;
			return distances;
		}
	}
}

// Description:
// Gets the coordinate that should be discarded if the plane is going
// to be projected to only 2 dimensions. For example: X_Y, Z_Y or X_Z
int GetDiscardedCoord(PLANE plane)
{
	// We discard the coordinate with the highest absolute value.
	if(AbsValue(plane.a) >= AbsValue(plane.b) && AbsValue(plane.a) >= AbsValue(plane.c))
		return X_COORD;
	else if (AbsValue(plane.b) >= AbsValue(plane.a) && AbsValue(plane.b) >= AbsValue(plane.c))
		return Y_COORD;
	else
		return Z_COORD;
}

// Description:
// Transforms a 3D point to a 2D coordinate.
COORD_2D TransformPoint(VECTOR point, int discarded_coord)
{
	COORD_2D vertix;
	switch(discarded_coord)
	{
		case X_COORD:
			vertix.horiz = point.z;
			vertix.verti = point.y;
			break;
		case Y_COORD:
			vertix.horiz = point.x;
			vertix.verti = point.z;
			break;
		case Z_COORD:
			vertix.horiz = point.x;
			vertix.verti = point.y;
			break;
	}
	return vertix;
}
*/
