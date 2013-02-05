/* plane.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains all the polygon object functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../utilities/memory_handler.h"
#include "../tracing/vector.h"
#include "../tracing/object.h"
#include "../tracing/intersection.h"
#include "plane.h"
#include "polygon.h"

/*
 * Transforms a 3D point to a 2D coordinate.
 *
 * point: 3D point that is transformed.
 * discarded_axis: Axis that is discarded.
 */
Coord2D transform_3d_to_2d(Vector point, Axis discarded_axis)
{
	Coord2D vertex;
	switch(discarded_axis)
	{
		case X_AXIS:
			vertex.u = point.z;
			vertex.v = point.y;
			break;
		case Y_AXIS:
			vertex.u = point.x;
			vertex.v = point.z;
			break;
		case Z_AXIS:
			vertex.u = point.x;
			vertex.v = point.y;
			break;
	}
	return vertex;
}

/*
 * Returns the axis that should be discarded if the plane were to exist in only
 * 2 dimensions. For example, the plane could be in any of these combinations:
 * X-Y, Z-Y, X-Z
 *
 * plane: Plane for which an axis needs to be discarded.
 */
Axis get_discarded_axis(Plane plane)
{
    Vector dir = plane.direction;
	// We discard the coordinate with the highest absolute value.
	if(fabsl(dir.x) >= fabsl(dir.y) && fabsl(dir.x) >= fabsl(dir.z)) return X_AXIS;
	else if (fabsl(dir.y) >= fabsl(dir.z)) return Y_AXIS;
	else return Z_AXIS;
}

/*
 * Translates a 2D coordinate by subtracting the units indicated by 'mover'.
 * Works the same as a reverse 'pan' operation.
 *
 * posn: 2D coordinate that will be translated.
 * mover: Units that the position given will be moved.
 */
Coord2D translate(Coord2D posn, Coord2D mover)
{
	posn.u -= mover.u;
	posn.v -= mover.v;
	return posn;
}

/*
 * Indicates if there is an intersection between the horizontal line drawn from
 * the origin to the right, and the border formed by 'border_beg' and
 * 'border_end'.
 *
 * border_beg: 2D coordinate at which the border begins.
 * border_end: 2D coordinate at which the border ends.
 * origin: 2D coordinate (origin) used to check intersection with the border.
 */
int is_border_valid(Coord2D border_beg, Coord2D border_end, Coord2D origin)
{
	// We translate the border to the origin
	border_beg = translate(border_beg, origin);
	border_end = translate(border_end, origin);
	// We discard and count the obvious borders
	if ((border_beg.u <= 0.0 && border_end.u <= 0.0) ||
		(border_beg.v <= 0.0 && border_end.v <= 0.0) ||
		(border_beg.v >= 0.0 && border_end.v >= 0.0))
    {
        return 0;
    }

	else if (border_beg.u > 0.0 && border_end.u > 0.0)
    {
        return 1;
    }
	else
	{   // And check for the complex ones
		long double x, m, b;
		m = (border_end.v - border_beg.v) / (border_end.u - border_beg.u);
		b = border_end.v - m * border_end.u;
		x = -b / m;
		// If x is positive when y = 0, then there is an intersection
		if (x >= 0)
			return 1;
		return 0;
	}
}

/*
 * Returns true if a given 3D point is contained within a polygon.
 *
 * polygon: Polygon in which we check whether the given point is contained.
 * point_3d: 3D point that we wish to know if belongs to the given polygon.
 */
int is_point_contained (Polygon polygon, Vector point_3d)
{
	// We transform the point to a 2D coordinate
	Coord2D point = transform_3d_to_2d(point_3d, get_discarded_axis(polygon.plane));
	Coord2D previous;
	Coord2D next = polygon.vertex[polygon.vertex_amount - 1];

	int border_counter = 0;
	int vertex_index;
	// We count the number of vertixes that intersect with a horizontal
	// line drawn to the right from the 'point'
	for(vertex_index = 0; vertex_index < polygon.vertex_amount; vertex_index++)
	{
		previous = next;
		next = polygon.vertex[vertex_index];
		// We only sum up the borders that intersect
		border_counter += is_border_valid(previous, next, point);
	}
	// If the number of borders is odd, the point is contained
	return border_counter % 2 == 1;
}

/*
 * Returns a pointer to the intersection between a polygon and a ray.
 *
 * eye: Position from which the ray is thrown.
 * dir_vec: Direction towards which the ray is thrown. Must be normalized
 * object_ptr: Pointer to the Object struct that represents the polygon
 * length: Output parameter for the number of intersections found.
 */
void* get_polygon_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length)
{
	Polygon* polygon_ptr =  (Polygon*) ((Object*)object_ptr)->figure;
	// We get the polygon's plane intersection
	((Object*)object_ptr)->figure = &(polygon_ptr->plane);
	Intersection* inter = get_plane_intersection(eye, dir_vec, object_ptr, NULL);
	((Object*)object_ptr)->figure = polygon_ptr;
	if(inter)
    {
        if(is_point_contained(*polygon_ptr, inter->posn))
        {
            *length = 1;
            return inter;
        }
		else
		{
		    free(inter);
		}
    }
	return NULL;
}

/*
 * Returns the normal vector of a polygon on a given position. The vector is
 * already normalized.
 *
 * posn: Position at which the intersection occured
 * polygon_ptr: Pointer to a polygon figure.
 */
Vector get_polygon_normal_vector(Vector posn, void* polygon_ptr)
{
	return get_plane_normal_vector(posn, &((Polygon*) polygon_ptr)->plane);
}
