#ifndef POLYGON_H
#define POLYGON_H

#include "../tracing/vector.h"
#include "plane.h"
#include "coord_2d.h"

/*
 * Represents a polygon object
 *
 * plane: Plane in which the polygon is drawn.
 * vertex_amount: Number of vertex that the polygon has.
 * vertex: List of vertex that enclose the polygon.
 */
typedef struct
{
	Plane plane;
	int vertex_amount;
	Coord2D *vertex;
} Polygon;

typedef enum { X_AXIS, Y_AXIS, Z_AXIS} Axis;

void* get_polygon_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length);
Vector get_polygon_normal_vector(Vector posn, void* polygon_ptr);
Coord2D transform_3d_to_2d(Vector point, Axis discarded_axis);
Axis get_discarded_axis(Plane plane);

#endif
