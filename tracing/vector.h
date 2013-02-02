#ifndef VECTOR_H
#define VECTOR_H

#define INVALID_DISTANCE -1

/*
 * Represents a universal coordinate. It may be used as a position or as a vector.
 *
 * x: Position in X axis.
 * y: Position in Y axis.
 * z: Position in Z axis.
 */
typedef struct
{
	long double x;
	long double y;
	long double z;
} Vector;

int is_one_distance(long double *distances);
long double normalize_vector(Vector *vector);
long double* do_cuadratic_function(long double a, long double b, long double c);
Vector get_ray_position(Vector anchor, Vector dir, long double distance);
Vector subtract_vectors(Vector vector, Vector subtracting_vector);
Vector multiply_vector(long double scalar_value, Vector vector);
long double do_dot_product(Vector vec1, Vector vec2);

#endif
